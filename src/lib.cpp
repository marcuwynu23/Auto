

#include <thread>
#include <mutex>
#include <direct.h>
#include <unistd.h> 
#include <fstream>
#include <string>
#include <vector>

//user header files
#include "lib.h"



// user functions
void __termExecute(std::string command)
{
	std::mutex m;
	std::thread t{[&]
		{
			std::lock_guard<std::mutex> lock{m};
			system(command.c_str());
		}
	};
	t.join();
}


void __readStringLine(std::string fileName){
	std::fstream newfile;
	newfile.open(fileName, std::ios::in); 
	if (newfile.is_open()){  
		string tp;
		while(getline(newfile, tp)){ 
				if(tp[0] == '#'){
					continue;
				}
				else{
					__termExecute("echo "+tp+"&& start cmd /K "+tp);
				}
		}
		newfile.close();
	}
}

void autoRunner(void){
	string sourcefile = ".autofile";
	__readStringLine(sourcefile);
}