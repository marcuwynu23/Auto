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
#include <regex>
#include <iostream>

// Function to execute the commands synchronously
void __termExecuteSync(std::string command)
{
  std::mutex m;
  std::thread t{[&]
                {
                  std::lock_guard<std::mutex> lock{m};
                  system(command.c_str());
                }};
  t.join();
}

// Function to execute the commands asynchronously
void __termExecuteAsync(std::string command)
{
  std::thread t{[&]
                {
                  system(command.c_str());
                }};
  t.detach();
}

// Function to print the command in the terminal (synchronously)
void __termPrintSync(int type, std::string str)
{
  std::mutex m;
  std::thread t{[&]
                {
                  std::lock_guard<std::mutex> lock{m};
                  std::cout << "  type: " << type << " : " << str << std::endl;
                }};
  t.join();
}

// Function to print the command in the terminal (asynchronously)
void __termPrintAsync(int type, std::string str)
{
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

// Function to read and process the script with blocks
void __processBlocks(std::string fileName, std::string targetBlock)
{
  std::fstream newfile;
  newfile.open(fileName, std::ios::in);
  int executionType = 0; // Default to SYNC
  std::string currentBlock;
  bool blockFound = false; // Flag to check if the block exists

  if (newfile.is_open())
  {
    std::string line;
    std::cout << "Autofile: " << std::endl;
    std::cout << "Start:\n"
              << std::endl;

    while (std::getline(newfile, line))
    {
      // Trim leading and trailing whitespaces
      line = std::regex_replace(line, std::regex("^ +| +$"), "");

      if (line.empty())
        continue;

      // Check for block definition (e.g., dev { or publish {)
      if (line.back() == '{')
      {
        currentBlock = line.substr(0, line.length() - 2); // Extract block name (before '{')
        std::cout << "Found block: " << currentBlock << std::endl;

        // Check if this is the target block
        if (currentBlock == targetBlock)
        {
          blockFound = true; // Set the flag if the block matches the target
        }
      }
      else if (line == "}")
      {
        // End of current block
        currentBlock = "";
      }
      else if (!currentBlock.empty() && currentBlock == targetBlock)
      {
        // If we are in the target block, process commands
        if (line[0] == '+')
        {
          // Process commands within a block
          if (executionType == 1) // Async execution
          {
            __termPrintAsync(executionType, line);
            std::vector<std::string> args = splitCommand(line.substr(1)); // Split the command into arguments
            std::string command = "start \"" + args[0] + "\" cmd /K \"" + line.substr(1) + "\"";
            __termExecuteAsync(command);
          }
          else // Sync execution
          {
            __termPrintSync(executionType, line);
            std::vector<std::string> args = splitCommand(line.substr(1)); // Split the command into arguments
            std::string command = "start \"" + args[0] + "\" cmd /K \"" + line.substr(1) + "\"";
            __termExecuteSync(command);
          }
        }
        else if (line[0] == '-')
        {
          // Handle minimized execution
          if (executionType == 1) // Async execution
          {
            __termPrintAsync(executionType, line);
            std::vector<std::string> args = splitCommand(line.substr(1));
            std::string command = "start /MIN \"" + args[0] + "\" cmd /K \"" + line.substr(1) + "\"";
            __termExecuteAsync(command);
          }
          else // Sync execution
          {
            __termPrintSync(executionType, line);
            std::vector<std::string> args = splitCommand(line.substr(1));
            std::string command = "start /MIN \"" + args[0] + "\" cmd /K \"" + line.substr(1) + "\"";
            __termExecuteSync(command);
          }
        }
        else if (line[0] == '$')
        {
          // Handle background execution
          if (executionType == 1) // Async execution
          {
            __termPrintAsync(executionType, line);
            std::vector<std::string> args = splitCommand(line.substr(1));
            std::string command = "start /B \"" + args[0] + "\" cmd /C \"" + line.substr(1) + "\"";
            __termExecuteAsync(command);
          }
          else // Sync execution
          {
            __termPrintSync(executionType, line);
            std::vector<std::string> args = splitCommand(line.substr(1));
            std::string command = "start /B \"" + args[0] + "\" cmd /C \"" + line.substr(1) + "\"";
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

    std::cout << "\nEnd:\n"
              << std::endl;
    newfile.close();
  }
  else
  {
    std::cout << "Failed to open file: " << fileName << std::endl;
  }
}

// Main function to execute the script
void autoRunner(std::string sourceFile, std::string targetBlock)
{
  __processBlocks(sourceFile, targetBlock);
}