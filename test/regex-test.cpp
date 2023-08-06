#include "gtest/gtest.h"
#include <gtest/gtest.h>

#include "regexp.h"

class RegexEnd2EndTest
    : public testing::TestWithParam<std::pair<std::string, std::string>> {};

TEST_P(RegexEnd2EndTest, PositiveSearches) {

  auto params = GetParam();
  auto matcher = create_matcher(params.first);

  EXPECT_TRUE(matcher(params.second));
}

INSTANTIATE_TEST_SUITE_P(
    BasicPositiveMatches, RegexEnd2EndTest,
    testing::Values(std::pair<std::string, std::string>{"", ""},

                    std::pair<std::string, std::string>{"a*", "a"},
                    std::pair<std::string, std::string>{"a*", "aaaaaaaaaaaa"},
                    std::pair<std::string, std::string>{"a*", "aa"},

                    std::pair<std::string, std::string>{"a?", "a"},
                    std::pair<std::string, std::string>{"a?", ""},

                    // std::pair<std::string, std::string>{"a+", "a"},
                    // std::pair<std::string, std::string>{"a+", "aa"},
                    // std::pair<std::string, std::string>{"a+", "aaaaa"},

                    std::pair<std::string, std::string>{"a*b", "ab"},
                    std::pair<std::string, std::string>{"a*b", "aaaaaaaaab"},
                    std::pair<std::string, std::string>{"a*b", "aab"},

                    std::pair<std::string, std::string>{"a|b", "a"},
                    std::pair<std::string, std::string>{"a|b", "b"},

                    std::pair<std::string, std::string>{"abcd|efgh", "abcd"},
                    std::pair<std::string, std::string>{"abcd|efgh", "efgh"}));