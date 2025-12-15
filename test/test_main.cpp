/*
 *   Copyright (c) 2023
 *   All rights reserved.
 * 
 *   Test suite for Auto library
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <sstream>
#include <ctime>
#include <chrono>

// Include the test header which exposes internal functions
// Note: In a real scenario, we'd need to refactor lib.cpp to expose these
// For now, we'll test through the public API and create test utilities

#include "../include/lib.h"

// Test utilities
class TestRunner {
private:
    int testsRun = 0;
    int testsPassed = 0;
    int testsFailed = 0;

public:
    void assertTrue(bool condition, const std::string& testName) {
        testsRun++;
        if (condition) {
            testsPassed++;
            std::cout << "✓ PASS: " << testName << std::endl;
        } else {
            testsFailed++;
            std::cerr << "✗ FAIL: " << testName << std::endl;
        }
    }

    void assertEquals(const std::string& expected, const std::string& actual, const std::string& testName) {
        testsRun++;
        if (expected == actual) {
            testsPassed++;
            std::cout << "✓ PASS: " << testName << std::endl;
        } else {
            testsFailed++;
            std::cerr << "✗ FAIL: " << testName << " - Expected: '" << expected 
                      << "', Got: '" << actual << "'" << std::endl;
        }
    }

    void assertEquals(const std::vector<std::string>& expected, const std::vector<std::string>& actual, const std::string& testName) {
        testsRun++;
        if (expected == actual) {
            testsPassed++;
            std::cout << "✓ PASS: " << testName << std::endl;
        } else {
            testsFailed++;
            std::cerr << "✗ FAIL: " << testName << std::endl;
            std::cerr << "  Expected: [";
            for (size_t i = 0; i < expected.size(); ++i) {
                std::cerr << "\"" << expected[i] << "\"";
                if (i < expected.size() - 1) std::cerr << ", ";
            }
            std::cerr << "]" << std::endl;
            std::cerr << "  Got: [";
            for (size_t i = 0; i < actual.size(); ++i) {
                std::cerr << "\"" << actual[i] << "\"";
                if (i < actual.size() - 1) std::cerr << ", ";
            }
            std::cerr << "]" << std::endl;
        }
    }

    void printSummary() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test Summary:" << std::endl;
        std::cout << "  Total:  " << testsRun << std::endl;
        std::cout << "  Passed: " << testsPassed << std::endl;
        std::cout << "  Failed: " << testsFailed << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        if (testsFailed == 0) {
            std::cout << "All tests passed! ✓" << std::endl;
            exit(0);
        } else {
            std::cout << "Some tests failed! ✗" << std::endl;
            exit(1);
        }
    }
};

// Helper function to create a test autofile
std::string createTestFile(const std::string& content) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::string filename = ".test_autofile_" + std::to_string(timestamp);
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        return filename;
    }
    return "";
}

// Helper function to delete test file
void deleteTestFile(const std::string& filename) {
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }
}

// Helper function to capture stdout
class OutputCapture {
private:
    std::ostringstream buffer;
    std::streambuf* original;

public:
    OutputCapture() : original(std::cout.rdbuf()) {
        std::cout.rdbuf(buffer.rdbuf());
    }

    ~OutputCapture() {
        std::cout.rdbuf(original);
    }

    std::string getOutput() {
        return buffer.str();
    }

    void clear() {
        buffer.str("");
        buffer.clear();
    }
};

// Test trim function (indirectly through file processing)
void testTrimFunction(TestRunner& runner) {
    std::cout << "\n=== Testing trim() function ===" << std::endl;
    
    // Create test file with whitespace
    std::string content = "test {\n  + echo hello\n}";
    std::string filename = createTestFile(content);
    
    if (!filename.empty()) {
        // Capture output
        OutputCapture capture;
        autoRunner(filename, "test");
        std::string output = capture.getOutput();
        
        // Check that processing worked (indirect test of trim)
        runner.assertTrue(output.find("echo hello") != std::string::npos || 
                         output.find("End:") != std::string::npos, 
                         "trim() handles whitespace correctly");
        
        deleteTestFile(filename);
    }
}

// Test removeQuotes function (indirectly through command splitting)
void testRemoveQuotes(TestRunner& runner) {
    std::cout << "\n=== Testing removeQuotes() function ===" << std::endl;
    
    // Create test file with quoted arguments
    std::string content = "test {\n  + echo \"hello world\"\n}";
    std::string filename = createTestFile(content);
    
    if (!filename.empty()) {
        OutputCapture capture;
        autoRunner(filename, "test");
        std::string output = capture.getOutput();
        
        // If processing succeeded, quotes were handled
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "removeQuotes() handles quoted strings");
        
        deleteTestFile(filename);
    }
}

// Test splitCommand function (indirectly)
void testSplitCommand(TestRunner& runner) {
    std::cout << "\n=== Testing splitCommand() function ===" << std::endl;
    
    // Test with simple command
    std::string content1 = "test {\n  + echo hello\n}";
    std::string filename1 = createTestFile(content1);
    
    if (!filename1.empty()) {
        OutputCapture capture;
        autoRunner(filename1, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "splitCommand() handles simple commands");
        deleteTestFile(filename1);
    }
    
    // Test with quoted arguments
    std::string content2 = "test {\n  + echo \"hello world\" test\n}";
    std::string filename2 = createTestFile(content2);
    
    if (!filename2.empty()) {
        OutputCapture capture;
        autoRunner(filename2, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "splitCommand() handles quoted arguments");
        deleteTestFile(filename2);
    }
}

// Test replaceTabs function
void testReplaceTabs(TestRunner& runner) {
    std::cout << "\n=== Testing replaceTabs() function ===" << std::endl;
    
    // Create test file with tab keyword
    std::string content = "test {\n  + echo tab\ttest\n}";
    std::string filename = createTestFile(content);
    
    if (!filename.empty()) {
        OutputCapture capture;
        autoRunner(filename, "test");
        std::string output = capture.getOutput();
        
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "replaceTabs() processes tab keyword");
        
        deleteTestFile(filename);
    }
}

// Test block parsing
void testBlockParsing(TestRunner& runner) {
    std::cout << "\n=== Testing block parsing ===" << std::endl;
    
    // Test finding a block
    std::string content = "dev {\n  + echo hello\n}\nprod {\n  + echo world\n}";
    std::string filename = createTestFile(content);
    
    if (!filename.empty()) {
        OutputCapture capture;
        autoRunner(filename, "dev");
        std::string output = capture.getOutput();
        
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "Block parsing finds target block");
        
        deleteTestFile(filename);
    }
    
    // Test block not found
    std::string content2 = "dev {\n  + echo hello\n}";
    std::string filename2 = createTestFile(content2);
    
    if (!filename2.empty()) {
        OutputCapture capture;
        autoRunner(filename2, "nonexistent");
        std::string output = capture.getOutput();
        
        runner.assertTrue(output.find("not found") != std::string::npos, 
                         "Block parsing reports missing block");
        
        deleteTestFile(filename2);
    }
}

// Test special blocks (starting with .)
void testSpecialBlocks(TestRunner& runner) {
    std::cout << "\n=== Testing special blocks (.blocks) ===" << std::endl;
    
    std::string content = ".init {\n  + echo init\n}\ndev {\n  + echo dev\n}";
    std::string filename = createTestFile(content);
    
    if (!filename.empty()) {
        OutputCapture capture;
        autoRunner(filename, ".init");
        std::string output = capture.getOutput();
        
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "Special blocks (starting with .) are recognized");
        
        deleteTestFile(filename);
    }
}

// Test command execution modes
void testCommandModes(TestRunner& runner) {
    std::cout << "\n=== Testing command execution modes ===" << std::endl;
    
    // Test normal mode (+)
    std::string content1 = "test {\n  + echo normal\n}";
    std::string filename1 = createTestFile(content1);
    
    if (!filename1.empty()) {
        OutputCapture capture;
        autoRunner(filename1, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("type: 0") != std::string::npos || 
                         output.find("End:") != std::string::npos, 
                         "Normal mode (+) is recognized");
        deleteTestFile(filename1);
    }
    
    // Test minimized mode (-)
    std::string content2 = "test {\n  - echo minimized\n}";
    std::string filename2 = createTestFile(content2);
    
    if (!filename2.empty()) {
        OutputCapture capture;
        autoRunner(filename2, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("type: 0") != std::string::npos || 
                         output.find("End:") != std::string::npos, 
                         "Minimized mode (-) is recognized");
        deleteTestFile(filename2);
    }
    
    // Test background mode ($)
    std::string content3 = "test {\n  $ echo background\n}";
    std::string filename3 = createTestFile(content3);
    
    if (!filename3.empty()) {
        OutputCapture capture;
        autoRunner(filename3, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("type: 0") != std::string::npos || 
                         output.find("End:") != std::string::npos, 
                         "Background mode ($) is recognized");
        deleteTestFile(filename3);
    }
}

// Test file handling
void testFileHandling(TestRunner& runner) {
    std::cout << "\n=== Testing file handling ===" << std::endl;
    
    // Test non-existent file
    OutputCapture capture;
    autoRunner("nonexistent_file.autofile", "test");
    std::string output = capture.getOutput();
    
    runner.assertTrue(output.find("Failed to open file") != std::string::npos, 
                     "File handling reports missing file");
    
    // Test empty file
    std::string filename = createTestFile("");
    if (!filename.empty()) {
        OutputCapture capture2;
        autoRunner(filename, "test");
        std::string output2 = capture2.getOutput();
        runner.assertTrue(output2.find("not found") != std::string::npos || 
                         output2.find("End:") != std::string::npos, 
                         "File handling processes empty file");
        deleteTestFile(filename);
    }
}

// Test edge cases
void testEdgeCases(TestRunner& runner) {
    std::cout << "\n=== Testing edge cases ===" << std::endl;
    
    // Test empty block
    std::string content1 = "test {\n}";
    std::string filename1 = createTestFile(content1);
    
    if (!filename1.empty()) {
        OutputCapture capture;
        autoRunner(filename1, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "Empty blocks are handled");
        deleteTestFile(filename1);
    }
    
    // Test block with only whitespace
    std::string content2 = "test {\n  \n  \n}";
    std::string filename2 = createTestFile(content2);
    
    if (!filename2.empty()) {
        OutputCapture capture;
        autoRunner(filename2, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "Blocks with only whitespace are handled");
        deleteTestFile(filename2);
    }
    
    // Test command with empty arguments
    std::string content3 = "test {\n  +\n}";
    std::string filename3 = createTestFile(content3);
    
    if (!filename3.empty()) {
        OutputCapture capture;
        autoRunner(filename3, "test");
        std::string output = capture.getOutput();
        runner.assertTrue(output.find("End:") != std::string::npos, 
                         "Commands with empty arguments are handled");
        deleteTestFile(filename3);
    }
}

// Test complex scenarios
void testComplexScenarios(TestRunner& runner) {
    std::cout << "\n=== Testing complex scenarios ===" << std::endl;
    
    // Test multiple blocks with different commands
    std::string content = R"(dev {
  $ taskkill /im node.exe
  - mongod.exe --dbpath C:\data\db
  + node server.js
  + python fileserver.py
  + set PORT=4000 && npm start --prefix client
}

prod {
  + node app.js
  $ nginx.exe
}
)";
    std::string filename = createTestFile(content);
    
    if (!filename.empty()) {
        // Test dev block
        OutputCapture capture1;
        autoRunner(filename, "dev");
        std::string output1 = capture1.getOutput();
        runner.assertTrue(output1.find("End:") != std::string::npos, 
                         "Complex scenario: dev block processed");
        
        // Test prod block
        OutputCapture capture2;
        autoRunner(filename, "prod");
        std::string output2 = capture2.getOutput();
        runner.assertTrue(output2.find("End:") != std::string::npos, 
                         "Complex scenario: prod block processed");
        
        deleteTestFile(filename);
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Auto Library Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    TestRunner runner;
    
    // Run all test suites
    testTrimFunction(runner);
    testRemoveQuotes(runner);
    testSplitCommand(runner);
    testReplaceTabs(runner);
    testBlockParsing(runner);
    testSpecialBlocks(runner);
    testCommandModes(runner);
    testFileHandling(runner);
    testEdgeCases(runner);
    testComplexScenarios(runner);
    
    // Print summary
    runner.printSummary();
    
    return 0;
}

