#include "TThread.h"
#include <process.h>

TThread::TThread(): f_FreeOnTerminate(false),f_ThreadHandle(0),f_ThreadID(0)
{
    f_StopEvent = CreateEventW(NULL,TRUE,FALSE,NULL);  
}
void TThread::Start()
{    
    f_ThreadHandle = (HANDLE)_beginthreadex(NULL,0,Wrapper,(void *)this,0,&f_ThreadID);
}

unsigned int TThread::Wrapper(void *Param)
{
    TThread * thrd = (TThread*)(Param);
    thrd->Run();    
    try{
        if (thrd->GetFreeOnTerminate())
            delete thrd;
    }
    catch(...){}      
    _endthreadex(NULL);
    return 0;
}

TThread::~TThread()
{
    CloseHandle(f_StopEvent);
}

