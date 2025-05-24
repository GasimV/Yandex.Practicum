# C++ Preprocessor Simulation

This project is a simplified preprocessor for handling `#include` directives in C++ source files. The program recursively processes `#include` statements, resolves file paths for local and system includes, and outputs the processed content to an output file. The code can handle nested includes and searches for header files in specified directories.

## Overview

- The program reads a source C++ file and processes all `#include` directives.
- It supports both local (`#include "file.h"`) and system (`#include <file.h>`) includes.
- Local includes are first searched in the current file's directory, followed by the provided include directories.
- System includes are only searched in the provided include directories.
- When an include file is not found, an error message is printed, and processing stops.

## Dependencies

- C++17 or newer for the `<filesystem>` library.
- Standard C++ libraries: `<cassert>`, `<fstream>`, `<iostream>`, `<regex>`, `<sstream>`, `<string>`, `<vector>`, `<algorithm>`, `<cmath>`.

## Code Workflow Breakdown

Here is a detailed step-by-step breakdown of the workflow:

### 1. **Custom Path Operator**
```cpp
path operator""_p(const char* data, std::size_t sz);
```
- This is a user-defined literal that simplifies the creation of `path` objects using the `_p` suffix, making file path management more convenient.

### 2. **`Preprocess` Function**
```cpp
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories);
```
- This is the main entry point for the preprocessing.
- Takes an input file (`in_file`), an output file (`out_file`), and a list of directories (`include_directories`) where the program will look for include files.
- Opens the input file stream and output file stream. If either fails to open, it returns `false`.
- Calls `ProcessIncludes` to recursively process the includes.

### 3. **`ProcessIncludes` Function**
```cpp
bool ProcessIncludes(istream& input, ostream& output, const path& current_file, const vector<path>& include_directories);
```
- Processes each line of the input stream to detect `#include` directives using regular expressions.
- **Regular Expressions**: Used to match local (`#include "file.h"`) and system (`#include <file.h>`) includes.
  - `include_local_pattern`: Matches local includes.
  - `include_system_pattern`: Matches system includes.
- For each line:
  - If it matches an `#include` directive:
    - **Local Includes**:
      1. Tries to open the file relative to the directory of the current file.
      2. If not found, searches through the provided include directories.
    - **System Includes**:
      1. Searches for the file only in the provided include directories.
  - If the include file is found:
    - Recursively calls `ProcessIncludes` to handle nested includes.
  - If the include file is not found:
    - Prints an error message to `stdout` and returns `false`.
  - If the line does not match an `#include` directive, it is written to the output stream directly.

### 4. **File Content Extraction**
```cpp
string GetFileContents(const string& file);
```
- Reads the entire content of a file into a string for comparison in tests.
- Uses `ifstream` and `istreambuf_iterator` to efficiently read the file contents.

### 5. **Testing: `Test` Function**
```cpp
void Test();
```
- Sets up a simulated file structure in a `sources` directory to test the preprocessor.
- Creates several directories and files using `ofstream` to simulate a C++ project with multiple header files.
- Writes specific content to these files, including nested `#include` directives.
- Calls `Preprocess` to process `a.cpp`, the main source file.
- Asserts that the `Preprocess` function returns `false` when it encounters an unknown include (simulating an error case).
- Asserts that the contents of the processed output file match the expected output using `GetFileContents`.

### 6. **Main Function**
```cpp
int main();
```
- Calls the `Test` function to execute the preprocessing and verify its correctness.

## Usage

1. **Compile the Program:**
   - Ensure your compiler supports C++17 for the `<filesystem>` library.
   ```bash
   g++ -std=c++17 -o preprocessor main.cpp
   ```

2. **Run the Program:**
   - Simply run the compiled executable.
   ```bash
   ./preprocessor
   ```

3. **Program Output:**
   - The program processes the includes in `sources/a.cpp` and generates `sources/a.in`.
   - It prints messages indicating errors for unknown include files.

## How It Works

1. **Include Processing:** The program uses regular expressions to detect `#include` statements and processes them based on their type (local or system).
2. **File Search:** For local includes, it first looks in the current directory of the source file, then in the provided include directories. For system includes, it only looks in the provided include directories.
3. **Recursive Processing:** Includes are processed recursively, handling nested include files.
4. **Error Handling:** If an `#include` file is not found, an error message is printed, and the process terminates.
5. **Output:** The processed content is written to the specified output file, excluding unknown includes.

## Notes
- The program simulates how a preprocessor handles `#include` directives in a C++ file.
- It works with both local and system include files.
- The `Test` function demonstrates how nested includes are processed and verifies the output.
- If an unknown include file is encountered, the program prints an error and terminates.

## Limitations
- Does not fully emulate a real C++ preprocessor; it is simplified and only supports basic `#include` directive processing.
- Stops processing if any include file is missing, unlike some preprocessors which may continue after reporting an error.