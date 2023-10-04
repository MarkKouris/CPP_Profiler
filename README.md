# CPP_Profiler
This is a C++ Profiler I made as part of the low level optimization class at DigiPen. 

### Implementation Details:  
- Implementing this profiler makes use of std::thread and std::chrono, along with windows.h, primarily for SymFormAddr.
- In general, this profiler will, every millisecond, freeze the main thread and take a "snapshot" of the current status.
- What this means is, it will check what function is currently being run, record the instruction address. 
- With them all saved, once the program is set for exit, the profiler will parse all of the recorded instruction addresses and retrieve the function names associated with them. 
- Once this has all been recorded, it will output it all to a .csv file for inspection.

### Integration Details:  
- Integration into a seperate project is a bit more involved than just including a header, but it is still pretty simple.
- Before the normal code begins, you will need 3 lines of code:
-   HANDLE main_thread = OpenThread(THREAD_SUSPEND_RESUME |	THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,	0,GetCurrentThreadId());
-   Profiler* profiler = new Profiler(main_thread);
-   std::thread helperThread(profilingFunctionHelper, profiler);

- Then, once normal functionality has finished, you will need a few more lines of code:
- profiler->isProfiling = false;
-	profiler->setFunctionNames();
-	profiler->ExportData();
-	helperThread.join();
-	delete profiler;

- With this, the profiler is ready to go!

### Requirements  
- In order to add this profiler to a new project, in main, you will need to include the Profiler header, "Profiler.h"
- And then, you will need to add the aforementioned lines of code before and after normal functionality.

### Output  
- For output, the filetype is .csv, which will output all the data into an excel spreadsheet.
- The data that is recorded is simply the function name and the number of times the function was hit.
