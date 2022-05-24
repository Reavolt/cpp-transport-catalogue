// NOLINTNEXTLINE
#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg
{
    using namespace std::literals;

    struct Rgb
    {
        Rgb() = default;
        Rgb(uint8_t r, uint8_t g, uint8_t b);
        uint8_t red   = 0;
        uint8_t green = 0;
        uint8_t blue  = 0;
    };

    struct Rgba
    {
        Rgba() = default;
        Rgba(uint8_t r, uint8_t g, uint8_t b, double op);
        uint8_t red     = 0;
        uint8_t green   = 0;
        uint8_t blue    = 0;
        double  opacity = 1.0;
    };

    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
    inline const Color NoneColor{"none"s};

    struct ColorPrinter
    {
        std::ostream& out;
        void          operator()(std::monostate) const;
        void          operator()(const std::string& color) const;
        void          operator()(Rgb color) const;
        void          operator()(Rgba color) const;
    };

    enum class StrokeLineCap
    {
        BUTT,
        ROUND,
        SQUARE,
    };

    std::ostream& operator<<(std::ostream& stream, StrokeLineCap str_cap);

    enum class StrokeLineJoin
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    std::ostream& operator<<(std::ostream& stream, StrokeLineJoin str_line_join);

    struct RenderContext
    {
        RenderContext(std::ostream& out) : out(out) {}

        RenderContext(std::ostream& out, int indent_step, int indent = 0) :
            out(out),
            indent_step(indent_step),
            indent(indent)
        {
        }

        RenderContext Indented() const
        {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const
        {
            for(int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        std::ostream& out;
        int           indent_step = 0;
        int           indent      = 0;
    };

    struct Point
    {
        Point() = default;
        Point(double x, double y) : x(x), y(y) {}
        double x = 0;
        double y = 0;
    };

    template<typename Owner>
    class PathProps
    {
    public:
        Owner& SetFillColor(Color color)
        {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color)
        {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeWidth(double width)
        {
            stroke_width_ = width;
            return AsOwner();
        }
        Owner& SetStrokeLineCap(StrokeLineCap line_cap)
        {
            stroke_line_cap_ = line_cap;
            return AsOwner();
        }
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            stroke_line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const
        {
            if(fill_color_)
            {
                out << " fill=\""sv;
                std::visit(ColorPrinter{out}, *fill_color_);
                out << "\""sv;
            }
            if(stroke_color_)
            {
                out << " stroke=\""sv;
                std::visit(ColorPrinter{out}, *stroke_color_);
                out << "\""sv;
            }

            if(stroke_width_)
            {
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }

            if(stroke_line_cap_)
            {
                out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
            }

            if(stroke_line_join_)
            {
                out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
            }
        }

    private:
        Owner& AsOwner()
        {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color>          fill_color_;
        std::optional<Color>          stroke_color_;
        std::optional<double>         stroke_width_;
        std::optional<StrokeLineCap>  stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
    };

    class Object
    {
    public:
        void Render(const RenderContext& context) const;
        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class Circle final : public Object, public PathProps<Circle>
    {
    public:
        Circle();
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);
        ~Circle() override = default;

    private:
        void RenderObject(const RenderContext& context) const override;

    private:
        Point  center_;
        double radius_;
    };

    // NOLINTNEXTLINE
    class Polyline final : public Object, public PathProps<Polyline>
    {
    public:
        Polyline& AddPoint(Point point);
        ~Polyline() override = default;

    private:
        void RenderObject(const RenderContext& context) const override;

    private:
        std::vector<Point> poli_line_coordinates_;
    };

    // NOLINTNEXTLINE
    class Text final : public Object, public PathProps<Text>
    {
    public:
        Text();
        Text& SetPosition(Point pos);
        Text& SetOffset(Point offset);
        Text& SetFontSize(uint32_t size);
        Text& SetFontFamily(std::string font_family);
        Text& SetFontWeight(std::string font_weight);
        Text& SetData(std::string data);
        ~Text() override = default;

    private:
        void RenderObject(const RenderContext& context) const override;
        void ReplaceServiceChars(std::string& data);

    private:
        Point                                                  position_;
        Point                                                  offset_;
        uint32_t                                               size_;
        std::string                                            font_family_;
        std::string                                            font_weight_;
        std::string                                            data_;
        const std::vector<std::pair<std::string, std::string>> replacements_ = {
          {"&", "&amp;"},
          {"\"", "&quot;"},
          {"'", "&apos;"},
          {"<", "&lt;"},
          {">", "&gt;"},
        };
    };

    class ObjectContainer
    {
    public:
        template<typename Object>
        void Add(Object object)
        {
            AddPtr(std::make_unique<Object>(std::move(object)));
        }
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
        virtual ~ObjectContainer()                         = default;
    };

    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable()                                 = default;
    };

    // NOLINTNEXTLINE
    class Document : public ObjectContainer
    {
    private:
        std::vector<std::unique_ptr<Object>> document_objects_;

    public:
        // Document();
        void AddPtr(std::unique_ptr<Object>&& obj) override;
        void Render(std::ostream& out) const;
    };
}    // namespace svg