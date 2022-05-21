#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json
{
    /*
 * Вспомогательная структура, хранящая контекст для вывода с отступами.
 * Хранит ссылку на поток вывода и шаг отступа при выводе элемента
 */
    struct RenderContext
    {
        explicit RenderContext(std::ostream& out, int indent = 0) : out(out), indent(indent) {}
        void RenderIndent() const
        {
            for(int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }
        std::ostream& out;
        int           indent = 0;
    };

    class Node;
    using Dict  = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

    using NodeData = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    class Node final : private NodeData
    {
    public:
        using NodeData::variant;

        bool IsNull() const noexcept;
        bool IsBool() const noexcept;
        bool IsInt() const noexcept;
        bool IsDouble() const noexcept;
        bool IsPureDouble() const noexcept;
        bool IsString() const noexcept;
        bool IsArray() const noexcept;
        bool IsMap() const noexcept;

        // возвращает ссылку на ноду
        const Array&       AsArray() const;
        const Dict&        AsMap() const;
        bool               AsBool() const;
        int                AsInt() const;
        double             AsDouble() const;
        const std::string& AsString() const;

        friend bool operator==(const Node& lhs, const Node& rhs)
        {
            return static_cast<NodeData>(lhs) == static_cast<NodeData>(rhs);
        }
        friend bool operator!=(const Node& lhs, const Node& rhs)
        {
            return !(lhs == rhs);
        }
    };

    class Document final
    {
    public:
        explicit Document(Node root);
        const Node& GetRoot() const;
        friend bool operator==(const Document& lhs, const Document& rhs)
        {
            return lhs.root_ == rhs.root_;
        }
        friend bool operator!=(const Document& lhs, const Document& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}    // namespace json