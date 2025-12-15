/*
 *   Copyright (c) 2023
 *   All rights reserved.
 */
#include "lib.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char *argv[])
{
  std::string fileName = ".autofile"; // Default hidden file name
  std::string targetBlock;

  // Check the number of arguments
  if (argc == 2) // Only the block name is provided
  {
    targetBlock = argv[1]; // Get the block name
  }
  else if (argc == 4) // "-f" flag followed by file and block name
  {
    std::string flag = argv[1];

    if (flag != "-f")
    {
      std::cerr << "Error: First argument must be '-f' followed by a file path." << std::endl;
      return 1;
    }

    fileName = argv[2];    // Get the file name
    targetBlock = argv[3]; // Get the block name

    // Check if the file is a hidden file (starts with dot in Unix-like systems)
    if (fileName.empty() || fileName[0] != '.')
    {
      std::cerr << "Error: The file must be a hidden file (starting with '.')." << std::endl;
      return 1;
    }
  }
  else
  {
    std::cerr << "Usage: " << argv[0] << " [-f <file_path>] <block_name>" << std::endl;
    return 1;
  }

  // Proceed with the script execution
  autoRunner(fileName, targetBlock);

  return 0;
}
