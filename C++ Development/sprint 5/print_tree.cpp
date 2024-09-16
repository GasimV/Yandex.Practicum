#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using filesystem::path;
using filesystem::directory_iterator;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

void PrintTree(ostream& dst, const path& p, int offset = 0) {
    // Print the current directory or file
    dst << string(offset, ' ') << p.filename().string() << '\n';

    // If it's not a directory, return
    if (!filesystem::is_directory(p)) {
        return;
    }

    // Gather directory contents
    vector<filesystem::directory_entry> entries(directory_iterator(p), directory_iterator{});

    // Sort the entries in reverse alphabetical order using a custom comparator
    sort(entries.begin(), entries.end(), [](const filesystem::directory_entry& a, const filesystem::directory_entry& b) {
        return a.path().filename().string() > b.path().filename().string();
    });

    // Iterate over sorted entries and call PrintTree recursively
    for (const auto& entry : entries) {
        PrintTree(dst, entry.path(), offset + 2);
    }
}

int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"s
    );
}