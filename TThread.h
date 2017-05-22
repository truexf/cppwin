#ifndef TTHREAD_H_
#define TTHREAD_H_

#include <Windows.h>
#include <process.h>


/*
封装的线程类
使用方法，从TThread派生一个具体的线程类，然后重写Run()成员函数，在里面写线程要执行的代码，例如：
class TMyThread: public TThread
{
protected:
void Run(){//在这里写线程要执行的代码}
}
...
TMyThread my_thrd = new TMyThread();
my_thrd.Start();

成员说明：
Start()  开始线程执行
SetTerminated()  标记结束线程
GetTerminated()  线程是否标记为结束
SetFreeOnTerminate() 设置线程执行完以后自动是否当前线程对象
GetFreeOnTerminate() 是否自动是否线程对象
GetThreadHandle() 线程句柄
GetThreadID() 线程ID
*/
class TThread
{
private:  
    HANDLE f_ThreadHandle; //线程Handle
    unsigned int f_ThreadID; //线程ID
    bool f_FreeOnTerminate; //线程结束时自动删除线程对象  
    static unsigned int __stdcall Wrapper(void *Param);    
    TThread(const TThread&);
    TThread& operator=(const TThread&);    
public:
    TThread(); //构造函数
    virtual ~TThread();
    void Start();//开始
    void SetTerminated(bool v){if(v) SetEvent(f_StopEvent); else ResetEvent(f_StopEvent);} //标记停止
    bool GetTerminated(){return (WAIT_OBJECT_0 == WaitForSingleObject(f_StopEvent,0));}//是否标记为停止  
    bool WaitforTerminate(DWORD ms){return WAIT_OBJECT_0 == WaitForSingleObject(f_StopEvent,ms);} //等待停止
    void SetFreeOnTerminate(bool v){f_FreeOnTerminate = v;} //
    bool GetFreeOnTerminate(){return f_FreeOnTerminate;}
    HANDLE GetThreadHandle(){return f_ThreadHandle;}
    unsigned int GetThreadID(){return f_ThreadID;}  
protected:  
    virtual void Run()=0;  
protected:
    HANDLE f_StopEvent; //标记停止
};

class AutoCriticalSection
{
private:
  CRITICAL_SECTION *f_lock;
  AutoCriticalSection(const AutoCriticalSection&);
  AutoCriticalSection& operator=(const AutoCriticalSection&);
public:
  AutoCriticalSection(){};
  explicit AutoCriticalSection(CRITICAL_SECTION &cri)
  {
	  f_lock = &cri;
	  EnterCriticalSection(f_lock);
  }  
  ~AutoCriticalSection(){LeaveCriticalSection(f_lock);}
};

class AutoSetEvent
{
private:
  HANDLE f_Event;  
public:
  AutoSetEvent(HANDLE evt):f_Event(evt){}
  ~AutoSetEvent(){SetEvent(f_Event);}
};

class AutoCloseHandle
{
private:
  HANDLE f_Handle;
public:  
  AutoCloseHandle(const HANDLE &h):f_Handle(h){}
  ~AutoCloseHandle(){if (f_Handle) CloseHandle(f_Handle);}
};



class BaseFuncClass
{
public:
  virtual ~BaseFuncClass(){};
};

template<typename OBJTYPE,typename FUNCTYPE>
class ObjFuncClass0: public BaseFuncClass
{
public:  
  ObjFuncClass0(OBJTYPE *obj,FUNCTYPE func){m_func = func;m_obj = obj;}
  ~ObjFuncClass0(){if (m_func && m_obj) m_obj->*m_func();}
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1>
class ObjFuncClass1: public BaseFuncClass
{
public:  
  ObjFuncClass1(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1){m_func = func;m_obj = obj; m_arg1 = arg1;}
  ~ObjFuncClass1()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2>
class ObjFuncClass2: public BaseFuncClass
{
public:  
  ObjFuncClass2(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
  }
  ~ObjFuncClass2()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3>
class ObjFuncClass3: public BaseFuncClass
{
public:  
  ObjFuncClass3(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
  }
  ~ObjFuncClass3()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4
>
class ObjFuncClass4: public BaseFuncClass
{
public:  
  ObjFuncClass4(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
    m_arg4 = arg4; 
  }
  ~ObjFuncClass4()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3,m_arg4);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5
>
class ObjFuncClass5: public BaseFuncClass
{
public:  
  ObjFuncClass5(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4,ARGTYPE5 arg5)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
    m_arg4 = arg4; 
    m_arg5 = arg5; 
  }
  ~ObjFuncClass5()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
>
class ObjFuncClass6: public BaseFuncClass
{
public:  
  ObjFuncClass6(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
    ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
    m_arg4 = arg4; 
    m_arg5 = arg5; 
    m_arg6 = arg6; 
  }
  ~ObjFuncClass6()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
,typename ARGTYPE7
>
class ObjFuncClass7: public BaseFuncClass
{
public:  
  ObjFuncClass7(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
    ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
    ,ARGTYPE7 arg7)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
    m_arg4 = arg4; 
    m_arg5 = arg5; 
    m_arg6 = arg6; 
    m_arg7 = arg7; 
  }
  ~ObjFuncClass7()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6,m_arg7);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
  ARGTYPE7 m_arg7;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
,typename ARGTYPE7,typename ARGTYPE8
>
class ObjFuncClass8: public BaseFuncClass
{
public:  
  ObjFuncClass8(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
    ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
    ,ARGTYPE7 arg7,ARGTYPE8 arg8)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
    m_arg4 = arg4; 
    m_arg5 = arg5; 
    m_arg6 = arg6; 
    m_arg7 = arg7; 
    m_arg8 = arg8; 
  }
  ~ObjFuncClass8()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6,m_arg7,m_arg8);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
  ARGTYPE7 m_arg7;
  ARGTYPE8 m_arg8;
};

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
,typename ARGTYPE7,typename ARGTYPE8,typename ARGTYPE9
>
class ObjFuncClass9: public BaseFuncClass
{
public:  
  ObjFuncClass9(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
    ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
    ,ARGTYPE7 arg7,ARGTYPE8 arg8,ARGTYPE9 arg9)
  {
    m_func = func;
    m_obj = obj; 
    m_arg1 = arg1; 
    m_arg2 = arg2; 
    m_arg3 = arg3; 
    m_arg4 = arg4; 
    m_arg5 = arg5; 
    m_arg6 = arg6; 
    m_arg7 = arg7; 
    m_arg8 = arg8; 
    m_arg9 = arg9; 
  }
  ~ObjFuncClass9()
  {
    if (m_func && m_obj) 
      (m_obj->*m_func)(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6,m_arg7,m_arg8,m_arg9);
  }
private:  
  FUNCTYPE m_func;    
  OBJTYPE *m_obj;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
  ARGTYPE7 m_arg7;
  ARGTYPE8 m_arg8;
  ARGTYPE9 m_arg9;
};

template<typename OBJTYPE,typename FUNCTYPE>
ObjFuncClass0<OBJTYPE,FUNCTYPE> *ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func)
{
  return new ObjFuncClass0<OBJTYPE,FUNCTYPE>(obj,func);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1>
ObjFuncClass1<OBJTYPE,FUNCTYPE,ARGTYPE1> *ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1)
{
  return new ObjFuncClass1<OBJTYPE,FUNCTYPE,ARGTYPE1>(obj,func,arg1);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2>
ObjFuncClass2<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2> *ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2)
{
  return new ObjFuncClass2<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2>(obj,func,arg1,arg2);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3>
ObjFuncClass3<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3)
{
  return new ObjFuncClass3<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3>(obj,func,arg1,arg2,arg3);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
  ,typename ARGTYPE4>
ObjFuncClass4<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
             ,ARGTYPE4 arg4)
{
  return new ObjFuncClass4<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4>(obj,func,arg1,arg2,arg3,arg4);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5
>
ObjFuncClass5<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
             ,ARGTYPE4 arg4,ARGTYPE5 arg5
             )
{
  return new ObjFuncClass5<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5>(obj,func,arg1,arg2,arg3
    ,arg4,arg5);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
>
ObjFuncClass6<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
             ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
             )
{
  return new ObjFuncClass6<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6>
    (obj,func,arg1,arg2,arg3
    ,arg4,arg5,arg6);
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
,typename ARGTYPE7
>
ObjFuncClass7<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6
,ARGTYPE7> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
             ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
             ,ARGTYPE4 arg7
             )
{
  return new ObjFuncClass7<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6
    ,ARGTYPE7>
    (obj,func,arg1,arg2,arg3
    ,arg4,arg5,arg6
    ,arg7
    );
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
,typename ARGTYPE7,typename ARGTYPE8
>
ObjFuncClass8<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6
,ARGTYPE7,ARGTYPE8> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
             ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
             ,ARGTYPE4 arg7,ARGTYPE8 arg8
             )
{
  return new ObjFuncClass8<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6
    ,ARGTYPE7,ARGTYPE8>
    (obj,func,arg1,arg2,arg3
    ,arg4,arg5,arg6
    ,arg7,arg8
    );
}

template<typename OBJTYPE,typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3
,typename ARGTYPE4,typename ARGTYPE5,typename ARGTYPE6
,typename ARGTYPE7,typename ARGTYPE8,typename ARGTYPE9
>
ObjFuncClass9<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6
,ARGTYPE7,ARGTYPE8,ARGTYPE9> 
*ObjAutoFunc(OBJTYPE *obj,FUNCTYPE func
             ,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3
             ,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
             ,ARGTYPE4 arg7,ARGTYPE8 arg8,ARGTYPE9 arg9
             )
{
  return new ObjFuncClass9<OBJTYPE,FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6
  ,ARGTYPE7,ARGTYPE8,ARGTYPE9>
    (obj,func,arg1,arg2,arg3
    ,arg4,arg5,arg6
    ,arg7,arg8,arg9
    );
}


template<typename FUNCTYPE>
class FuncClass0: public BaseFuncClass
{
public:  
  FuncClass0(FUNCTYPE func){m_func = func;}
  ~FuncClass0(){if (m_func) m_func();}
private:  
  FUNCTYPE m_func;    
};


template<typename FUNCTYPE,typename ARGTYPE1>
class FuncClass1: public BaseFuncClass
{
public:  
  FuncClass1(FUNCTYPE func,ARGTYPE1 arg1)
  {
    m_func = func;
    m_arg1 = arg1;
  }
  ~FuncClass1(){if (m_func) m_func(m_arg1);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2>
class FuncClass2: public BaseFuncClass
{
public:  
  FuncClass2(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
  }
  ~FuncClass2(){if (m_func) m_func(m_arg1,m_arg2);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3>
class FuncClass3: public BaseFuncClass
{
public:
  FuncClass3(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
  }
  ~FuncClass3(){if (m_func) m_func(m_arg1,m_arg2,m_arg3);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4>
class FuncClass4: public BaseFuncClass
{
public:
  FuncClass4(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
    m_arg4 = arg4;
  }
  ~FuncClass4(){if (m_func) m_func(m_arg1,m_arg2,m_arg3,m_arg4);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4,typename ARGTYPE5>
class FuncClass5: public BaseFuncClass
{
public:
  FuncClass5(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4,ARGTYPE5 arg5)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
    m_arg4 = arg4;
    m_arg5 = arg5;
  }
  ~FuncClass5(){if (m_func) m_func(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4,typename ARGTYPE5
,typename ARGTYPE6>
class FuncClass6: public BaseFuncClass
{
public:
  FuncClass6(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
    m_arg4 = arg4;
    m_arg5 = arg5;
    m_arg6 = arg6;
  }
  ~FuncClass6(){if (m_func) m_func(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4,typename ARGTYPE5
,typename ARGTYPE6,typename ARGTYPE7>
class FuncClass7: public BaseFuncClass
{
public:
  FuncClass7(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
    ,ARGTYPE7 arg7)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
    m_arg4 = arg4;
    m_arg5 = arg5;
    m_arg6 = arg6;
    m_arg7 = arg7;
  }
  ~FuncClass7(){if (m_func) m_func(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6,m_arg7);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
  ARGTYPE7 m_arg7;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4,typename ARGTYPE5
,typename ARGTYPE6,typename ARGTYPE7,typename ARGTYPE8>
class FuncClass8: public BaseFuncClass
{
public:
  FuncClass8(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
    ,ARGTYPE7 arg7,ARGTYPE8 arg8)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
    m_arg4 = arg4;
    m_arg5 = arg5;
    m_arg6 = arg6;
    m_arg7 = arg7;
    m_arg8 = arg8;
  }
  ~FuncClass8(){if (m_func) m_func(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6,m_arg7,m_arg8);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
  ARGTYPE7 m_arg7;
  ARGTYPE8 m_arg8;
};

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4,typename ARGTYPE5
,typename ARGTYPE6,typename ARGTYPE7,typename ARGTYPE8,typename ARGTYPE9>
class FuncClass9: public BaseFuncClass
{
public:
  FuncClass9(FUNCTYPE func,ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6
    ,ARGTYPE7 arg7,ARGTYPE8 arg8,ARGTYPE9 arg9)
  {
    m_func = func;
    m_arg1 = arg1;
    m_arg2 = arg2;
    m_arg3 = arg3;
    m_arg4 = arg4;
    m_arg5 = arg5;
    m_arg6 = arg6;
    m_arg7 = arg7;
    m_arg8 = arg8;
    m_arg9 = arg9;
  }
  ~FuncClass9(){if (m_func) m_func(m_arg1,m_arg2,m_arg3,m_arg4,m_arg5,m_arg6,m_arg7,m_arg8,m_arg9);}
private:  
  FUNCTYPE m_func;
  ARGTYPE1 m_arg1;
  ARGTYPE2 m_arg2;
  ARGTYPE3 m_arg3;
  ARGTYPE4 m_arg4;
  ARGTYPE5 m_arg5;
  ARGTYPE6 m_arg6;
  ARGTYPE7 m_arg7;
  ARGTYPE8 m_arg8;
  ARGTYPE9 m_arg9;
};

//...3,4,5,6,7,8,9

template<typename FUNCTYPE>
FuncClass0<FUNCTYPE> *AutoFunc(FUNCTYPE func_ptr)
{  
  FuncClass0<FUNCTYPE> *ptr = new FuncClass0<FUNCTYPE>(func_ptr);
  return ptr;
}

template<typename FUNCTYPE,typename ARGTYPE1>
FuncClass1<FUNCTYPE,ARGTYPE1> *AutoFunc(FUNCTYPE func_ptr,ARGTYPE1 arg1)
{  
  return (new FuncClass1<FUNCTYPE,ARGTYPE1>(func_ptr,arg1));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2>
FuncClass2<FUNCTYPE,ARGTYPE1,ARGTYPE2> *AutoFunc(FUNCTYPE func_ptr,ARGTYPE1 arg1,ARGTYPE2 arg2)
{
  return (new FuncClass2<FUNCTYPE,ARGTYPE1,ARGTYPE2>(func_ptr,arg1,arg2));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3>
FuncClass3<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3>
*AutoFunc(FUNCTYPE func_ptr,
          ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3)
{
  return (new FuncClass3<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3>(func_ptr,arg1,arg2,arg3));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4>
FuncClass4<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4>
*AutoFunc(FUNCTYPE func_ptr,
          ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,
          ARGTYPE4 arg4)
{
  return (new FuncClass4<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4>(func_ptr,arg1,arg2,arg3,arg4));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4
,typename ARGTYPE5>
FuncClass5<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,
ARGTYPE5> *AutoFunc(FUNCTYPE func_ptr,
                    ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,
                    ARGTYPE4 arg4,ARGTYPE5 arg5)
{
  return (new FuncClass5<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5>(func_ptr,arg1,arg2,arg3,arg4,arg5));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4
,typename ARGTYPE5,typename ARGTYPE6>
FuncClass6<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,
ARGTYPE5,ARGTYPE6> *AutoFunc(FUNCTYPE func_ptr,
                             ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,
                             ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6)
{
  return (new FuncClass6<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6>
    (func_ptr,arg1,arg2,arg3,arg4,arg5,arg6));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4
,typename ARGTYPE5,typename ARGTYPE6,typename ARGTYPE7>
FuncClass7<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,
ARGTYPE5,ARGTYPE6,ARGTYPE7> *AutoFunc(FUNCTYPE func_ptr,
                                      ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,
                                      ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6,
                                      ARGTYPE7 arg7)
{
  return (new FuncClass7<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6,ARGTYPE7>
    (func_ptr,arg1,arg2,arg3,arg4,arg5,arg6,arg7));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4
,typename ARGTYPE5,typename ARGTYPE6,typename ARGTYPE7,typename ARGTYPE8>
FuncClass8<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,
ARGTYPE5,ARGTYPE6,ARGTYPE7,ARGTYPE8> *AutoFunc(FUNCTYPE func_ptr,
                                               ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,
                                               ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6,
                                               ARGTYPE7 arg7,ARGTYPE8 arg8)
{
  return (new FuncClass8<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6,ARGTYPE7,ARGTYPE8>
    (func_ptr,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8));
}

template<typename FUNCTYPE,typename ARGTYPE1,typename ARGTYPE2,typename ARGTYPE3,typename ARGTYPE4
,typename ARGTYPE5,typename ARGTYPE6,typename ARGTYPE7,typename ARGTYPE8,typename ARGTYPE9>
FuncClass9<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,
ARGTYPE5,ARGTYPE6,ARGTYPE7,ARGTYPE8,ARGTYPE9> *AutoFunc(FUNCTYPE func_ptr,
                                                        ARGTYPE1 arg1,ARGTYPE2 arg2,ARGTYPE3 arg3,
                                                        ARGTYPE4 arg4,ARGTYPE5 arg5,ARGTYPE6 arg6,
                                                        ARGTYPE7 arg7,ARGTYPE8 arg8,ARGTYPE9 arg9)
{
  return (new FuncClass9<FUNCTYPE,ARGTYPE1,ARGTYPE2,ARGTYPE3,ARGTYPE4,ARGTYPE5,ARGTYPE6,ARGTYPE7,ARGTYPE8,ARGTYPE9>
    (func_ptr,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9));
}

class AutoFuncClass
{
public:
  AutoFuncClass(BaseFuncClass *obj):m_funcobj(obj){}
  ~AutoFuncClass(){if (m_funcobj) delete m_funcobj;}
private:
  BaseFuncClass *m_funcobj;
};

//...3,4,5,6,7,8,9

//使用方式
/*
int myfunc();
int myfunc1(string arg)；

void myfunc()
{
AutoClass c1(AutoFunc(myfunc));
AutoClass c2(AutoFunc(myfunc1,string("hello"))); 

在作用域结束的时候会调用上面两个AutoFunc返回的对象的析构，从而调用了myfunc和myfunc1;
}

*/


#endif