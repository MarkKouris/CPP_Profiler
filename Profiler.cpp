//implementation by Mark Kouris, 4/6/21 CS315

#include "Profiler.h"

//static functions for getting the function name
// Initialize the debug help libarary
static void InitSym()
{
    // Only initialize once
    static bool ImghelpInitilized = false;
    if (ImghelpInitilized) { return; }

    // Initilize IMAGEHLP.DLL
    SymInitialize(GetCurrentProcess(), nullptr, true);
    // OPTIONAL: Add support for line # in stack trace using SymGetLineFromAddr()
    SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES);

    ImghelpInitilized = true;
}

//gets the function name from a given instruction address and sets the data in the reference struct
void Profiler::setFunctionName(DWORD64 addr, contextData& data)
{
    // Initialize the symbol information if not done already
    InitSym();

    // Find the symbol name
    // This weird structure allows the SYMBOL_INFO to store a string of a user
    // defined maximum length without requiring any allocations or additional pointers
    struct
    {
        SYMBOL_INFO symbol_info;
        char buffer[MAX_PATH];
    } symbol = { 0 };

    symbol.symbol_info.SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol.symbol_info.MaxNameLen = MAX_PATH;
    DWORD64 symbol_offset = 0;
    BOOL result = SymFromAddr(GetCurrentProcess(), addr, &symbol_offset, &symbol.symbol_info);

    // If an associated symbol was found, print its name
    if (result)
    {
        for (unsigned j = 0; j < symbol.symbol_info.NameLen; ++j)
        {
            if (symbol.symbol_info.Name[j] == ',')
            {
                symbol.symbol_info.Name[j] = ' ';
            }
        }
        data.functionName = symbol.symbol_info.Name;
    }
    else //print error message
    {
        printf("failed with error code: %d \n", GetLastError());
    }


}



Profiler::Profiler(HANDLE thread): main_thread(thread)
{



}

Profiler::~Profiler()
{
}

//this is the function that is called by the sleeper thread to get a screenshot of the main thread
void Profiler::profilingFunction()
{
    while (isProfiling == true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //pauses loop for a milisecond

        //this will retrieve the .rip
        SuspendThread(main_thread);
        CONTEXT context = { 0 };
        context.ContextFlags = WOW64_CONTEXT_i386 | CONTEXT_CONTROL;
        GetThreadContext(main_thread, &context);
        //here we have access to the .rip
        ResumeThread(main_thread);
        instructionAddr.push_back(context.Rip);
    }
}

//this creates the .csv file which contains all the function names and number of times hit.
void Profiler::ExportData()
{
    if (profiledData.size() != 0)
    {
        std::ofstream outputFile;
        outputFile.open("ProfileReport.csv");
        outputFile << "Function Name, Times hit, Percentage \n";

        for (int i = 0; i < profiledData.size(); ++i)
        {

            float percentage = ((float)profiledData[i].timesHit / (float)totalHits);
            outputFile << profiledData[i].functionName << ","
                       << profiledData[i].timesHit << ","
                       << percentage << std::endl;
        }

        outputFile.close();
    }
}

//iterates through all the instruction addresses and gets their function name
//then adds to the vector
void Profiler::setFunctionNames()
{
    for (int i = 0; i < instructionAddr.size(); ++i)
    {
        contextData context;
        setFunctionName(instructionAddr[i], context);
        int index = searchVector(context.functionName);
        totalHits += 1;
        if (index == -1)
        {
            profiledData.push_back(context);
        }
        else
        {
            profiledData[index].timesHit += 1;
        }
    }

}

//searches for if a certain function has already been added to the vector
int Profiler::searchVector(std::string funcName)
{
    for (int i = 0; i < profiledData.size(); ++i)
    {
        if (profiledData[i].functionName == funcName)
        {
            return i;
        }
    }
    //if reaches here, .rip is not in vector
    return -1;
}

//helper for threading since bound function could not be used
void profilingFunctionHelper(Profiler* profiler)
{
    profiler->profilingFunction();
}
