#include "svg.h"

namespace svg {

    using namespace std::literals;

    Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
    Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, double op) : red(r), green(g), blue(b), opacity(op) {}

    std::ostream& operator<<(std::ostream& stm, const svg::Color& color) {
        std::visit(svg::PrintColor{ stm }, color);
        return stm;
    }

    void PrintColor::operator()(std::monostate) const {
        out << "none"s;
    }
    void PrintColor::operator()(const std::string& str) const {
        out << str;
    }
    void PrintColor::operator()(Rgb rgb) const {
        double r = rgb.red;
        double g = rgb.green;
        double b = rgb.blue;
        out << "rgb("s << r
            << ","s << g
            << ","s << b << ")"s;
    }
    void PrintColor::operator()(Rgba rgba)  const {
        double r = rgba.red;
        double g = rgba.green;
        double b = rgba.blue;
        double o = rgba.opacity;
        out << "rgba("s << r
            << ","s << g
            << ","s << b
            << ","s << o << ")"s;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();
        RenderObject(context);
        context.out << std::endl;
    }
    std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join) {
        switch (line_join) {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap) {
        switch (line_cap) {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
        }
        return out;
    }

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (size_t i = 0; i < points_.size(); ++i) {
            if (i > 0 && i < points_.size()) {
                out << " "sv;
            }
            out << points_[i].x << ","sv << points_[i].y;
        }
        out << "\"";
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text";
        RenderAttrs(context.out);
        out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << size_ << "\""sv;
        if (font_family_) {
            out << " font-family=\""sv << *font_family_ << "\""sv;
        }
        if (font_weight_) {
            out << " font-weight=\""sv << *font_weight_ << "\""sv;
        }
        out << ">"sv;
        for (const auto& c : data_) {
            if (c == '\"') {
                out << "&quot;"sv;
            }
            else if (c == '\'') {
                out << "&apos;"sv;
            }
            else if (c == '<') {
                out << "&lt;"sv;
            }
            else if (c == '>') {
                out << "&gt;"sv;
            }
            else if (c == '&') {
                out << "&amp;"sv;
            }
            else {
                out << c;
            }
        }
        out << "</text>"sv;
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& ptr : objects_) {
            ptr->Render(out);
        }
        out << "</svg>"sv;
    }

}  // namespace svg
