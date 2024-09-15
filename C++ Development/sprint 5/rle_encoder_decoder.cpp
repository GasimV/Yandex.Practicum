#include <iostream>
#include <string>
#include <sstream>

// Run-length encoding function
std::string rle_encode(const std::string& input, bool length_first = true) {
    std::ostringstream encoded;
    int count = 1;
    
    for (size_t i = 1; i <= input.size(); ++i) {
        if (i == input.size() || input[i] != input[i - 1]) {
            if (length_first) {
                encoded << count << input[i - 1];
            } else {
                encoded << input[i - 1] << count;
            }
            count = 1;
        } else {
            ++count;
        }
    }
    return encoded.str();
}

// Run-length decoding function
std::string rle_decode(const std::string& input, bool length_first = true) {
    std::ostringstream decoded;
    size_t i = 0;

    while (i < input.size()) {
        int count = 0;
        if (length_first) {
            // Read the number
            while (isdigit(input[i])) {
                count = count * 10 + (input[i] - '0');
                ++i;
            }
            // Read the character
            char character = input[i++];
            decoded << std::string(count, character);
        } else {
            // Read the character
            char character = input[i++];
            // Read the number
            while (isdigit(input[i])) {
                count = count * 10 + (input[i] - '0');
                ++i;
            }
            decoded << std::string(count, character);
        }
    }
    return decoded.str();
}

int main() {
    std::string input = "AAAABBBCCDAA";
    bool length_first = true;

    // Encode
    std::string encoded = rle_encode(input, length_first);
    std::cout << "Encoded: " << encoded << std::endl;

    // Decode
    std::string decoded = rle_decode(encoded, length_first);
    std::cout << "Decoded: " << decoded << std::endl;

    return 0;
}