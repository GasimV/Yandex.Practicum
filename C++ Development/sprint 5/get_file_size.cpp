#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

size_t GetFileSize(const string& file) {
    ifstream fin(file, ios::in | ios::binary); // Open file in binary mode for accurate size measurement

    if (!fin) {
        // Failed to open the file
        return string::npos;
    }

    fin.seekg(0, ios::end); // Move to the end of the file
    size_t file_size = static_cast<size_t>(fin.tellg()); // Get the position, which represents the file size

    fin.close(); // Close the file
    return file_size;
}

int main() {
    ofstream("test.txt") << "123456789"s;
    assert(GetFileSize("test.txt"s) == 9);

    ofstream test2("test2.txt"s);
    test2.seekp(1000);
    test2 << "abc"s;
    test2.flush();

    assert(GetFileSize("test2.txt"s) == 1003);
    assert(GetFileSize("missing file name"s) == string::npos);
}