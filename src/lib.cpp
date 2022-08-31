#include "lib.h"
#include <direct.h>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// user functions
void __termExecute(std::string command) {
  std::mutex m;
  std::thread t{[&] {
    std::lock_guard<std::mutex> lock{m};
    system(command.c_str());
  }};
  t.join();
}
void __termPrint(std::string str) {
  std::mutex m;
  std::thread t{[&] {
    std::lock_guard<std::mutex> lock{m};
    cout << str.c_str() << endl;
  }};
  t.join();
}

void __readStringLine(std::string fileName) {
  std::fstream newfile;

  newfile.open(fileName, std::ios::in);

  if (newfile.is_open()) {
    string tp;
    cout << "Autofile: " << endl;
    cout << " Start:\n " << endl;
    while (getline(newfile, tp)) {

      
      if (tp.length() > 0) {
        if (tp[0] != '#' && tp[0] == '-') {
          __termPrint(tp);
          tp = tp.erase(0, 1);
          __termExecute("start cmd /K " + tp);
        }
      }
    }
    cout << "\n End:\n " << endl;
    newfile.close();
  }
}

void autoRunner(void) {
  string sourcefile = ".autofile";
  __readStringLine(sourcefile);
}
