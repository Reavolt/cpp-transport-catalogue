#include "json.h"

using namespace std;

namespace json
{
    RenderContext::RenderContext(std::ostream& out, int indent = 0) : out(out), indent(indent) {}

    void RenderContext::RenderIndent() const
    {
        for(int i = 0; i < indent; ++i)
        {
            out.put(' ');
        }
    }

    namespace
    {
        // --------- вспомогательные функции для парсинга нод ---------------

        std::string ParseString(std::istream& input)
        {
            std::string line;
            char c = ' ';
            while(input.get(c))
            {
                if(c == '\\')
                {
                    char next = ' ';
                    input.get(next);
                    if(next == '\"')
                    {
                        line += '\"';
                    }
                    else if(next == 'r')
                    {
                        line += '\r';
                    }
                    else if(next == 'n')
                    {
                        line += '\n';
                    }
                    else if(next == 't')
                    {
                        line += '\t';
                    }
                    else if(next == '\\')
                    {
                        line += '\\';
                    }
                }
                else
                {
                    if(c == '\"')
                    {
                        break;
                    }
                    line += c;
                }
            }

            if(c != '\"')
            {
                throw ParsingError("Failed to parse string node : "s + line);
            }
            return line;
        }

        using Number = std::variant<int, double>;
        Number ParseNumber(std::istream& input)
        {
            using namespace std::literals;

            std::string parsed_num;

            auto read_char = [&parsed_num, &input]
            {
                parsed_num += static_cast<char>(input.get());
                if(!input)
                {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            auto read_digits = [&input, read_char]
            {
                if(!std::isdigit(input.peek()))
                {
                    throw ParsingError("A digit is expected"s);
                }
                while(std::isdigit(input.peek()))
                {
                    read_char();
                }
            };

            if(input.peek() == '-')
            {
                read_char();
            }

            if(input.peek() == '0')
            {
                read_char();
            }
            else
            {
                read_digits();
            }

            bool is_int = true;

            if(input.peek() == '.')
            {
                read_char();
                read_digits();
                is_int = false;
            }

            if(int ch = input.peek(); ch == 'e' || ch == 'E')
            {
                read_char();
                if(ch = input.peek(); ch == '+' || ch == '-')
                {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try
            {
                if(is_int)
                {
                    // Сначала пробуем преобразовать строку в int
                    try
                    {
                        return std::stoi(parsed_num);
                    }
                    catch(...)
                    {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch(...)
            {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // ------------------- функции парсинга нод -------------------------

        Node LoadNode(istream& input);

        Node LoadArray(istream& input)
        {
            Array result;
            char  c = ' ';
            while(input >> c)
            {
                if(c == ']')
                {
                    break;
                }
                if(c != ',')
                {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if(c != ']')
            {
                throw ParsingError("Failed to parse array node");
            }
            return Node(move(result));
        }

        Node LoadDict(istream& input)
        {
            Dict result;
            char c = ' ';
            input >> c;

            if(c == '}')
            {
                return Node(Dict{});
            }
            input.putback(c);

            while(input >> c)
            {
                input.putback(c);
                string key;
                auto   first_node = LoadNode(input);

                if(first_node.IsString())
                {
                    key = first_node.AsString();
                }
                else
                {
                    throw ParsingError("Failed to parse dict key");
                }

                input >> c;
                if(c != ':')
                {
                    throw ParsingError("Failed to parse dict node");
                }

                result.insert({move(key), LoadNode(input)});

                input >> c;
                if(c == '}')
                {
                    break;
                }
                if(c != ',')
                {
                    throw ParsingError("Failed to parse dict");
                }
            }

            if(c != '}')
            {
                throw ParsingError("Failed to parse dict node");
            }

            return Node(move(result));
        }

        Node LoadString(istream& input)
        {
            string line = ParseString(input);
            return Node(move(line));
        }

        Node LoadNull(istream& input)
        {
            std::string res;

            char c = ' ';

            for(int i = 0; i < 4; ++i)
            {
                if(input.get(c))
                {
                    res += c;
                }
            }

            if(res != "null")
            {
                throw ParsingError("Failed to parse null node");
            }

            return Node();
        }

        Node LoadBool(istream& input)
        {
            std::string res;
            char        c = static_cast<char>(input.peek());
            int length = c == 't' ? 4 : 5;

            for(int i = 0; i < length; ++i)
            {
                if(input.get(c))
                {
                    res += c;
                }
            }

            if(res != "true"s && res != "false"s)
            {
                throw ParsingError("Failed to parse bool node");
            }

            if(res == "true")
            {
                return Node(true);
            }
            return Node(false);
        }

        Node LoadNum(istream& input)
        {
            auto num = ParseNumber(input);
            if(std::holds_alternative<double>(num))
            {
                return Node(std::get<double>(num));
            }
            return Node(std::get<int>(num));
        }

        Node LoadNode(istream& input)
        {
            char c = ' ';
            input >> c;

            if(c == '[')
            {
                return LoadArray(input);
            }
            if(c == '{')
            {
                return LoadDict(input);
            }
            if(c == '"')
            {
                return LoadString(input);
            }
            if(c == 'n')
            {
                input.putback(c);
                return LoadNull(input);
            }
            if(std::isdigit(c) || c == '-')
            {
                input.putback(c);
                return LoadNum(input);
            }
            if(c == 't' || c == 'f')
            {
                input.putback(c);
                return LoadBool(input);
            }
            throw ParsingError("Failed to parse document"s);
        }

    }    // namespace

    // ------------ методы проверки на тип значения ---------------------

    bool Node::IsNull() const noexcept
    {
        return std::holds_alternative<std::nullptr_t>(*this);
    }
    bool Node::IsBool() const noexcept
    {
        return std::holds_alternative<bool>(*this);
    }
    bool Node::IsInt() const noexcept
    {
        return std::holds_alternative<int>(*this);
    }
    bool Node::IsDouble() const noexcept
    {
        return (std::holds_alternative<int>(*this) || std::holds_alternative<double>(*this));
    }
    bool Node::IsPureDouble() const noexcept
    {
        return std::holds_alternative<double>(*this);
    }
    bool Node::IsString() const noexcept
    {
        return std::holds_alternative<std::string>(*this);
    }
    bool Node::IsArray() const noexcept
    {
        return std::holds_alternative<Array>(*this);
    }
    bool Node::IsMap() const noexcept
    {
        return std::holds_alternative<Dict>(*this);
    }

    // -------- возврат значения определенного типа по ссылке -----------

    const Array& Node::AsArray() const
    {
        if(IsArray())
        {
            return std::get<Array>(*this);
        }
        throw std::logic_error("Node data is not array"s);
    }

    const Dict& Node::AsMap() const
    {
        if(IsMap())
        {
            return std::get<Dict>(*this);
        }
        throw std::logic_error("Node data is not map"s);
    }

    bool Node::AsBool() const
    {
        if(IsBool())
        {
            return std::get<bool>(*this);
        }
        throw std::logic_error("Node data is not bool"s);
    }

    int Node::AsInt() const
    {
        if(IsInt())
        {
            return std::get<int>(*this);
        }
        throw std::logic_error("Node data is not int"s);
    }

    double Node::AsDouble() const
    {
        if(IsInt())
        {
            return static_cast<double>(std::get<int>(*this));
        }
        if(IsPureDouble())
        {
            return std::get<double>(*this);
        }
        throw std::logic_error("Node data is not double"s);
    }

    const string& Node::AsString() const
    {
        if(IsString())
        {
            return std::get<std::string>(*this);
        }
        throw std::logic_error("Node data is not string"s);
    }

    bool operator==(const Node& lhs, const Node& rhs)
    {
        return static_cast<NodeData>(lhs) == static_cast<NodeData>(rhs);
    }

    bool operator!=(const Node& lhs, const Node& rhs)
    {
        return !(lhs == rhs);
    }

    // ------------------------------------------------------------------

    Document::Document(Node root) : root_(move(root)) {}

    const Node& Document::GetRoot() const
    {
        return root_;
    }

    // ------------------------------------------------------------------

    Document Load(istream& input)
    {
        Document result{LoadNode(input)};
        if(char c = ' '; input >> c)
        {
            throw ParsingError("Failed to parse document"s);
        }
        return result;
    }

    namespace
    {
        // -------------------------- печать нод ----------------------------

        std::string AddEscapes(const std::string& str)
        {
            std::string result;
            for(auto c : str)
            {
                if(c == '\"')
                {
                    result += "\\\"";
                }
                else if(c == '\r')
                {
                    result += "\\r";
                }
                else if(c == '\n')
                {
                    result += "\\n";
                }
                else if(c == '\\')
                {
                    result += "\\\\";
                }
                else
                {
                    result += c;
                }
            }
            return result;
        }

        void PrintNode(const Node& node, RenderContext ctx);

        void PrintNullNode(const Node&, RenderContext ctx)
        {
            ctx.out << "null"sv;
        }

        void PrintBoolNode(const Node& node, RenderContext ctx)
        {
            if(node.AsBool())
            {
                ctx.out << "true"sv;
            }
            else
            {
                ctx.out << "false"sv;
            }
        }

        void PrintIntNode(const Node& node, RenderContext ctx)
        {
            ctx.out << node.AsInt();
        }

        void PrintDoubleNode(const Node& node, RenderContext ctx)
        {
            ctx.out << node.AsDouble();
        }

        void PrintStringNode(const Node& node, RenderContext ctx)
        {
            ctx.out << "\""sv << AddEscapes(node.AsString()) << "\""sv;
        }

        void PrintArrayNode(const Node& node, RenderContext ctx)
        {
            auto arr  = node.AsArray();
            auto size = arr.size();
            if(size != 0)
            {
                ctx.out << "["sv;
                // вывожу первый элемент вне цикла, чтобы не было лишних пробелов в начале или в конце
                PrintNode(arr.at(0), ctx);
                for(auto i = 1u; i < size; ++i)
                {
                    ctx.out << ", "sv;
                    PrintNode(arr.at(i), ctx);
                }
                ctx.out << "]"sv;
            }
            else
            {
                ctx.out << "[]"sv;
            }
        }

        void PrintMapNode(const Node& node, RenderContext ctx)
        {
            auto map  = node.AsMap();
            auto size = map.size();
            if(size != 0)
            {
                ctx.out << "{"sv << std::endl;
                RenderContext map_ctx(ctx.out, ctx.indent + 2);
                map_ctx.RenderIndent();
                // вывожу первую пару вне цикла, чтобы не было лишнего переноса строки в начале или в конце
                PrintNode(map.begin()->first, map_ctx);
                map_ctx.out << ": "sv;
                PrintNode(map.begin()->second, map_ctx);
                for(auto it = std::next(map.begin()); it != map.end(); ++it)
                {
                    map_ctx.out << ","sv << std::endl;
                    map_ctx.RenderIndent();
                    PrintNode(it->first, map_ctx);
                    map_ctx.out << ": "sv;
                    PrintNode(it->second, map_ctx);
                }
                ctx.out << std::endl;
                ctx.RenderIndent();
                ctx.out << "}"sv;
            }
            else
            {
                ctx.out << "{}"sv;
            }
        }

        void PrintNode(const Node& node, RenderContext ctx)
        {
            if(node.IsNull())
            {
                PrintNullNode(node, ctx);
            }
            else if(node.IsBool())
            {
                PrintBoolNode(node, ctx);
            }
            else if(node.IsInt())
            {
                PrintIntNode(node, ctx);
            }
            else if(node.IsPureDouble())
            {
                PrintDoubleNode(node, ctx);
            }
            else if(node.IsString())
            {
                PrintStringNode(node, ctx);
            }
            else if(node.IsArray())
            {
                PrintArrayNode(node, ctx);
            }
            else if(node.IsMap())
            {
                PrintMapNode(node, ctx);
            }
        }

    }    // namespace

    void Print(const Document& doc, std::ostream& output)
    {
        RenderContext ctx(output, 0);
        PrintNode(doc.GetRoot(), ctx);
    }

}    // namespace json