#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

bool ProcessIncludes(istream& input, ostream& output, const path& current_file, const vector<path>& include_directories);

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream input(in_file);
    if (!input.is_open()) {
        return false;
    }

    ofstream output(out_file);
    if (!output.is_open()) {
        return false;
    }

    return ProcessIncludes(input, output, in_file, include_directories);
}

bool ProcessIncludes(istream& input, ostream& output, const path& current_file, const vector<path>& include_directories) {
    // Regular expressions for #include directives
    static const regex include_local_pattern("#\\s*include\\s*\"([^\"]+)\"");
    static const regex include_system_pattern("#\\s*include\\s*<([^>]+)>");

    string line;
    int line_number = 0;

    while (getline(input, line)) {
        line_number++;

        smatch match;
        if (regex_match(line, match, include_local_pattern) || regex_match(line, match, include_system_pattern)) {
            // Extract the included file path
            path include_file = match[1].str();
            ifstream include_stream;

            // Determine the search mode
            bool is_local_include = line.find('"') != string::npos;

            if (is_local_include) {
                // Local include: search relative to the current file's parent directory first
                path local_path = current_file.parent_path() / include_file;
                include_stream.open(local_path);

                // If not found, search in the include directories
                if (!include_stream.is_open()) {
                    for (const auto& dir : include_directories) {
                        include_stream.open(dir / include_file);
                        if (include_stream.is_open()) {
                            break;
                        }
                    }
                }
            } else {
                // System include: search only in the include directories
                for (const auto& dir : include_directories) {
                    include_stream.open(dir / include_file);
                    if (include_stream.is_open()) {
                        break;
                    }
                }
            }

            // Check if the include file was successfully opened
            if (!include_stream.is_open()) {
                cout << "unknown include file " << include_file.string()
                     << " at file " << current_file.string()
                     << " at line " << line_number << endl;
                return false;
            }

            // Recursively process the included file, passing the resolved path
            if (!ProcessIncludes(include_stream, output, current_file.parent_path() / include_file, include_directories)) {
                return false;
            }
        } else {
            // Not an include line, just write it to the output
            output << line << '\n';
        }
    }
    return true;
}

string GetFileContents(const string& file) {
    ifstream stream(file);
    return {istreambuf_iterator<char>(stream), istreambuf_iterator<char>()};
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n";
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include";
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n";
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n";
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n";
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n";
    }

    assert(!Preprocess("sources/a.cpp"_p, "sources/a.in"_p, {"sources/include1"_p, "sources/include2"_p}));

    ostringstream test_out;
    test_out << "// this comment before include\n"
             << "// text from b.h before include\n"
             << "// text from c.h before include\n"
             << "// std1\n"
             << "// text from c.h after include\n"
             << "// text from b.h after include\n"
             << "// text between b.h and c.h\n"
             << "// text from d.h before include\n"
             << "// std2\n"
             << "// text from d.h after include\n"
             << "\n"
             << "int SayHello() {\n"
             << "    cout << \"hello, world!\" << endl;\n";

    assert(GetFileContents("sources/a.in") == test_out.str());
}

int main() {
    Test();
}