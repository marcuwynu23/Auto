/*
 *   Copyright (c) 2023
 *   All rights reserved.
 */
#include "lib.h"
#include <direct.h>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

// Static mutex for thread-safe console output
static std::mutex g_console_mutex;

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string &str)
{
  size_t first = str.find_first_not_of(" \t\n\r");
  if (first == std::string::npos)
    return "";
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

// Helper function to safely get environment variable (cross-platform)
inline bool isTestMode()
{
#if defined(_MSC_VER)
  // MSVC: use secure CRT to avoid deprecation warning
  size_t requiredSize = 0;
  errno_t err = getenv_s(&requiredSize, nullptr, 0, "AUTO_TEST_MODE");
  if (err != 0 || requiredSize == 0)
    return false;

  char *buffer = new char[requiredSize];
  err = getenv_s(&requiredSize, buffer, requiredSize, "AUTO_TEST_MODE");
  if (err == 0)
  {
    bool result = (std::string(buffer) == "1");
    delete[] buffer;
    return result;
  }
  delete[] buffer;
  return false;
#else
  // MinGW, GCC, and other platforms: use standard getenv
  const char *testMode = std::getenv("AUTO_TEST_MODE");
  return testMode && std::string(testMode) == "1";
#endif
}

// Function to execute the commands synchronously
void __termExecuteSync(const std::string &command)
{
  // Skip actual execution in test mode (CI environments)
  if (isTestMode())
  {
    // In test mode, just print what would be executed (for testing)
    std::lock_guard<std::mutex> lock{g_console_mutex};
    std::cout << "  [TEST MODE] Would execute: " << command << std::endl;
    return;
  }
  system(command.c_str());
}

// Function to execute the commands asynchronously
void __termExecuteAsync(const std::string &command)
{
  // Skip actual execution in test mode (CI environments)
  if (isTestMode())
  {
    // In test mode, just print what would be executed (for testing)
    std::lock_guard<std::mutex> lock{g_console_mutex};
    std::cout << "  [TEST MODE] Would execute async: " << command << std::endl;
    return;
  }
  // Capture by value to avoid use-after-free
  std::thread t{[command]
                {
                  system(command.c_str());
                }};
  t.detach();
}

// Function to print the command in the terminal (synchronously)
void __termPrintSync(int type, const std::string &str)
{
  std::lock_guard<std::mutex> lock{g_console_mutex};
  std::cout << "  type: " << type << " : " << str << std::endl;
}

// Function to print the command in the terminal (asynchronously)
void __termPrintAsync(int type, const std::string &str)
{
  std::lock_guard<std::mutex> lock{g_console_mutex};
  std::cout << "  type: " << type << " : " << str << std::endl;
}

// Function to remove surrounding quotes from a string if present
std::string removeQuotes(const std::string &str)
{
  if (str.length() >= 2 && str.front() == '"' && str.back() == '"')
  {
    return str.substr(1, str.length() - 2); // Remove the surrounding quotes
  }
  return str; // Return the string as is
}

// Function to split the string by spaces, considering quoted parts as single arguments
std::vector<std::string> splitCommand(const std::string &command)
{
  std::vector<std::string> args;
  std::string currentArg;
  bool inQuotes = false;

  for (size_t i = 0; i < command.size(); ++i)
  {
    char c = command[i];
    if (c == '"' && (i == 0 || command[i - 1] != '\\')) // Handle non-escaped quote
    {
      inQuotes = !inQuotes; // Toggle the inQuotes flag
    }
    else if (c == ' ' && !inQuotes)
    {
      if (!currentArg.empty()) // Add current argument to the list if it's not empty
      {
        args.push_back(removeQuotes(currentArg)); // Remove quotes around arguments
        currentArg.clear();
      }
    }
    else
    {
      currentArg += c; // Add the character to the current argument
    }
  }

  if (!currentArg.empty()) // Add the last argument if there's any
  {
    args.push_back(removeQuotes(currentArg));
  }

  return args;
}

// Helper function to replace tabs with spaces
void replaceTabs(std::string &str)
{
  for (size_t i = 0; i < str.length(); ++i)
  {
    if (str[i] == '\t')
    {
      str.replace(i, 1, "    ");
      i += 3; // Skip the inserted spaces
    }
  }
}

// Function to read and process the script with blocks
void __processBlocks(const std::string &fileName, const std::string &targetBlock)
{
    std::ifstream newfile(fileName);
    int executionType = 0;
    std::string currentBlock;
    bool blockFound = false;

    if (!newfile.is_open())
    {
        std::cout << "Failed to open file: " << fileName << std::endl;
        return;
    }

    // Working directory for all commands = directory containing the .autofile (C++11: no filesystem)
    // Use large buffer so long paths (e.g. GitHub Actions D:\a\...\repo) and user paths work
    std::string scriptDir;
    {
        const size_t pathBufSize = 4096;
        char buf[pathBufSize];
        DWORD len = GetFullPathNameA(fileName.c_str(), (DWORD)pathBufSize, buf, nullptr);
        if (len > 0 && len < pathBufSize)
        {
            std::string fullPath(buf);
            size_t last = fullPath.find_last_of("/\\");
            scriptDir = (last != std::string::npos && last > 0)
                            ? fullPath.substr(0, last)
                            : fullPath;
        }
        if (scriptDir.empty() && _getcwd(buf, (int)pathBufSize) != nullptr)
            scriptDir = buf;
        if (scriptDir.empty())
            scriptDir = ".";
        // Avoid trailing backslash (except for root "C:\") so quoted path in /D and -d is safe
        if (scriptDir.size() > 3 && (scriptDir.back() == '\\' || scriptDir.back() == '/'))
            scriptDir.pop_back();
    }

    // Helper lambda to build the final command string (runs in scriptDir)
    auto buildCommand = [&scriptDir](const std::string &cmdLine,
                                    const std::vector<std::string> &args,
                                    const std::string &mode) -> std::string
    {
        if (args.empty())
            return "";

        // 🔥 If command already starts with wt → DO NOT wrap
        if (args[0] == "wt")
        {
            return cmdLine;
        }

        const std::string dirArg = "\"" + scriptDir + "\"";
        if (mode == "NORMAL")
            return "start /D " + dirArg + " \"" + args[0] + "\" cmd /K \"" + cmdLine + "\"";
        if (mode == "MIN")
            return "start /MIN /D " + dirArg + " \"" + args[0] + "\" cmd /K \"" + cmdLine + "\"";
        if (mode == "BG")
            return "start /B /D " + dirArg + " \"" + args[0] + "\" cmd /C \"" + cmdLine + "\"";

        return cmdLine;
    };

    std::string line;
    while (std::getline(newfile, line))
    {
        line = trim(line);
        if (line.empty())
            continue;

        // Check for block start
        if (!line.empty() && line.back() == '{')
        {
            currentBlock = trim(line.substr(0, line.length() - 1));
            if (!currentBlock.empty() &&
                (currentBlock == targetBlock || currentBlock.front() == '.'))
            {
                blockFound = true;
            }
            continue;
        }

        // Check for block end
        if (line == "}")
        {
            currentBlock.clear();
            continue;
        }

        // Only process lines inside the target block
        if (currentBlock.empty() ||
            (currentBlock != targetBlock && currentBlock.front() != '.'))
            continue;

        if (line.empty())
            continue;

        char type = line[0];

        // 🔹 Handle new '&' symbol for Windows Terminal
        if (type == '&')
        {
            std::string content = trim(line.substr(1));
            if (content.empty() || content[0] != '"')
                continue;

            // Extract quoted title
            size_t closingQuote = content.find('"', 1);
            if (closingQuote == std::string::npos)
                continue;

            std::string title = content.substr(1, closingQuote - 1);

            // Remaining command (strip surrounding quotes if present)
            std::string remaining = removeQuotes(trim(content.substr(closingQuote + 1)));
            if (remaining.empty())
                continue;

            // New tab in current window (-w 0 new-tab), start in script dir (-d), run command; ^" escapes for cmd
            std::string wtCommand =
                "wt -w 0 new-tab -d \"" + scriptDir + "\" --title \"" + title +
                "\" --suppressApplicationTitle cmd /K ^\"" + remaining + "^\"";

            if (executionType == 1)
            {
                __termPrintAsync(executionType, line);
                __termExecuteAsync(wtCommand);
            }
            else
            {
                __termPrintSync(executionType, line);
                __termExecuteSync(wtCommand);
            }
            continue;
        }

        // 🔹 Existing '+', '-', '$' handling
        if (type != '+' && type != '-' && type != '$')
            continue;

        std::string cmdLine = trim(line.substr(1));
        std::vector<std::string> args = splitCommand(cmdLine);
        if (args.empty())
            continue;

        std::string command;
        if (type == '+')
            command = buildCommand(cmdLine, args, "NORMAL");
        else if (type == '-')
            command = buildCommand(cmdLine, args, "MIN");
        else if (type == '$')
            command = buildCommand(cmdLine, args, "BG");

        if (command.empty())
            continue;

        if (executionType == 1)
        {
            __termPrintAsync(executionType, line);
            __termExecuteAsync(command);
        }
        else
        {
            __termPrintSync(executionType, line);
            __termExecuteSync(command);
        }
    }

    if (!blockFound)
    {
        std::cout << "Block \"" << targetBlock << "\" not found in the script."
                  << std::endl;
    }

    std::cout << "\nEnd:\n" << std::endl;
}
// Main function to execute the script
void autoRunner(const std::string &sourceFile, const std::string &targetBlock)
{
  __processBlocks(sourceFile, targetBlock);
}
