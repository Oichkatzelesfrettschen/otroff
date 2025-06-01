/**
 * @file tv.cpp
 * @brief Pure C++17 Table Vertical Line Drawing Implementation
 * @author Modern C++17 Engineering Team
 * @version 3.0
 * @date 2024
 *
 * @details Pure C++17 implementation for drawing vertical lines in tables.
 *          Completely redesigned with modern C++17 principles:
 *          - Full class encapsulation and type safety
 *          - Strong typing with custom enums and types
 *          - Exception-safe design patterns
 *          - Template-based extensible architecture
 *          - RAII resource management
 *          - Compile-time optimization
 */

#include "tbl.hpp"

namespace tbl {

/**
 * @brief Forward declarations for C++17 types
 */
class TableContext;
class OutputStream;
class TableFormatting;

/**
 * @brief Type-safe enumerations for vertical line drawing
 */
enum class LineIntersection : int {
    None = 0,
    Through = 1,
    Left = 2,
    Right = 3
};

enum class BarType : char {
    None = '\0',
    Dash = '-',
    Equal = '='
};

/**
 * @brief C++17 measurement wrapper for type safety
 */
class MeasurementString {
  private:
    const char *value_;

  public:
    constexpr explicit MeasurementString(const char *val) noexcept : value_(val) {}
    constexpr const char *value() const noexcept { return value_; }

    // Factory methods
    static constexpr MeasurementString points_2() noexcept { return MeasurementString{"2p"}; }
    static constexpr MeasurementString points_neg1() noexcept { return MeasurementString{"-1p"}; }
    static constexpr MeasurementString points_1() noexcept { return MeasurementString{"1p"}; }
    static constexpr MeasurementString points_0() noexcept { return MeasurementString{"0p"}; }
    static constexpr MeasurementString points_neg2() noexcept { return MeasurementString{"-2p"}; }
    static constexpr MeasurementString vertical_minus_half_em() noexcept { return MeasurementString{"(1v-.5m)"}; }
    static constexpr MeasurementString vertical_minus_half_em_plus_point() noexcept { return MeasurementString{"(1v-.5m+1p)"}; }
    static constexpr MeasurementString half_em() noexcept { return MeasurementString{".5m"}; }
    static constexpr MeasurementString half_em_plus_point() noexcept { return MeasurementString{"(.5m+1p)"}; }
};

/**
 * @brief Pure C++17 table context interface
 */
class TableContext {
  public:
    virtual ~TableContext() = default;
    virtual int line_count() const = 0;
    virtual bool is_full_bottom(int line) const = 0;
    virtual bool is_instead_line(int line) const = 0;
    virtual bool is_all_horizontal(int line) const = 0;
    virtual char get_cell_type(int line, int column) const = 0;
    virtual const char *get_cell_content(int line, int column) const = 0;
    virtual LineIntersection get_intersection_type(int line, int column) const = 0;
};

/**
 * @brief Pure C++17 output stream interface
 */
class OutputStream {
  public:
    virtual ~OutputStream() = default;
    virtual void printf(const char *format, ...) = 0;
};

/**
 * @brief Pure C++17 table formatting interface
 */
class TableFormatting {
  public:
    virtual ~TableFormatting() = default;
    virtual int get_line_size() const = 0;
    virtual int get_linestop(int line) const = 0;
};

/**
 * @brief Pure C++17 vertical line measurement calculator
 */
class VerticalLineMeasurements {
  private:
    const TableContext &context_;

  public:
    explicit VerticalLineMeasurements(const TableContext &ctx) : context_(ctx) {}

    struct Measurements {
        const char *extension_bottom_1 = nullptr;
        const char *extension_bottom_2 = nullptr;
        const char *extension_top_1 = nullptr;
        const char *extension_top_2 = nullptr;
    };

    [[nodiscard]] Measurements calculate(int start, int end, int column, int line_width) const {
        Measurements result{};

        // Calculate bottom extensions
        if (end < context_.line_count()) {
            if (context_.is_full_bottom(end) || (!context_.is_instead_line(end) && context_.is_all_horizontal(end))) {
                result.extension_bottom_1 = MeasurementString::points_2().value();
            } else {
                switch (get_middle_bar_type(end, column)) {
                case BarType::Dash:
                    result.extension_bottom_1 = MeasurementString::vertical_minus_half_em().value();
                    break;
                case BarType::Equal:
                    result.extension_bottom_1 = MeasurementString::vertical_minus_half_em_plus_point().value();
                    break;
                default:
                    break;
                }
            }
        }

        // Calculate intersection-based bottom extension
        const auto intersection_bottom = get_intersection_type(end, column);
        if (line_width > 1) {
            switch (intersection_bottom) {
            case LineIntersection::Through:
                result.extension_bottom_2 = MeasurementString::points_neg1().value();
                break;
            case LineIntersection::Right:
                result.extension_bottom_2 = (line_width == 0) ? MeasurementString::points_1().value() : MeasurementString::points_neg1().value();
                break;
            case LineIntersection::Left:
                result.extension_bottom_2 = (line_width == 1) ? MeasurementString::points_1().value() : MeasurementString::points_neg1().value();
                break;
            default:
                break;
            }
        } else {
            switch (intersection_bottom) {
            case LineIntersection::Through:
                result.extension_bottom_2 = MeasurementString::points_neg1().value();
                break;
            case LineIntersection::Right:
            case LineIntersection::Left:
                result.extension_bottom_2 = MeasurementString::points_1().value();
                break;
            default:
                break;
            }
        }

        // Calculate top extensions
        if (start > 0) {
            int search_line = start - 1;
            while (search_line >= 0 && context_.is_instead_line(search_line)) {
                --search_line;
            }

            if (search_line >= 0 && (context_.is_full_bottom(search_line) || context_.is_all_horizontal(search_line))) {
                result.extension_top_1 = MeasurementString::points_0().value();
            } else if (search_line >= 0) {
                switch (get_middle_bar_type(search_line, column)) {
                case BarType::Dash:
                    result.extension_top_1 = MeasurementString::half_em().value();
                    break;
                case BarType::Equal:
                    result.extension_top_1 = MeasurementString::half_em_plus_point().value();
                    break;
                default:
                    result.extension_top_1 = MeasurementString::points_neg2().value();
                    break;
                }
            } else {
                result.extension_top_1 = MeasurementString::points_2().value();
            }
        } else if (start == 0 && context_.is_all_horizontal(0)) {
            result.extension_top_1 = MeasurementString::points_neg2().value();
        }

        // Calculate intersection-based top extension
        const auto intersection_top = get_intersection_type(start, column);
        if (line_width > 1) {
            switch (intersection_top) {
            case LineIntersection::Through:
                result.extension_top_2 = MeasurementString::points_1().value();
                break;
            case LineIntersection::Left:
                result.extension_top_2 = (line_width == 0) ? MeasurementString::points_1().value() : MeasurementString::points_neg1().value();
                break;
            case LineIntersection::Right:
                result.extension_top_2 = (line_width == 1) ? MeasurementString::points_1().value() : MeasurementString::points_neg1().value();
                break;
            default:
                break;
            }
        } else {
            switch (intersection_top) {
            case LineIntersection::Through:
                result.extension_top_2 = MeasurementString::points_1().value();
                break;
            case LineIntersection::Left:
            case LineIntersection::Right:
                result.extension_top_2 = MeasurementString::points_neg1().value();
                break;
            default:
                break;
            }
        }

        return result;
    }

  private:
    [[nodiscard]] BarType get_middle_bar_type(int line, int column) const {
        auto bar_type = get_middle_bar_column_type(line, column);
        if (bar_type == BarType::None && column > 0) {
            bar_type = get_middle_bar_column_type(line, column - 1);
        }
        return bar_type;
    }

    [[nodiscard]] BarType get_middle_bar_column_type(int line, int column) const {
        auto cell_type = context_.get_cell_type(line, column);

        // Skip span cells
        while (cell_type == 's' && column > 0) {
            --column;
            cell_type = context_.get_cell_type(line, column);
        }

        if (cell_type == '-' || cell_type == '=') {
            return static_cast<BarType>(cell_type);
        }

        // Check for bare entry
        const auto bare_type = get_bare_entry_type(context_.get_cell_content(line, column));
        if (bare_type != BarType::None) {
            return bare_type;
        }

        return BarType::None;
    }

    [[nodiscard]] BarType get_bare_entry_type(const char *content) const {
        if (content == nullptr) {
            return static_cast<BarType>(1); // Special case for null
        }

        if (content[1] != '\0') {
            return BarType::None;
        }

        switch (content[0]) {
        case '_':
            return BarType::Dash;
        case '=':
            return BarType::Equal;
        default:
            return BarType::None;
        }
    }

    [[nodiscard]] LineIntersection get_intersection_type(int line, int column) const {
        // This maps to the interh() function from the original
        return context_.get_intersection_type(line, column);
    }
};

/**
 * @brief Pure C++17 vertical line renderer
 */
class VerticalLineRenderer {
  private:
    OutputStream &output_;
    const TableFormatting &formatting_;

  public:
    VerticalLineRenderer(OutputStream &output, const TableFormatting &fmt)
        : output_(output), formatting_(fmt) {}

    void render_vertical_line(
        int start_line,
        int end_line,
        int column,
        int line_width,
        const VerticalLineMeasurements::Measurements &measurements) {

        int position_offset = 0;

        for (int line_index = 0; line_index < line_width; ++line_index) {
            const int position = 2 * line_index - line_width + 1;

            if (position != position_offset) {
                output_.printf("\\h'%dp'", position - position_offset);
                position_offset = position;
            }

            // Render measurement adjustments
            if (measurements.extension_bottom_1) {
                output_.printf("\\v'%s'", measurements.extension_bottom_1);
            }
            if (measurements.extension_bottom_2) {
                output_.printf("\\v'%s'", measurements.extension_bottom_2);
            }

            // Line size setting
            output_.printf("\\s\\n(%d", formatting_.get_line_size());

            // Main line drawing
            const char linestop_char = static_cast<char>(formatting_.get_linestop(start_line) + 'a' - 1);
            output_.printf("\\L'|\\n(#%cu-1v", linestop_char);
            output_.printf("\\s0");

            // Add extensions
            if (measurements.extension_top_1) {
                output_.printf("-%s", measurements.extension_top_1);
            }
            if (measurements.extension_top_2) {
                output_.printf("+%s", measurements.extension_top_2);
            }
            if (measurements.extension_bottom_1) {
                output_.printf("-%s", measurements.extension_bottom_1);
            }
            if (measurements.extension_bottom_2) {
                output_.printf("-%s", measurements.extension_bottom_2);
            }

            // Final part of line drawing
            output_.printf("'\\v'\\n(\\*(#du-\\n(#%cu+1v", linestop_char);

            if (measurements.extension_top_1) {
                output_.printf("+%s", measurements.extension_top_1);
            }
            if (measurements.extension_top_2) {
                output_.printf("-%s", measurements.extension_top_2);
            }

            output_.printf("'");
        }
    }
};

/**
 * @brief Pure C++17 vertical line drawing engine
 */
class VerticalLineDrawingEngine {
  private:
    VerticalLineMeasurements measurements_calculator_;
    VerticalLineRenderer renderer_;
    const TableContext &context_;

  public:
    VerticalLineDrawingEngine(
        const TableContext &context,
        OutputStream &output,
        const TableFormatting &formatting)
        : measurements_calculator_(context), renderer_(output, formatting), context_(context) {}

    void draw_vertical_lines(int start, int end, int column, int line_width) {
        // Adjust end line to skip instead lines
        int adjusted_end = end + 1;
        while (adjusted_end < context_.line_count() && context_.is_instead_line(adjusted_end)) {
            ++adjusted_end;
        }

        const auto measurements = measurements_calculator_.calculate(
            start, adjusted_end, column, line_width);

        renderer_.render_vertical_line(
            start, adjusted_end, column, line_width, measurements);
    }
};

/**
 * @brief Modern C++17 public interface for vertical line drawing
 */
void draw_vertical_lines(
    int start,
    int end,
    int column,
    int line_width,
    const TableContext &context,
    OutputStream &output,
    const TableFormatting &formatting) {

    VerticalLineDrawingEngine engine{context, output, formatting};
    engine.draw_vertical_lines(start, end, column, line_width);
}

// Legacy compatibility function (converted to pure C++17)
extern "C" {
void drawvert(int start, int end, int c, int lwid) {
    // Get global context - this would need to be injected in real implementation
    extern TableContext global_table_context;
    extern OutputStream global_output_stream;
    extern TableFormatting global_table_formatting;

    draw_vertical_lines(start, end, c, lwid,
                        global_table_context,
                        global_output_stream,
                        global_table_formatting);
}
} // extern "C"

} // namespace tbl
