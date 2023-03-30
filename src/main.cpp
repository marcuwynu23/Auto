/*
 *   Copyright (c) 2023
 *   All rights reserved.
 */
#include "lib.h"

int main(int, char const **argv)
{
  // if argv[1] is  null then set sourceFile to default '.autofile'
  std::string sourceFile = ".autofile";
  if (argv[1] != NULL)
  {
    sourceFile = argv[1];
  }
  autoRunner(sourceFile);
  return 0;
}
