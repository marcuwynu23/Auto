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
#include <unistd.h>
#include <vector>

// user functions
//function to execute the commands Concurrently
void __termExecuteSync(std::string command) {
  std::mutex m;
  std::thread t{[&] {
    std::lock_guard<std::mutex> lock{m};
    system(command.c_str());
  }};
  t.join();
}



//function to print the command in terminal in Parallel
void __termPrintSync(int type,std::string str) {
  std::mutex m;
  std::thread t{[&] {
    std::lock_guard<std::mutex> lock{m};
    cout <<"  type: " << type <<" : " << str.c_str() << endl;
  }};
  t.join();
}



// function for executing command in Parallel
void __termExecuteAsync(std::string command) {
 system(command.c_str());
}



//function for printing the command in terminal in Parallel
void __termPrintAsync(int type,std::string str) {
    cout <<"  type: "<< type << " : "<< str.c_str() << endl;  
}



// function to read String in A Line
void __readStringLine(std::string fileName) {
  std::fstream newfile;
  newfile.open(fileName, std::ios::in);
  int lineNumber = 1;
  int executionType = 0;


  if (newfile.is_open()) {
    string tp;
    cout << "Autofile: " << endl;
    cout << " Start:\n " << endl;
    while (getline(newfile, tp)) {
      if(lineNumber == 1){
        //check if first character of line 1 is contains : 
        if( tp.substr(0,2) == "::"){
           //remove : from the first line
          tp = tp.erase(0, 1);
          cout << "  type: "+tp << endl;
          if(tp == "SYNC"){
            executionType = 0;
          }else if(tp == "ASYNC"){
            executionType = 1;
          }
        }
      }
      //check if the autofile content length is not empty
      if (tp.length() > 0) {
        //if is not empty, check the first character in every line if
        // not contains # because this is for commenting and
        // it is contains - which is the first character of a executable command
        // to be execute.
        if (tp[0] == '+') {
          if(executionType == 1){
          //print the commands
          __termPrintAsync(executionType,tp);
          //remove the first character
          tp = tp.erase(0, 1);
          //execute the command with start -
          string command = "start \"" + tp + "\" cmd /K " + tp;
          __termExecuteAsync(command);
          }else if(executionType == 0){
               //print the commands
          __termPrintSync(executionType,tp);
          //remove the first character
          tp = tp.erase(0, 1);
          //execute the command with start -
           string command = "start \"" + tp + "\" cmd /K " + tp;
          __termExecuteSync(command);
          }
        }



        if (tp[0] == '-') {
          if(executionType == 1){
          //print the commands
          __termPrintAsync(executionType,tp);
          //remove the first character
          tp = tp.erase(0, 1);
          //execute the command with start -
          string command = "start /MIN \"" + tp + "\" cmd /K " + tp;
          __termExecuteAsync(command);
          }else if(executionType == 0){
               //print the commands
          __termPrintSync(executionType,tp);
          //remove the first character
          tp = tp.erase(0, 1);
          //execute the command with start -
           string command = "start /MIN \"" + tp + "\" cmd /K " + tp;
          __termExecuteSync(command);
          }
        }

        if(tp[0] == '#'){
          //ignore
        }
      }
      lineNumber++;
    }
    cout << "\n End:\n " << endl;
    newfile.close();
  }
}

void autoRunner(void) {
  string sourcefile = ".autofile";
  __readStringLine(sourcefile);
}
