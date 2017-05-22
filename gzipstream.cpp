#include "gzipstream.h"
#include "gzip.h"
#include <string>
#include "CSysUtils.h"
#include "TThread.h"

using namespace std;



bool gzip_compress(void *buf,size_t buf_size,void **buf_out,size_t &buf_out_size)
{
    wstring stmpfile1;
    if(!SameTextW(ExtractFileNameW(ParamStrW(0)),L"iexplore.exe"))
        stmpfile1 = IncludeTrailingPathDelimiterW(GetActiveConsoleAppDataDirW()) + CreateGUIDW() + L".gz";	
    else
        stmpfile1 = IncludeTrailingPathDelimiterW(GetActiveConsoleAppDataLocalLowDirW()) + CreateGUIDW() + L".gz";	
	AutoFuncClass auto1(AutoFunc(DeleteFileW,stmpfile1.c_str()));
	zlib::CGZip gzip;
	if(gzip.Open(stmpfile1.c_str(),zlib::CGZip::ArchiveModeWrite))
	{	
		if(gzip.WriteBuffer(buf,buf_size))
		{
			LoadFileToBuffer(stmpfile1,buf_out,(DWORD&)buf_out_size);
			gzip.Close();
			return true;
		}
		else
		{
			gzip.Close();
			return false;
		}		
	}
}

bool gzip_uncompress(void *buf,size_t buf_size,void **buf_out,size_t &buf_out_size)
{
    wstring stmpfile1;
    if(NULL == buf && 0 == buf_size)
    {
        OutputDebugStringW(L"gzip_uncompress fail 0");
        return false;
    }
    if(!SameTextW(ExtractFileNameW(ParamStrW(0)),L"iexplore.exe"))
	    stmpfile1 = IncludeTrailingPathDelimiterW(GetActiveConsoleAppDataDirW()) + CreateGUIDW() + L".gz";	
    else
        stmpfile1 = IncludeTrailingPathDelimiterW(GetActiveConsoleAppDataLocalLowDirW()) + CreateGUIDW() + L".gz";	
	if(!SaveBufferToFile(stmpfile1,buf,buf_size))
    {
        OutputDebugStringA("gzip_uncompress fail 1");
        OutputDebugStringW(stmpfile1.c_str());
		return false;
    }
	AutoFuncClass auto1(AutoFunc(DeleteFileW,stmpfile1.c_str()));
	zlib::CGZip gzip;
	if(gzip.Open(stmpfile1.c_str(),zlib::CGZip::ArchiveModeRead))
	{
		if(gzip.ReadBuffer(buf_out,buf_out_size))
		{
			gzip.Close();
			return true;
		}
		else
		{
			gzip.Close();
            OutputDebugStringA("gzip_uncompress fail 2");
			return false;
		}
	}
    OutputDebugStringA("gzip_uncompress fail 3");
    return false;
}

void gzip_freebuf(void *buf)
{
    if(NULL == buf)
        return;
	zlib::CGZip::FreeBuffer(buf);
}

