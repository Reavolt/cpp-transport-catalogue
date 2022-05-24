#include "svg.h"

namespace svg
{
    using namespace std::literals;

    Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

    Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, double op) : red(r), green(g), blue(b), opacity(op) {}

    // ---------- ColorPrinter ------------------

    void ColorPrinter::operator()(std::monostate) const
    {
        out << "none"s;
    }

    void ColorPrinter::operator()(const std::string& color) const
    {
        out << color;
    }

    void ColorPrinter::operator()(Rgb color) const
    {
        out << "rgb("s << static_cast<unsigned>(color.red) << ',' << static_cast<unsigned>(color.green) << ','
            << static_cast<unsigned>(color.blue) << ')';
    }

    void ColorPrinter::operator()(Rgba color) const
    {
        out << "rgba("s << static_cast<unsigned>(color.red) << ',' << static_cast<unsigned>(color.green) << ','
            << static_cast<unsigned>(color.blue) << ',' << color.opacity << ')';
    }

    // ----------  StrokeLine ------------------

    std::ostream& operator<<(std::ostream& stream, StrokeLineCap str_cap)
    {
        switch(str_cap)
        {
            case StrokeLineCap::BUTT:
            {
                return stream << "butt";
            }
            case StrokeLineCap::ROUND:
            {
                return stream << "round";
            }
            case StrokeLineCap::SQUARE:
            {
                return stream << "square";
            }
            default:
            {
                return stream << "stroke-linecap{" << static_cast<int>(str_cap) << '}';
            }
        }
    }

    std::ostream& operator<<(std::ostream& stream, StrokeLineJoin str_line_join)
    {
        switch(str_line_join)
        {
            case StrokeLineJoin::ARCS:
            {
                return stream << "arcs";
            }
            case StrokeLineJoin::BEVEL:
            {
                return stream << "bevel";
            }
            case StrokeLineJoin::MITER:
            {
                return stream << "miter";
            }
            case StrokeLineJoin::MITER_CLIP:
            {
                return stream << "miter-clip";
            }
            case StrokeLineJoin::ROUND:
            {
                return stream << "round";
            }
            default:
            {
                return stream << "stroke-linejoin{" << static_cast<int>(str_line_join) << '}';
            }
        }
    }

    void Object::Render(const RenderContext& context) const
    {
        context.RenderIndent();
        RenderObject(context);
        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle::Circle() : center_({0.0, 0.0}), radius_(1.0) {}

    Circle& Circle::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)
    {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point)
    {
        poli_line_coordinates_.emplace_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<polyline points=\"";
        std::string delimiter = "";
        for(const auto& point : poli_line_coordinates_)
        {
            out << delimiter << point.x << "," << point.y;
            delimiter = " ";
        }
        out << "\"";
        RenderAttrs(out);
        out << "/>";
    }

    // ---------- Text ------------------
    Text::Text() : position_({0.0, 0.0}), offset_({0.0, 0.0}), size_(1) {}

    Text& Text::SetPosition(Point pos)
    {
        position_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        data_ = std::move(data);
        ReplaceServiceChars(data_);
        return *this;
    }

    void Text::ReplaceServiceChars(std::string& data)
    {
        for(const auto& [from, to] : replacements_)
        {
            size_t start_pos = 0;
            while((start_pos = data.find(from, start_pos)) != std::string::npos)
            {
                data.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
        }
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text x=\""sv << position_.x << "\" y=\""sv << position_.y << "\""sv;
        out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
        out << " font-size=\""sv << size_ << "\""sv;
        if(!font_family_.empty())
        {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if(!font_weight_.empty())
        {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        RenderAttrs(out);
        out << ">" << data_ << "</text>";
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& object)
    {
        document_objects_.emplace_back(std::move(object));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for(const auto& object : document_objects_)
        {
            out << " ";
            object->Render(out);
        }
        out << "</svg>";
    }
}    // namespace svg