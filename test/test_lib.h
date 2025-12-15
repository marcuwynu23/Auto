#ifndef __TEST_LIB_H__
#define __TEST_LIB_H__

#include <string>
#include <vector>

// Test helper functions - expose internal functions for testing
std::string trim(const std::string& str);
std::string removeQuotes(const std::string& str);
std::vector<std::string> splitCommand(const std::string& command);
void replaceTabs(std::string& str);

#endif // __TEST_LIB_H__

