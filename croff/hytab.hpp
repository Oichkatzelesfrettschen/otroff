/*
 * hytab.cpp - Hyphenation Tables Implementation
 */

#include "hytab.hpp"
#include <algorithm>
#include <numeric>
#include <ranges>

namespace croff::hyphenation {

HyphenWeight HyphenationTables::evaluate_position(
    std::string_view word,
    std::size_t position,
    bool at_beginning,
    bool at_ending,
    bool has_context
) const noexcept {
    if (position == 0 || position >= word.length()) {
        return static_cast<HyphenWeight>(0);
    }

    const char left_char = word[position - 1];
    const char right_char = word[position];

    HyphenWeight total_weight{0};

    // Primary digram weight
    if (const auto weight = digram_weight(left_char, right_char)) {
        total_weight = total_weight + *weight;
    }

    // Beginning context
    if (at_beginning && position == 1) {
        if (const auto weight = beginning_weight(right_char)) {
            total_weight = total_weight + *weight;
        }
    }

    // Ending context
    if (at_ending) {
        if (const auto weight = ending_weight(left_char, right_char)) {
            total_weight = total_weight + *weight;
        }
    }

    // Mixed context (existing hyphens nearby)
    if (has_context) {
        if (const auto weight = context_weight(left_char, right_char)) {
            total_weight = total_weight + *weight;
        }
    }

    // Fallback for uncertain cases
    if (total_weight == static_cast<HyphenWeight>(0)) {
        if (const auto weight = fallback_weight(left_char, right_char)) {
            total_weight = *weight;
        }
    }

    return total_weight;
}

constexpr bool HyphenationTables::validate_tables() const noexcept {
    // Validate table sizes are correct
    static_assert(beginning_table_.size() == BXH_SIZE);
    static_assert(general_table_.size() == MATRIX_SIZE);
    static_assert(ending_table_.size() == MATRIX_SIZE);
    static_assert(context_table_.size() == MATRIX_SIZE);
    static_assert(fallback_table_.size() == MATRIX_SIZE);

    // Additional runtime validation could go here
    return true;
}

HyphenationTables::TableStats HyphenationTables::get_statistics() const noexcept {
    TableStats stats{};

    auto analyze_table = [&stats](const auto& table) {
        for (const auto weight : table) {
            const auto value = static_cast<std::int8_t>(weight);
            if (value != 0) {
                ++stats.non_zero_entries;
            }
            if (value < 0) {
                ++stats.negative_entries;
            }

            if (weight > stats.max_weight) {
                stats.max_weight = weight;
            }
            if (weight < stats.min_weight) {
                stats.min_weight = weight;
            }
        }
    };

    analyze_table(beginning_table_);
    analyze_table(general_table_);
    analyze_table(ending_table_);
    analyze_table(context_table_);
    analyze_table(fallback_table_);

    return stats;
}

HyphenationEngine::HyphenationResult
HyphenationEngine::hyphenate(std::string_view word) const {
    HyphenationResult result;

    if (word.length() < min_word_length_) {
        return result;
    }

    // Don't hyphenate too close to word boundaries
    const std::size_t start_pos = 2;
    const std::size_t end_pos = word.length() - 2;

    for (std::size_t pos = start_pos; pos < end_pos; ++pos) {
        if (!is_valid_hyphen_position(word, pos)) {
            continue;
        }

        const auto weight = calculate_position_weight(word, pos);

        if (weight > threshold_) {
            result.emplace_back(HyphenationPoint{
                .position = pos,
                .confidence = weight,
                .prefix = word.substr(0, pos),
                .suffix = word.substr(pos)
            });
        }
    }

    // Sort by confidence (highest first)
    std::ranges::sort(result, [](const auto& a, const auto& b) {
        return a.confidence > b.confidence;
    });

    return result;
}

std::optional<HyphenationEngine::HyphenationPoint>
HyphenationEngine::best_hyphenation(std::string_view word) const {
    const auto candidates = hyphenate(word);
    return candidates.empty() ? std::nullopt :
           std::optional{candidates.front()};
}

bool HyphenationEngine::should_hyphenate_at(
    std::string_view word,
    std::size_t position,
    HyphenWeight threshold
) const noexcept {
    if (!is_valid_hyphen_position(word, position)) {
        return false;
    }

    const auto weight = calculate_position_weight(word, position);
    return weight > threshold;
}

bool HyphenationEngine::is_valid_hyphen_position(
    std::string_view word,
    std::size_t position
) const noexcept {
    // Basic validity checks
    if (position == 0 || position >= word.length()) {
        return false;
    }

    // Don't hyphenate too close to boundaries
    if (position < 2 || position > word.length() - 2) {
        return false;
    }

    // Both characters must be alphabetic
    const auto left_idx = char_to_index(word[position - 1]);
    const auto right_idx = char_to_index(word[position]);

    return left_idx && right_idx;
}

HyphenWeight HyphenationEngine::calculate_position_weight(
    std::string_view word,
    std::size_t position
) const noexcept {
    const bool at_beginning = (position <= 2);
    const bool at_ending = (position >= word.length() - 2);

    // Check for existing hyphens nearby (context)
    const bool has_context = std::ranges::any_of(
        word | std::views::take(position),
        [](char c) { return c == '-'; }
    );

    return tables_.evaluate_position(word, position, at_beginning, at_ending, has_context);
}

// Legacy C interface implementation
extern "C" {
    int hytab_get_weight(char first, char second) {
        const auto weight = default_tables.digram_weight(first, second);
        return weight ? static_cast<int>(*weight) : 0;
    }

    int hytab_should_hyphenate(const char* word, int position) {
        if (!word || position < 0) {
            return 0;
        }

        HyphenationEngine engine{default_tables};
        return engine.should_hyphenate_at(word, static_cast<std::size_t>(position)) ? 1 : 0;
    }
}

} // namespace croff::hyphenation
