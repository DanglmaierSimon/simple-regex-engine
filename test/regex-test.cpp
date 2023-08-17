#include <gtest/gtest.h>

#include "regexp.h"

class PositiveTests
    : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(PositiveTests, PositiveSearches) {

  auto params = GetParam();
  auto matcher = create_matcher(params.first);

  EXPECT_TRUE(matcher(params.second));
}

INSTANTIATE_TEST_SUITE_P(
    BasicPositiveMatches, PositiveTests,
    testing::Values(std::pair<std::string, std::string>{"", ""},

                    std::pair<std::string, std::string>{"a*", "a"},
                    std::pair<std::string, std::string>{"a*", "aaaaaaaaaaaa"},
                    std::pair<std::string, std::string>{"a*", "aa"},

                    std::pair<std::string, std::string>{"a?", "a"},
                    std::pair<std::string, std::string>{"a?", ""},

                    std::pair<std::string, std::string>{"a+", "a"},
                    std::pair<std::string, std::string>{"a+", "aa"},
                    std::pair<std::string, std::string>{"a+", "aaaaa"},

                    std::pair<std::string, std::string>{"a*b", "ab"},
                    std::pair<std::string, std::string>{"a*b", "aaaaaaaaab"},
                    std::pair<std::string, std::string>{"a*b", "aab"},
                    std::pair<std::string, std::string>{"a*b", "b"},

                    std::pair<std::string, std::string>{"a|b", "a"},
                    std::pair<std::string, std::string>{"a|b", "b"},

                    std::pair<std::string, std::string>{"abcd|efgh", "abcd"},
                    std::pair<std::string, std::string>{"abcd|efgh", "efgh"}));

class NegativeTests
    : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(NegativeTests, NegativeSearches) {

  auto params = GetParam();
  auto matcher = create_matcher(params.first);

  EXPECT_FALSE(matcher(params.second));
}

INSTANTIATE_TEST_SUITE_P(
    BasicNegativeSearches, NegativeTests,
    testing::Values(
        std::pair<std::string, std::string>{"", "a"},

        std::pair<std::string, std::string>{"a*", "ab"},
        std::pair<std::string, std::string>{"a*", "aaaaavaaaaaaa"},

        std::pair<std::string, std::string>{"a?", "aaa"},
        std::pair<std::string, std::string>{"a?", "aa"},

        std::pair<std::string, std::string>{"a+", ""},

        std::pair<std::string, std::string>{"a*b", "bab"},
        std::pair<std::string, std::string>{"a*b", "bbbbb"},
        std::pair<std::string, std::string>{"a*b", "aaaaaaaaa"},

        std::pair<std::string, std::string>{"a|b", "c"},
        std::pair<std::string, std::string>{"a|b", "ab"},

        std::pair<std::string, std::string>{"abcd|efgh", "abcdefgh"},
        std::pair<std::string, std::string>{"abcd|efgh", "efghabcd"}));
