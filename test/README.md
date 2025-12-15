# Auto Library Test Suite

This directory contains comprehensive test cases for the Auto library.

## Test Coverage

The test suite covers all major functionalities:

### Core Functions
- ✅ **trim()** - String whitespace trimming
- ✅ **removeQuotes()** - Quote removal from strings
- ✅ **splitCommand()** - Command argument parsing with quote handling
- ✅ **replaceTabs()** - Tab-to-space replacement

### Block Processing
- ✅ **Block parsing** - Finding and processing named blocks
- ✅ **Special blocks** - Blocks starting with `.` (dot)
- ✅ **Block not found** - Error handling for missing blocks

### Command Execution Modes
- ✅ **Normal mode (+)** - Standard terminal execution
- ✅ **Minimized mode (-)** - Minimized window execution
- ✅ **Background mode ($)** - Background process execution

### File Handling
- ✅ **File opening** - Successful file reading
- ✅ **Missing file** - Error handling for non-existent files
- ✅ **Empty file** - Handling of empty files

### Edge Cases
- ✅ **Empty blocks** - Blocks with no commands
- ✅ **Whitespace-only blocks** - Blocks with only whitespace
- ✅ **Empty commands** - Commands with no arguments
- ✅ **Complex scenarios** - Multiple blocks with various command types

## Running Tests

### Using Make (LLVM)
```bash
make -f llvm.makefile test
make -f llvm.makefile test-run
```

### Manual Compilation
```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic -I ../include -o test.exe test/*.cpp ../src/*.cpp
./test.exe
```

## Test Structure

- `test_main.cpp` - Main test runner with all test cases
- `test_lib.h` - Test helper header (for future refactoring)

## Test Output

The test suite provides:
- ✓ Pass indicators for successful tests
- ✗ Fail indicators with detailed error messages
- Summary report with total, passed, and failed test counts

## Adding New Tests

To add new test cases:

1. Create a new test function following the pattern:
```cpp
void testNewFeature(TestRunner& runner) {
    std::cout << "\n=== Testing new feature ===" << std::endl;
    // Your test code here
    runner.assertTrue(condition, "Test description");
}
```

2. Call it in `main()`:
```cpp
testNewFeature(runner);
```

## Notes

- Tests create temporary `.test_autofile_*` files that are automatically cleaned up
- Some functions are tested indirectly through the public API (`autoRunner`)
- For more direct testing, consider refactoring internal functions to be testable

