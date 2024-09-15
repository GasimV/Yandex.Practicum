#pragma once

#include <fstream>
#include <string>

inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name) {
    using namespace std;

    // Open the input file in binary mode
    ifstream src_file(src_name, ios::binary);
    if (!src_file) {
        // Return false if the input file could not be opened
        return false;
    }

    // Open the output file in binary mode
    ofstream dst_file(dst_name, ios::binary);
    if (!dst_file) {
        return false;
    }

    // Read the input file block by block
    while (true) {
        // Read the block header
        int header = src_file.get();
        if (header == istream::traits_type::eof()) {
            break; // End of file reached
        }

        // Extract the block type and size
        unsigned char block_header = static_cast<unsigned char>(header);
        int block_type = block_header & 1;
        int data_size = (block_header >> 1) + 1;

        if (block_type == 0) {
            // Uncompressed block
            char* buffer = new char[data_size];
            src_file.read(buffer, data_size);
            dst_file.write(buffer, src_file.gcount());
            delete[] buffer;
        } else {
            // Series block
            int repeat_char = src_file.get();
            if (repeat_char == istream::traits_type::eof()) {
                break; // Unexpected end of file
            }
            char repeated_byte = static_cast<char>(repeat_char);
            dst_file.write(string(data_size, repeated_byte).c_str(), data_size);
        }
    }

    return true;
}