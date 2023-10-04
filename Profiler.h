//implementation by Mark Kouris, 4/6/21 CS315

#pragma once
//allowed to use std::thread and std:: chrono
//going to need to output a .csv file like in the memory manager
//use WOW64_CONTEXT_i386 instead of CONTEXT_i386
//want the current instruction pointer for symbol info, and not return address
//do not call new in between suspend/resumethread
//to get clock cycles, use __rdtsc()
//use ctor and dtor to record the time when you enter and leave a function.
//can use __FUNCSIG__ to get the function name string literal

//these are the necessary headers for the profiler:
#include <thread>
#include <chrono>
#include <ctime>
#include <Windows.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "dbghelp.h"

#pragma comment( lib, "dbghelp" )

//context.rip contains the instruction address, which we use to get the function name
//store function name as string
//set counter to 1

struct contextData
{
    std::string functionName = "";
    int timesHit = 1;
};

class Profiler
{
public:
    Profiler(HANDLE thread);
    ~Profiler();

    void ExportData();

    //set File data
    void setFunctionName(DWORD64 addr, contextData& data);
    void setFunctionNames();
    bool isProfiling = true;
    int searchVector(std::string funcName);
    void profilingFunction();
private:
    std::vector<contextData> profiledData; //this will change to be the storage thing for each function.
    std::vector<DWORD64> instructionAddr; //this will change to be the storage thing for each function.
    HANDLE main_thread; //this stores the main thread at the time of this class's construction.
    int totalHits = 0;
};

void profilingFunctionHelper(Profiler* profiler);
