#ifndef __RUNNER_H__
#define __RUNNER_H__
#ifdef __cplusplus

#include <iostream>

// namespaces functions
using std::cout;
using std::endl;
using std::string;

extern "C"
{
#endif
	void autoRunner(string sourceFile, string targetBlock);
#ifdef __cplusplus
}
#endif
#endif
