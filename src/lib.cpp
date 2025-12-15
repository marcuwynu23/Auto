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

// Static mutex for thread-safe console output
static std::mutex g_console_mutex;

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str)
{
  size_t first = str.find_first_not_of(" \t\n\r");
  if (first == std::string::npos)
    return "";
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

// Function to execute the commands synchronously
void __termExecuteSync(const std::string& command)
{
  system(command.c_str());
}

// Function to execute the commands asynchronously
void __termExecuteAsync(const std::string& command)
{
  // Capture by value to avoid use-after-free
  std::thread t{[command]
                {
                  system(command.c_str());
                }};
  t.detach();
}

// Function to print the command in the terminal (synchronously)
void __termPrintSync(int type, const std::string& str)
{
  std::lock_guard<std::mutex> lock{g_console_mutex};
  std::cout << "  type: " << type << " : " << str << std::endl;
}

// Function to print the command in the terminal (asynchronously)
void __termPrintAsync(int type, const std::string& str)
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
void replaceTabs(std::string& str)
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
void __processBlocks(const std::string& fileName, const std::string& targetBlock)
{
  std::ifstream newfile(fileName);
  int executionType = 0; // Default to SYNC
  std::string currentBlock;
  bool blockFound = false; // Flag to check if the block exists

  if (newfile.is_open())
  {
    std::string line;

    while (std::getline(newfile, line))
    {
      // Trim leading and trailing whitespaces
      line = trim(line);

      if (line.empty())
        continue;

      // Check for block definition (e.g., dev { or publish {)
      if (!line.empty() && line.back() == '{')
      {
        // Extract block name (before '{'), trim whitespace
        currentBlock = trim(line.substr(0, line.length() - 1));

        // Check if this is the target block or if the block starts with "."
        if (!currentBlock.empty() && (currentBlock == targetBlock || currentBlock.front() == '.'))
        {
          blockFound = true; // Set the flag if the block matches the target
        }
      }
      else if (line == "}")
      {
        // End of current block
        currentBlock.clear();
      }
      else if (!currentBlock.empty() && (currentBlock == targetBlock || (!currentBlock.empty() && currentBlock.front() == '.')))
      {
        // Replace tabs with spaces if "tab" keyword is found in the block
        if (line.find("tab") != std::string::npos)
        {
          replaceTabs(line);
        }

        // Check if line has at least one character before accessing
        if (line.empty())
          continue;

        // If we are in the target block or a special block starting with a dot (".")
        if (line[0] == '+')
        {
          // Process commands within a block
          std::string cmdLine = line.substr(1);
          std::vector<std::string> args = splitCommand(cmdLine);
          
          // Safety check: ensure args is not empty before accessing args[0]
          if (args.empty())
            continue;
            
          if (executionType == 1) // Async execution
          {
            __termPrintAsync(executionType, line);
            std::string command = "start \"" + args[0] + "\" cmd /K \"" + cmdLine + "\"";
            __termExecuteAsync(command);
          }
          else // Sync execution
          {
            __termPrintSync(executionType, line);
            std::string command = "start \"" + args[0] + "\" cmd /K \"" + cmdLine + "\"";
            __termExecuteSync(command);
          }
        }
        else if (line[0] == '-')
        {
          // Handle minimized execution
          std::string cmdLine = line.substr(1);
          std::vector<std::string> args = splitCommand(cmdLine);
          
          if (args.empty())
            continue;
            
          if (executionType == 1) // Async execution
          {
            __termPrintAsync(executionType, line);
            std::string command = "start /MIN \"" + args[0] + "\" cmd /K \"" + cmdLine + "\"";
            __termExecuteAsync(command);
          }
          else // Sync execution
          {
            __termPrintSync(executionType, line);
            std::string command = "start /MIN \"" + args[0] + "\" cmd /K \"" + cmdLine + "\"";
            __termExecuteSync(command);
          }
        }
        else if (line[0] == '$')
        {
          // Handle background execution
          std::string cmdLine = line.substr(1);
          std::vector<std::string> args = splitCommand(cmdLine);
          
          if (args.empty())
            continue;
            
          if (executionType == 1) // Async execution
          {
            __termPrintAsync(executionType, line);
            std::string command = "start /B \"" + args[0] + "\" cmd /C \"" + cmdLine + "\"";
            __termExecuteAsync(command);
          }
          else // Sync execution
          {
            __termPrintSync(executionType, line);
            std::string command = "start /B \"" + args[0] + "\" cmd /C \"" + cmdLine + "\"";
            __termExecuteSync(command);
          }
        }
      }
    }

    // If block is not found, print a message
    if (!blockFound)
    {
      std::cout << "Block \"" << targetBlock << "\" not found in the script." << std::endl;
    }

    std::cout << "\nEnd:\n" << std::endl;
  }
  else
  {
    std::cout << "Failed to open file: " << fileName << std::endl;
  }
}

// Main function to execute the script
void autoRunner(const std::string& sourceFile, const std::string& targetBlock)
{
  __processBlocks(sourceFile, targetBlock);
}
