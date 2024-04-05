/*
 *   Copyright (c) 2023
 *   All rights reserved.
 */
#include "lib.h"
int main(int, char const **argv)
{
  // if argv[1] is  null then set sourceFile to default '.autofile'
  std::string SOURCE_FILE = ".autofile";
  
  if (argv[1] != NULL)
  {
    SOURCE_FILE = argv[1];
  }
  autoRunner(SOURCE_FILE);
  
  return 0;
}
