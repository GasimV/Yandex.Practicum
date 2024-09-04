#pragma once

#include <string>
#include <vector>
#include <set>

// Function to split text into words
std::vector<std::string> SplitIntoWords(const std::string& text);

// Full definition of the template function
template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}