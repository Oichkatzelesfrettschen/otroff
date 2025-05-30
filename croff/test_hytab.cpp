/*
 * test_hytab.cpp - Comprehensive test suite for hyphenation tables
 */

#include "hytab.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace croff::hyphenation::test {

class HyphenationTest : public ::testing::Test {
protected:
    HyphenationTables tables_;
    HyphenationEngine engine_{tables_};
};

TEST_F(HyphenationTest, CharToIndexConversion) {
    EXPECT_EQ(char_to_index('a'), 0);
    EXPECT_EQ(char_to_index('z'), 25);
    EXPECT_EQ(char_to_index('A'), 0);
    EXPECT_EQ(char_to_index('Z'), 25);
    EXPECT_FALSE(char_to_index('1'));
    EXPECT_FALSE(char_to_index('@'));
}

TEST_F(HyphenationTest, MatrixIndexCalculation) {
    EXPECT_EQ(matrix_index('a', 'a'), 0);
    EXPECT_EQ(matrix_index('a', 'b'), 1);
    EXPECT_EQ(matrix_index('b', 'a'), 26);
    EXPECT_EQ(matrix_index('z', 'z'), 675);
    EXPECT_FALSE(matrix_index('1', 'a'));
    EXPECT_FALSE(matrix_index('a', '1'));
}

TEST_F(HyphenationTest, BasicWeightRetrieval) {
    // Test that we can retrieve weights without crashing
    const auto weight = tables_.digram_weight('a', 'b');
    EXPECT_TRUE(weight.has_value());

    // Test boundary cases
    EXPECT_FALSE(tables_.digram_weight('1', 'a'));
    EXPECT_FALSE(tables_.beginning_weight('1'));
}

TEST_F(HyphenationTest, TableValidation) {
    EXPECT_TRUE(tables_.validate_tables());
}

TEST_F(HyphenationTest, CommonWordHyphenation) {
    const std::vector<std::pair<std::string, std::vector<std::string>>> test_cases = {
        {"computer", {"com-puter", "compu-ter"}},
        {"hyphenation", {"hy-phenation", "hyphen-ation"}},
        {"algorithm", {"al-gorithm", "algo-rithm"}},
        {"beautiful", {"beau-tiful", "beauti-ful"}},
        {"understanding", {"un-derstanding", "under-standing", "understand-ing"}}
    };

    for (const auto& [word, expected_patterns] : test_cases) {
        const auto result = engine_.hyphenate(word);
        EXPECT_FALSE(result.empty()) << "No hyphenation found for: " << word;

        // Verify that hyphenation points create valid splits
        for (const auto& point : result) {
            EXPECT_GT(point.position, 0);
            EXPECT_LT(point.position, word.length());
            EXPECT_EQ(point.prefix.length() + point.suffix.length(), word.length());
        }
    }
}

TEST_F(HyphenationTest, ShortWordHandling) {
    // Short words should not be hyphenated
    const std::vector<std::string> short_words = {"a", "an", "the", "cat", "dog"};

    for (const auto& word : short_words) {
        const auto result = engine_.hyphenate(word);
        EXPECT_TRUE(result.empty()) << "Short word should not be hyphenated: " << word;
    }
}

TEST_F(HyphenationTest, BoundaryConditions) {
    // Test edge cases
    EXPECT_TRUE(engine_.hyphenate("").empty());
    EXPECT_FALSE(engine_.should_hyphenate_at("test", 0));
    EXPECT_FALSE(engine_.should_hyphenate_at("test", 4));
    EXPECT_FALSE(engine_.should_hyphenate_at("", 0));
}

TEST_F(HyphenationTest, EngineConfiguration) {
    // Test threshold setting
    engine_.set_threshold(static_cast<HyphenWeight>(100));
    const auto high_threshold_result = engine_.hyphenate("hyphenation");

    engine_.set_threshold(static_cast<HyphenWeight>(10));
    const auto low_threshold_result = engine_.hyphenate("hyphenation");

    EXPECT_LE(high_threshold_result.size(), low_threshold_result.size());
}

TEST_F(HyphenationTest, TableStatistics) {
    const auto stats = tables_.get_statistics();
    EXPECT_GT(stats.non_zero_entries, 0);
    EXPECT_GE(stats.negative_entries, 0);
}

TEST_F(HyphenationTest, LegacyCInterface) {
    // Test legacy C interface
    const int weight = hytab_get_weight('a', 'b');
    EXPECT_NE(weight, -999); // Should return valid weight or 0

    const int should_hyphen = hytab_should_hyphenate("computer", 3);
    EXPECT_GE(should_hyphen, 0);
    EXPECT_LE(should_hyphen, 1);
}

} // namespace croff::hyphenation::test
