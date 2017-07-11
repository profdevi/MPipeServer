/*    
    MPipeServer.exe : Starts up a named pipe server that a client can write to.
    Copyright (C) 2017  Comine.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


//v2.17 copyright Comine.com 20170709U0636
/*
Bug Notice:
	MStdSPrintf(const wchar_t *)  seems to be failing.
*/

/////////////////////////////////////////////////
#include "MStdLib.h"


//*******************************************
//* Display Standard Header Files and definitions
//*******************************************
#if (defined(MSTDLIB_API_WINDOWS) )
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <time.h>
	#include <math.h>
	#include <windows.h>
	#include <conio.h>
	#include <float.h>
	#include <intrin.h>

	// Link in Libraries if on windows
	#if ( defined(MSTDLIB_OS_WINDOWS) )
	#pragma comment(lib,"user32.lib")
	#pragma comment(lib,"kernel32.lib")
	#pragma comment(lib,"advapi32.lib")
	#pragma comment(lib,"rpcrt4.lib")
	#endif

///////////////////////////////////////////
#elif ( defined(MSTDLIB_API_LINUX) )
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <wchar.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <linux/limits.h>
#endif

//***************************************************
//** Module Elements
//***************************************************
static char GTempBuffer[100]="";
static wchar_t GTempWideBuffer[100]=L"";


//***************************************************
//** Functions
//***************************************************
bool MStdPrintInfo(void)
	{
	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWS)
	MStdPrintf(" MSTDLIB_OS_WINDOWS defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)
	MStdPrintf(" MSTDLIB_OS_WINDOWSRT defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSOLD)
	MStdPrintf(" MSTDLIB_OS_WINDOWSOLD defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_MINGW)
	MStdPrintf(" MSTDLIB_OS_MINGW defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_LINUX)
	MStdPrintf(" MSTDLIB_OS_LINUX defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_IPHONE)
	MStdPrintf(" MSTDLIB_OS_IPHONE defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_MACOS)
	MStdPrintf(" MSTDLIB_OS_MACOS defined\n");
	#endif

	//////////////////////////////////////////////////
	#if defined(MSTDLIB_OS_OTHER)
	MStdPrintf(" MSTDLIB_OS_OTHER defined\n");
	#endif

	MStdPrintf("\n");

	// Allocate a buffer for getting information
	char buffer[100];

	// Print Machine Name:
	MStdGetMachineName(buffer,sizeof(buffer)-2);
	MStdPrintf(" Machine Name    :  %s\n",buffer);

	// Print OS Root
	MStdGetOSRoot(buffer,sizeof(buffer)-2);
	MStdPrintf(" OS Root         :  %s\n",buffer);

	// Print Path Seperator
	MStdGetOSPathSeperator(buffer,sizeof(buffer)-2);
	MStdPrintf(" Path Seperator  :  %s\n",buffer);

	// Print User Name
	MStdGetUserName(buffer,sizeof(buffer)-2);
	MStdPrintf(" User Name       :  %s\n",buffer);

	// Print User Home Directory
	MStdGetUserHome(buffer,sizeof(buffer)-2);
	MStdPrintf(" User Home       :  %s\n",buffer);

	// Print out current time
	time_t timeofday=MStdGetTime();
	MStdCTime(buffer,sizeof(buffer)-2,&timeofday);
	MStdPrintf(" Time of Day     :  %s\n",buffer);
	
	return true;
	}


/////////////////////////////////////////////////////
void MStdAssertInternal(bool flag,const char *exp,const char *filename,int lineno)
	{
	if(flag==true) { return; }
	char line[1000];
	MStdSPrintf(line,sizeof(line),"Assert Failed: %s:%d  %s",filename,lineno,exp);
	MStdPrintf("ASSERT Failed: %s:%s(%d)\n",exp,filename,lineno);
	return;
	}


///////////////////////////////////////////////////////////
void MStdError(const char *info,const char *filename,int lineno)
	{
	char line[1000];
	MStdSPrintf(line,sizeof(line),"Error: %s %s(%d)",info,filename,lineno);
	MStdPrintf("Error: %s %s(%d)\n",info,filename,lineno);
	return;	
	}


//////////////////////////////////////////////////////
void MStdBreak(void)
	{
	// Check if Visual Stdio Debugger
    #if defined(_MSC_VER)
		// __asm int 3	(Works on the older vc<14.0 (vs 2015) )
		__debugbreak();
	#elif defined(__GNUC__)
		asm(".byte 0xcd,0x03\n\t");
	#else
		raise(5);
	#endif
	}


//////////////////////////////////////////////////////
MStdFileHandle MStdFileOpen(const char *filename,const char *mode)
	{
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSRT) )
	FILE *filehandle=NULL;
	if(fopen_s(&filehandle,filename,mode)!=0)
		{
		return NULL;
		}

	return (MStdFileHandle)filehandle;

	#else	// MSTDLIB_OS_WINDOWS

	return (MStdFileHandle)fopen(filename,mode);

	#endif // MSTDLIB_OS_WINDOWS
	}


//////////////////////////////////////////////////////
bool MStdSetStdInMode(bool binary)
	{
	const char *mode;
	if(binary==true)
		{  mode="rb";  }
	else
		{  mode="rt";  }

	//////////
	#if defined(MSTDLIB_OS_WINDOWS)
	//Reopen file handled for stdin/stdout to work
	FILE *tmp=NULL;
	if(freopen_s(&tmp,"CONIN$",mode,stdin)!=0)
		{
		return false;
		}

	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	if(freopen("CONIN$",mode,stdin)==NULL)
		{
		return false;
		}
				
	#elif (defined(MSTDLIB_OS_LINUX)  || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(freopen("/dev/tty",mode,stdin)==NULL)
		{
		return false;
		}
	#endif // MSTDLIB_OS_WINDOWS

	return true;
	}


//////////////////////////////////////////////////////
bool MStdSetStdOutMode(bool binary)
	{
	const char *mode;
	if(binary==true)
		{  mode="wb";  }
	else
		{  mode="wt";  }

	////////////////////////////////////
	#if defined(MSTDLIN_OS_WINDOWS)
	FILE *tmp=NULL;
	if(freopen_s(&tmp,"CONOUT$",mode,stdout)!=0)
		{
		return false;
		}

	return true;

	////////////////////////////////////
	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	if(freopen("CONOUT$",mode,stdout)==NULL)
		{
		return false;
		}

	return true;

	////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(freopen("/dev/tty",mode,stdout)==NULL)
		{
		return false;
		}				

	return true;

	#else
	return false;
	#endif // MSTDLIB_OS_WINDOWS				
	}


//////////////////////////////////////////////////////
bool MStdFileClose(MStdFileHandle handle)
	{
	if(fclose((FILE *)handle)!=0)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////
MStdFileHandle MStdGetStdIn(void)
	{  return (MStdFileHandle)stdin;  }


//////////////////////////////////////////////////////
MStdFileHandle MStdGetStdOut(void)
	{  return (MStdFileHandle)stdout;  }


////////////////////////////////////////////////////
bool MStdFileWrite(MStdFileHandle handle,const char *str)
	{
	int length=MStdStrLen(str);

	if(length!=(int)fwrite(str,sizeof(char),length,(FILE *)handle))
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////
int MStdFileWrite(MStdFileHandle handle,const void *buffer
		,int elementsize,int elementcount)
	{
	int newcount=(int)fwrite(buffer,(size_t)elementsize
			,(size_t)elementcount,(FILE *)handle);
	if(newcount!=elementcount)
		{
		return newcount;
		}

	return newcount;
	}


///////////////////////////////////////////////////////////////
int MStdFileRead(MStdFileHandle handle,void *buffer
		,int elementsize,int elementcount)
	{
	if(elementcount==0) { return 0; }
	int newcount=(int)fread(buffer,elementsize,elementcount,(FILE *)handle);
	return newcount;
	}


/////////////////////////////////////////////////////////////////
bool MStdFileGetString(MStdFileHandle handle,char *buffer,int buffersize)
	{
	if(fgets(buffer,buffersize,(FILE *)handle)==NULL)
		{  return false; }

	return true;
	}


///////////////////////////////////////////////////////
bool MStdFileFlush(MStdFileHandle handle)
	{
	if(fflush((FILE *)handle)!=0)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////////
bool MStdFileSeek(MStdFileHandle handle,int offset,int type)
	{
	if(fseek((FILE*)handle,offset,type)!=0)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////
long MStdFileTell(MStdFileHandle handle)
	{
	return ftell((FILE *)handle);
	}


//////////////////////////////////////////////////////
bool MStdFileEOF(MStdFileHandle handle)
	{
	if(feof((FILE *)handle)==0)
		{
		return false;
		}

	return true;
	}



//////////////////////////////////////////////////////
int MStdPrintf(const char *format,...)
	{
	va_list args;
	va_start(args,format);
	int length=vprintf(format,args);
	va_end(args);

	return length;
	}


//////////////////////////////////////////////////////
int MStdErrPrintf(const char *format,...)
	{
	va_list args;
	va_start(args,format);

	int length=vfprintf(stderr,format,args);
	va_end(args);

	return length;
	}


//////////////////////////////////////////////////////
bool MStdGetString(char *buffer,int size)
	{
	fgets(buffer,size,stdin);
	buffer[size-1]=0;
	return true;
	}


//////////////////////////////////////////////////////
bool MStdOpenConsoleIO(void)
	{
	#if defined(MSTDLIB_OS_WINDOWS) 
	if(AllocConsole()==FALSE) { return true; }

	FILE *tmp=NULL;
	if(freopen_s(&tmp,"CONIN$","r",stdin)!=0)
		{
		return false;
		}
				
	if(freopen_s(&tmp,"CONOUT$","w",stdout)!=0)
		{
		return false;
		}

	return true;

	/////////////////////////////////////////
	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	if(freopen("CONIN$","r",stdin)==NULL)
		{
		return false;
		}
				
	if(freopen("CONOUT$","w",stdout)==NULL)
		{
		return false;
		}

	return true;
	#elif( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	if(freopen("/dev/tty","r",stdin)==NULL)
		{
		return false;
		}
				
	if(freopen("/dev/tty","w",stdout)==NULL)
		{
		return false;
		}

	if(freopen("/dev/tty","w",stderr)==NULL)
		{
		return false;
		}

	return true;
	
	////////////////////////////////////////////////////
	#elif( defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(stdin==NULL && freopen("/dev/tty","r",stdin)==NULL)
		{
		return false;
		}
				
	if(stdout==NULL && freopen("/dev/tty","w",stdout)==NULL)
		{
		return false;
		}

	if(stderr==NULL && freopen("/dev/tty","w",stderr)==NULL)
		{
		return false;
		}

	return true;
	
	#else
	return false;
	#endif // MSTDLIB_OS_WINDOWS
	}


/////////////////////////////////////////////////////////
bool MStdSetOutput(const char *filename)
	{
	#if ( defined(MSTDLIB_OS_WINDOWS) ||  defined(MSTDLIB_OS_WINDOWSRT) )
	FILE *newfileout=NULL;
	if(freopen_s(&newfileout,filename,"a",stdout)!=0)
		{
		return false;
		}

	return true;

	////////////////////////////////////////
	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_IS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	FILE *newfileout=freopen(filename,"a",stdout);
	if(newfileout==NULL)
		{
		return false;
		}
	
	return true;

	#endif // MSTDLIB_OS_WINDOWS
	}


/////////////////////////////////////////////////////////
bool MStdSetInput(const char *filename)
	{
	#if defined(MSTDLIB_OS_WINDOWS)
	FILE *newfileout=NULL;
	if(freopen_s(&newfileout,filename,"r",stdin)!=0)
		{
		return false;
		}

	return true;

	////////////////////////////////
	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	FILE *newfilein=freopen(filename,"r",stdin);
	if(newfilein==NULL)
		{
		return false;
		}
	
	return true;

	#else
	return false;

	#endif // MSTDLIB_OS_WINDOWS
	}


////////////////////////////////////////////////////////
bool MStdSetError(const char *filename)
	{
	//////////////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWS)
	FILE *newfileout=NULL;
	if(freopen_s(&newfileout,filename,"a",stderr)!=0)
		{
		return false;
		}

	return true;

	//////////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) ||defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )

	FILE *newfilein=freopen(filename,"a",stderr);
	if(newfilein==NULL)
		{
		return false;
		}
	
	return true;

	//////////////////////////////////////////////
	#else
	return false;
	#endif // _MSC_VER
	}


//////////////////////////////////////////////////////
int MStdSPrintf(char *target,int targetsize,const char *format,...)
	{

	////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSRT) )
	va_list args;
	va_start(args,format);

	int length=vsnprintf_s(target,targetsize,targetsize,format,args);

	va_end(args);

	target[targetsize-1]=0;
	return length;

	////////////////////////////////
	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	va_list args;
	va_start(args,format);

	int length=_vsnprintf(target,targetsize,format,args);
	
	va_end(args);

	target[targetsize-1]=0;
	return length;

	////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	va_list args;
	va_start(args,format);

	int length=vsnprintf(target,targetsize,format,args);
	
	va_end(args);

	target[targetsize-1]=0;
	return length;

	///////////////////////////////////
	#else
	return 0;

	#endif // _MSC_VER
	}


/////////////////////////////////////////////////////
const char *MStdStr(bool value)
	{
	if(value==true)
		{  MStdStrCpy(GTempBuffer,"true");  }
	else
		{  MStdStrCpy(GTempBuffer,"false");  }

	return GTempBuffer;
	}


/////////////////////////////////////////////////////
const char *MStdStr(int value)
	{
	MStdSPrintf(GTempBuffer,sizeof(GTempBuffer),"%d",value);
	return GTempBuffer;
	}


/////////////////////////////////////////////////////
const char *MStdStr(float value)
	{
	MStdSPrintf(GTempBuffer,sizeof(GTempBuffer),"%f",value);
	return GTempBuffer;
	}


/////////////////////////////////////////////////////
const char *MStdStr(double value)
	{
	MStdSPrintf(GTempBuffer,sizeof(GTempBuffer),"%g",value);
	return GTempBuffer;
	}


//////////////////////////////////////////////////////
bool MStdStr(bool value,char strout[],int stroutlen)
	{
	MStdAssert(strout!=0 && stroutlen>=6);

	if(value==true)
		{  MStdStrCpy(strout,"true");  }
	else
		{  MStdStrCpy(strout,"false");  }

	return true;	
	}


//////////////////////////////////////////////////////
bool MStdStr(int value,char strout[],int stroutlen)
	{
	const int length=MStdSPrintf(strout,stroutlen,"%d",value);
	if(length>stroutlen+1 || length<=0) { return false; }
		
	return true;
	}


///////////////////////////////////////////////////////
bool MStdStr(float value,char strout[],int stroutlen)
	{
	const int length=MStdSPrintf(strout,stroutlen,"%f",value);
	if(length>stroutlen+1 || length<=0) { return false; }
		
	return true;
	}


///////////////////////////////////////////////////////
bool MStdStr(double value,char strout[],int stroutlen)
	{
	const int length=MStdSPrintf(strout,stroutlen,"%g",value);
	if(length>stroutlen+1 || length<=0) { return false; }
		
	return true;
	}


/////////////////////////////////////////////
int MStdSPrintf(wchar_t *target,int targetsize,const wchar_t *format,...)
	{

	///////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSRT) )
	va_list args;
	va_start(args,format);

	int length=0;

	length=_vsnwprintf_s(target,targetsize,_TRUNCATE,format,args);

	va_end(args);
	return length;	

	///////////////////////////////////
	#elif defined(MSTDLIB_OS_WINDOWSOLD)
	va_list args;
	va_start(args,format);

	int length=0;

	length=_vsnwprintf(target,targetsize,format,args);

	va_end(args);
	return length;	
	
	////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_MINGW) )
	va_list args;
	va_start(args,format);

	int length=0;

	// MINGW Does not have a size argument
	length=vswprintf(target,format,args);

	va_end(args);
	return length;	
	
	
	///////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	va_list args;
	va_start(args,format);

	int length=0;

	length=vswprintf(target,targetsize,format,args);

	va_end(args);
	return length;	
	
	#endif // MSTDLIB_OS_WINDOWS

	return 0;
	}


//////////////////////////////////////////////////////////
const wchar_t *MStdStrWide(bool value)
	{
	if(value==true)
		{  MStdStrCpy(GTempBuffer,"true");  }
	else
		{  MStdStrCpy(GTempBuffer,"false");  }

	return (const wchar_t *)GTempBuffer;
	}


////////////////////////////////////////////////////////
const wchar_t *MStdStrWide(int value)
	{
	MStdSPrintf(GTempWideBuffer,sizeof(GTempWideBuffer)/sizeof(wchar_t),L"%d",value);
	return GTempWideBuffer;
	}


/////////////////////////////////////////////////////////
const wchar_t *MStdStrWide(float value)
	{
	MStdSPrintf(GTempWideBuffer,sizeof(GTempWideBuffer)/sizeof(wchar_t),L"%f",value);
	return GTempWideBuffer;
	}


////////////////////////////////////////////////////////////
const wchar_t *MStdStrWide(double value)
	{
	MStdSPrintf(GTempWideBuffer,sizeof(GTempWideBuffer)/sizeof(wchar_t),L"%lf",value);
	return GTempWideBuffer;
	}


//////////////////////////////////////////////////////
bool MStdStrWide(bool value,wchar_t strout[],int stroutlen)
	{
	MStdAssert(strout!=0 && stroutlen>=6);

	if(value==true)
		{  MStdStrCpy(strout,L"true");  }
	else
		{  MStdStrCpy(strout,L"false");  }

	return true;	
	}


//////////////////////////////////////////////////////
bool MStdStrWide(int value,wchar_t strout[],int stroutlen)
	{
	const int length=MStdSPrintf(strout,stroutlen,L"%d",value);
	if(length>stroutlen+1 || length<=0) { return false; }
		
	return true;
	}


///////////////////////////////////////////////////////
bool MStdStrWide(float value,wchar_t strout[],int stroutlen)
	{
	const int length=MStdSPrintf(strout,stroutlen,L"%f",value);
	if(length>stroutlen+1 || length<=0) { return false; }
		
	return true;
	}


///////////////////////////////////////////////////////
bool MStdStrWide(double value,wchar_t strout[],int stroutlen)
	{
	const int length=MStdSPrintf(strout,stroutlen,L"%g",value);
	if(length>stroutlen+1 || length<=0) { return false; }
		
	return true;
	}


////////////////////////////////////////////////////////////
bool MStdStrIsMember(char ch,const char *str)
	{
	MStdAssert(str!=0);
	for(const char *p=str;*p!=0;++p)
		{
		if(*p==ch) { return true; }
		}

	return false;
	}


///////////////////////////////////////////////////////////
bool MStdStrIsMember(wchar_t ch,const wchar_t *str)
	{
	MStdAssert(str!=0);
	for(const wchar_t *p=str;*p!=0;++p)
		{
		if(*p==ch) { return true; }
		}

	return false;
	}


////////////////////////////////////////////////////////////
bool MStdStrRemoveChars(char *modifystr,const char *removechars)
	{
	char *src,*dst;
	for(dst=src=modifystr; ;++src)
		{
		*dst=*src;
		if(*dst==0) { break; }
		if(MStdStrIsMember(*dst,removechars)==false)
			{
			dst=dst+1;
			}
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MStdStrRemoveChars(wchar_t *modifystr,const wchar_t *removechars)
	{
	wchar_t *src,*dst;
	for(dst=src=modifystr; ;++src)
		{
		*dst=*src;
		if(*dst==0) { break; }
		if(MStdStrIsMember(*dst,removechars)==false)
			{
			dst=dst+1;
			}
		}

	return true;
	}


/////////////////////////////////////////////////////////
bool MStdWindowOutput(const char *title,const char *info)
	{
	///////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)

	MStdArray<wchar_t> wtitle;
	MStdStrCpy(wtitle,title);
	Platform::String^ ptitle=ref new Platform::String(wtitle.Get() );
	
	MStdArray<wchar_t> winfo;
	MStdStrCpy(winfo,info);
	Platform::String^ pinfo=ref new Platform::String(winfo.Get() );

	Windows::UI::Popups::MessageDialog^ dlg=ref new Windows::UI::Popups::MessageDialog(pinfo,ptitle);
	dlg->ShowAsync();
	dlg=nullptr;

	return true;
	
	#elif (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) )
	MessageBoxA(NULL,info,title,MB_OK);

	///////////////////////////////////////
	#elif defined(MSTDLIB_OS_MINGW)
	MessageBox(NULL,info,title,MB_OK);

	///////////////////////////////////////
	#else
	printf("%s",info);
	#endif // MSTDLIB_OS_WINDOWS

	return true;
	}


////////////////////////////////////////////////////////
bool MStdSleep(int ms)
	{
	if(ms<=0) {  return false;  }
	#if defined(MSTDLIB_OS_WINDOWSRT)

	return false;

	#elif ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined (MSTDLIB_OS_MINGW) )
	Sleep(ms);
	return true;

	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(ms*1000<0)
		{
		// Do a double call if ms wraps int storage
		sleep(ms/1000);
		usleep((ms%1000)*1000);
		return true;
		}

	usleep(ms*1000);
	return true;

	#else
	return false;
	#endif // MSTDLIB_OS_WINDOWS
	}


////////////////////////////////////////////////////////
bool MStdExit(int value)
	{
	/////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_WINDOWSRT) )
	exit(value);
	return true;

	////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	exit(value);
	return true;

	////////////////////////
	#else
	return false;

	#endif // MSTDLIB_OS_WINDOWS
	}


////////////////////////////////////////////////////////
char MStdToBinary(int value)
	{
	if(value==0)
		{  return '0';  }

	if(value==1)
		{  return '1';  }

	return 0;
	}


////////////////////////////////////////////////////////
char MStdToOctal(int value)
	{
	MStdAssert(value>=0 && value<8);
	return '0'+value;
	}


////////////////////////////////////////////////////////
char MStdToHex(int value)
	{
	MStdAssert(value>=0 && value<16);
	if(value<10)
		{  return '0'+value;  }

	return 'A'+(value-10);
	}


///////////////////////////////////////////////////////
bool MStdIsNan(double val)
	{
	////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)

	if(isnan(val)==0)
		{  return false; }
	else
		{  return true;  }	
	
	#elif ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	if(_isnan(val)==0)
		{  return false; }
	else
		{  return true;  }

	///////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(isnan(val)==0)
		{  return false;  }
	else
		{  return true;  }

	#endif 

	return false;
	}


//////////////////////////////////////////////////////
bool MStdIsFinite(double val)
	{
	/////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSRT) )

		#if (_MSC_VER>=1800)
			if(isfinite(val)) { return true; }
			return false;
		#else
		if(_finite(val)!=0) { return true; }
		return false;

		#endif	
	
	/////////////////////////////////////
	#else
	if(isfinite(val)) { return true; }
	return false;
	#endif
	}


///////////////////////////////////////////////////////
bool MStdGetEnvVar(const char *var,char *buf,int buflen)
	{
	MStdAssert(buf!=0 && buflen>0);

	///////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWS)
	if(GetEnvironmentVariableA(var,buf,(DWORD)buflen)==0)
		{
		*buf=0;
		return false;
		}

	return true;

	///////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_MACOS) )
	const char *envvalue=getenv(var);
	if(envvalue==0)
		{
		*buf=0;
		return false;
		}

	const int envvaluelen=MStdStrLen(envvalue);
	if(envvaluelen+1>buflen)
		{
		*buf=0;
		return false;
		}

	MStdStrCpy(buf,envvalue);

	return true;

	///////////////////////////////
	#else
	return false;
	#endif
	}


///////////////////////////////////////////////////////
bool MStdGetUserHome(char *buf,int buflen)
	{
	MStdAssert(buf!=0 && buflen>0);
	///////////////////////////////
	#if (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_MINGW) )
	if(MStdGetEnvVar("USERPROFILE",buf,buflen)==false)
		{
		*buf=0;
		return false;
		}

	// Convert forward slashes to backword slashes
	for(int i=0;buf[i]!=0;++i)
		{
		if(buf[i]=='\\') {  buf[i]='/'; }
		}
	return true;

	///////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) ||  defined(MSTDLIB_OS_MACOS) )
	if(MStdGetEnvVar("HOME",buf,buflen)==false)
		{
		*buf=0;
		return false;
		}

	return true;
	

	///////////////////////////////
	#else
	return false;
	#endif
	}

///////////////////////////////////////////////////////
bool MStdMemCpy(void *target,const void *src,int count)
	{
	int newcount=count/sizeof(int);
	int i;

	int *idst=(int *)target;
	int *isrc=(int *)src;

	// Copy Memory Block
	for(i=0;i<newcount;++i)
		{  *idst++=*isrc++;  }

	int copied=newcount*sizeof(int);
	int remaining=count-copied;
	if(remaining==0){  return true;	 }

	char *cdst=((char *)target)+copied;
	char *csrc=((char *)src)+copied;

	// copy the remainint chars
	for(i=0;i<remaining;++i)
		{
		*cdst++ = *csrc++;
		}

	return true;
	}


////////////////////////////////////////////////////////
bool MStdMemSet(void *dst,int value,int size)
	{
	char *cdst=(char *)dst;
	int i;
	for(i=0;i<size;++i)
		{  *cdst++=value;  }

	return true;
	}


////////////////////////////////////////////////////////
bool MStdMemZero(void *dst,int size)
	{
	int intcount=size/sizeof(int);
	int *idst=(int *)dst;
	int i;
	for(i=0;i<intcount;++i)
		{  *idst++=0;  }

	// Remaining bytes 
	int remaining=size-intcount*sizeof(int);
	if(remaining==0) { return true; }

	//=Still a couple of bytes left

	char *cdst=(char *)idst;
	for(i=0;i<remaining;++i)
		{  *cdst=0;  }

	return true;
	}


//////////////////////////////////////////////////////
int MStdMemCmp(const void *src,const void *dst,int size)
	{
	int i;
	const char *p=(const char *)src;
	const char *q=(const char *)dst;
	for(i=0;i<size;++i)
		{
		if(*p!=*q)
			{ return *p-*q; }

		p=p+1;
		q=q+1;
		}

	return 0;
	}


///////////////////////////////////////////////////////
bool MStdMemMove(void *dst,void *src,int count)
	{
	memmove(dst,src,count);
	return true;
	}


////////////////////////////////////////////////////////
void *MStdMalloc(int size)
	{
	return malloc(size);
	}


////////////////////////////////////////////////////////
bool MStdFree(void *memblock)
	{
	free(memblock);
	return true;
	}


///////////////////////////////////////////////////////
void *MStdReAlloc(void *memblock,int newsize)
	{
	return realloc(memblock,newsize);
	}


////////////////////////////////////////////////////
bool MStdKbHit(void)
	{
	////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)
		return false;

	#elif (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	if(_kbhit())
		{ return true; }

	return false;

	///////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	getchar();
	return true;

	//////////////////////////////
	#else
	return false;
	#endif // MSTDLIB_OS_WINDOWS
	}


///////////////////////////////////////////////////
int MStdGetCh(bool forcestdio)
	{
	////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)

	if(forcestdio==true)
		{  return getchar(); }

	return _getchar_nolock();
	
	#elif (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) )
	if(forcestdio==true)
		{  return getchar(); }

	return _getch();

	////////////////////////////
	#elif defined(MSTDLIB_OS_MINGW)
	if(forcestdio==true)
		{  return getchar();  }
	
	return _getch();
	
	///////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(forcestdio==true)
		{  return getchar();  }

	char ch;
	if(read(0,&ch,1)<=0)
		{  return 0; }

	return ch;

	#endif // MSDLIB_OS_WINDOWS
	}


////////////////////////////////////////////////
time_t MStdGetTime(void)
	{
	time_t timecount;
	time(&timecount);
	return timecount;
	}


////////////////////////////////////////////////
bool MStdCTime(char *buf,int bufsize,time_t *tmdata)
	{
	// Reset size of buffer	
	buf[0]=0;

	///////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSRT) )
	if(ctime_s(buf,bufsize,tmdata))
		{
		return false;
		}
	
	// Return current time
	return true;

	///////////////////////////////////////
	#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	const char *str=ctime(tmdata);
	if(str==NULL)
		{
		return false;
		}

	if(MStdStrLen(str)>=bufsize)
		{
		return false;
		}

	MStdStrCpy(buf,bufsize,str);
	return true;

	#endif // MSTDLIB_OS_WINDOWS
	}


/////////////////////////////////////////////////
bool MStdAtExit(void fun(void))
	{
	if(atexit(fun))
		{
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MStdFileRemove(const char *filename)
	{
	if(remove(filename)!=0)
		{
		// Do not generate an error
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MStdFileRename(const char *filesrc,const char *filetarget)
	{
	/////////////////////////////
	if(rename(filesrc,filetarget)!=0)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////////
bool MStdFileCopy(const char *srcfile,const char *dstfile,bool stopifexists,bool erroronfail)
	{
	//////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	BOOL checkexists;
	if(stopifexists==true) { checkexists=TRUE;  } else { checkexists=FALSE; }

	if(CopyFileA(srcfile,dstfile,checkexists)==FALSE)
		{
		if(stopifexists==true)
			{
			return false;
			}

		if(erroronfail==false)
			{
			return false;
			}

		return false;
		}

	return true;

	/////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(stopifexists==true && MStdFileExists(dstfile)==true)
		{
		return false;
		}

	MStdArray<char> buffer;
	if(buffer.Create(2048)==false)
		{
		return false;
		}

	int fdsrc=open(srcfile,O_RDONLY);
	if(fdsrc<0)
		{
		return false;
		}

	int fddst=open(dstfile,O_WRONLY|O_CREAT|O_TRUNC,0666);
	if(fddst<0)
		{
		close(fdsrc);
		return false;
		}

	int readamount;
	int writeamount;
	for(;;)
		{
		readamount=read(fdsrc,buffer.Get(),buffer.GetLength());
		if(readamount<=0)
			{
			break;
			}

		writeamount=write(fddst,buffer.Get(),readamount);
		if(writeamount<=0)
			{
			break;
			}
		}
	

	close(fdsrc);
	close(fddst);
	buffer.Destroy();
	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;	
	}


//////////////////////////////////////////////////
bool MStdFileExists(const char *filename)
	{
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	DWORD fileinfo=GetFileAttributesA(filename);
	if(fileinfo==INVALID_FILE_ATTRIBUTES) { return false; }
	if((fileinfo & FILE_ATTRIBUTE_DIRECTORY)>0) { return false; }
	if((fileinfo & FILE_ATTRIBUTE_DEVICE)>0) { return false; }

	return true;

	///////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	if(access(filename,F_OK)==0)
		{
		return true;
		}

	return false;
	#endif // MSTDLIB_OS_WINDOWS

	return false;	
	}


//////////////////////////////////////////////////
bool MStdFileIsBinary(const char *filename)
	{
	MStdAssert(filename!=0 && *filename!=0);
	MStdFileHandle file=MStdFileOpen(filename,"rb");
	if(file==NULL)
		{
		return false;
		}

	for( ; ; )
		{
		unsigned char ch;
		if(MStdFileRead(file,&ch,1,1)!=1) { break; }
		if(ch==0 || ch>127)
			{
			// Non Ascii
			MStdFileClose(file);
			return true;
			}
		}

	MStdFileClose(file);
	return false;
	}


//////////////////////////////////////////////////
bool MStdFileReadText(const char *filename,char *buffer,int &size)
	{
	if(filename==NULL || *filename==0 || buffer==NULL || size<=1)
		{
		size=0; return false;
		}

	// Check if file is binary
	if(MStdFileExists(filename)==false || MStdFileIsBinary(filename)==true)
		{
		size=0; return false;
		}

	// Read from file
	MStdFileHandle file=MStdFileOpen(filename,"rb");
	if(file==NULL)
		{
		return true;
		}

	int len=MStdFileRead(file,buffer,1,size-1);
	if(len<0)
		{
		size=0;  MStdFileClose(file);  return false;
		}

	MStdFileClose(file);
	buffer[len]=0;
	size=len;
	return true;	
	}


/////////////////////////////////////////////////////
bool MStdFileWriteText(const char *filename,const char *data)
	{
	MStdAssert(filename!=0 && *filename!=0 && data!=0);
	if(filename==NULL || *filename==0 || data==NULL )
		{
		return false;
		}

	// Open the file for writing
	MStdFileHandle file=MStdFileOpen(filename,"wb");
	if(file==NULL)
		{
		return false;
		}

	const int len=MStdStrLen(data);
	if(len==0)
		{
		MStdFileClose(file);
		return true;		
		}

	if(MStdFileWrite(file,data,1,len)!=len)
		{
		MStdFileClose(file);
		return false;
		}

	MStdFileClose(file);
	return true;
	}


//////////////////////////////////////////////////
bool MStdExec(const char *cmd)
	{
	#if defined(MSTDLIB_OS_WINDOWSRT)
		
		// Do Nothing for WINRT
		return false;

	#else

		int ret=system(cmd);
		if(ret==0)
			{  return true; }

		return false;

	#endif 
	}


//////////////////////////////////////////////////
int MStdAToI(const char *str)
	{
	return atoi(str);
	}


//////////////////////////////////////////////////
int MStdAToI(const wchar_t *str)
	{
	char buf[100];
	MStdSPrintf(buf,sizeof(buf),"%S",str);
	return atoi(buf);
	}


/////////////////////////////////////////////////
bool MStdIToA(int inval,char *buffer,int length)
	{
	
	int maxlength=0;

	// Check for -(number) or 0.
	if(inval<=0) { maxlength = maxlength+1; }
	
	if(inval!=0)
		{
		int tmpval;
		for(tmpval=inval;tmpval!=0;tmpval=tmpval/10)
			{
			maxlength = maxlength + 1;
			}
		}

	if(maxlength+1>length)
		{
		buffer[0]=0;
		return false;
		}

	//=We now have the correct write size

	// Check for zero
	if(inval==0)
		{
		buffer[0]='0';
		buffer[1]=0;
		return true;
		}


	if(inval<0)
		{
		buffer[0]='-';
		inval = -inval;
		}
	
	// Now write normally
	int writepos = maxlength-1;
	while(inval>0)
		{
		buffer[writepos] = '0' + (inval % 10 );
		writepos = writepos - 1;
		inval = inval/10;
		}

	buffer[maxlength]=0;
	return true;
	}


/////////////////////////////////////////////////
bool MStdIToA(int inval,wchar_t *buffer,int length)
	{
	
	int maxlength=0;

	// Check for -(number) or 0.
	if(inval<=0) { maxlength = maxlength+1; }
	
	if(inval!=0)
		{
		int tmpval;
		for(tmpval=inval;tmpval!=0;tmpval=tmpval/10)
			{
			maxlength = maxlength + 1;
			}
		}

	if(maxlength+1>length)
		{
		buffer[0]=0;
		return false;
		}

	//=We now have the correct write size

	// Check for zero
	if(inval==0)
		{
		buffer[0]='0';
		buffer[1]=0;
		return true;
		}


	if(inval<0)
		{
		buffer[0]='-';
		inval = -inval;
		}
	
	// Now write normally
	int writepos = maxlength-1;
	while(inval>0)
		{
		buffer[writepos] = '0' + (inval % 10 );
		writepos = writepos - 1;
		inval = inval/10;
		}

	buffer[maxlength]=0;
	return true;
	}




///////////////////////////////////////////////////
unsigned int MStdHToI(const char *str)
	{
	unsigned int retval=0;

	int i;
	for(i=0;str[i]!=0;++i)
		{
		const char ch=str[i];
		int converteddigit;
		if(ch>='0' && ch<='9')
			{
			converteddigit = (ch-'0');
			}
		else if(ch>='A' && ch<='F')
			{
			converteddigit = (ch - 'A') + 10;
			}
		else if(ch>='a' && ch<='f')
			{
			converteddigit = (ch - 'a') + 10;
			}
		else
			{
			return retval;
			}

		retval = retval*16 + converteddigit;
		}

	return retval;
	}


///////////////////////////////////////////////////
unsigned MStdHToI(const wchar_t *str)
	{
	unsigned int retval=0;

	int i;
	for(i=0;str[i]!=0;++i)
		{
		const wchar_t ch=str[i];
		int converteddigit;
		if(ch>='0' && ch<='9')
			{
			converteddigit = (ch-'0');
			}
		else if(ch>='A' && ch<='F')
			{
			converteddigit = (ch - 'A') + 10;
			}
		else if(ch>='a' && ch<='f')
			{
			converteddigit = (ch - 'a');
			}
		else
			{
			return retval;
			}

		retval = retval*16 + converteddigit;
		}

	return retval;
	}


///////////////////////////////////////////////////
double MStdAToF(const char *str)
	{
	return atof(str);
	}


//////////////////////////////////////////////////
double MStdAToF(const wchar_t *str)
	{
	char buf[100];
	MStdSPrintf(buf,sizeof(buf),"%S",str);
	return atof(buf);
	}


///////////////////////////////////////////////////
void *MStdIToP(int val)
	{
	////////////////////////////////////
	#if (defined(MSTDLIB_OS_WINDOWS)  || defined(MSTDLIB_OS_WINDOWSOLD) )
	return (void *)(__int64)val;

	////////////////////////////////////
	#else
	return (void *)(size_t)val;

	#endif	
	}


///////////////////////////////////////////////////
int MStdPToI(void *val)
	{
	//////////////////////////
	#if (defined(MSTDLIB_OS_WINDOWS)  || defined(MSTDLIB_OS_WINDOWSOLD) )
	return (int)(__int64)val;

	//////////////////////////
	#else	
	return (int)(long)(size_t)(val);
	#endif	// MSTDLIB_OS_WINDOWS
	}



///////////////////////////////////////////////////
int MStdGetMin(const int *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int datamin=data[0];

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]<datamin) { datamin=data[i]; }
		}

	return datamin;
	}


///////////////////////////////////////////////////
float MStdGetMin(const float *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	float datamin=data[0];

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]<datamin) { datamin=data[i]; }
		}

	return datamin;
	}


//////////////////////////////////////////////////
double MStdGetMin(const double *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	double datamin=data[0];

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]<datamin) { datamin=data[i]; }
		}

	return datamin;
	}


//////////////////////////////////////////////////
int MStdGetMax(const int *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int datamax=data[0];

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>datamax) { datamax=data[i]; }
		}

	return datamax;
	}


/////////////////////////////////////////////////
float MStdGetMax(const float *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	float datamax=data[0];

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>datamax) { datamax=data[i]; }
		}

	return datamax;
	}


/////////////////////////////////////////////////
double MStdGetMax(const double *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	double datamax=data[0];

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>datamax) { datamax=data[i]; }
		}

	return datamax;
	}


///////////////////////////////////////////////////
int MStdGetMinIndex(const int *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int index=0;

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]<data[index]) { index=i; }
		}

	return index;
	}


///////////////////////////////////////////////////
int MStdGetMinIndex(const float *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int index=0;

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]<data[index]) { index=i; }
		}

	return index;
	}


//////////////////////////////////////////////////
int MStdGetMinIndex(const double *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int index=0;

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]<data[index]) { index=i; }
		}

	return index;
	}


/////////////////////////////////////////////////
int MStdGetMaxIndex(const int *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int index=0;

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>data[index]) { index=i; }
		}

	return index;
	}


///////////////////////////////////////////////////
int MStdGetMaxIndex(const float *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int index=0;

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>data[index]) { index=i; }
		}

	return index;
	}


////////////////////////////////////////////////////
int MStdGetMaxIndex(const double *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int index=0;

	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>data[index]) { index=i; }
		}

	return index;
	}


/////////////////////////////////////////////////////
int MStdGetSum(const int *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	int sum=0;
	for(int i=0;i<datacount;++i)
		{
		sum = sum + data[i];
		}
	
	return sum;
	}


////////////////////////////////////////////////////
float MStdGetSum(const float *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	float sum=0;
	for(int i=0;i<datacount;++i)
		{
		sum = sum + data[i];
		}
	
	return sum;
	}


////////////////////////////////////////////////////
double MStdGetSum(const double *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);

	double sum=0;
	for(int i=0;i<datacount;++i)
		{
		sum = sum + data[i];
		}
	
	return sum;
	}


///////////////////////////////////////////////////
int MStdGetMidIndex(const double *data,int datacount)
	{
	MStdAssert(data!=NULL && datacount>0);
	
	const double datasum=MStdGetSum(data,datacount);
	const double midsum=datasum/2.0;

	if(datasum>0.0)
		{
		double total=0.0;
		int i;
		for(i=0;i<datacount;++i)
			{
			total = total + data[i];
			if(total>midsum) { return i; }
			}
		}
	
	return 0;
	}




////////////////////////////////////////////////////
void MStdSRand(void)
	{
	time_t tmdata;

	/////////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)

		time(&tmdata);
		return;

	#elif  (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW))
	time(&tmdata);
	tmdata = (time_t) (tmdata ^ GetCurrentProcessId() );

	srand((unsigned int)tmdata);
	return;

	/////////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	time(&tmdata);
	tmdata = (time_t) (tmdata ^ getpid() );

	srand((unsigned int)tmdata);
	return;

	////////////////////////////////////////
	#else
	return;

	#endif // MSTDLIB_OS_WINDOWS
	}


////////////////////////////////////////////////////
void MStdSRand(int seed)
	{
	// Will Reimplement in terms of x<-(Ax+B) mod C 
	// Need to find Good Values for A/B/C
	srand((unsigned int)seed);
	}


////////////////////////////////////////////////////
int MStdRand(int maxvalue)
	{
	// Will Reimplement in terms of x<-(Ax+B) mod C 
	// Need to find Good Values for A/B/C
	return rand()%maxvalue;
	}


///////////////////////////////////////////////////
bool MStdGetUUID(char buf[],int buflen)
	{
	MStdAssert(buf!=0 && buflen>=37);

	////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)
	return false;

	#elif (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	UUID newuuid;
	if(UuidCreate(&newuuid)!=RPC_S_OK)
		{
		return false;
		}

	char *uuidstr;
	if(UuidToStringA(&newuuid,(RPC_CSTR *)&uuidstr)!=RPC_S_OK)
		{
		return false;
		}

	if(MStdStrCpy(buf,buflen,uuidstr)==false)
		{
		// Unable to copy UUID string to output
		}

	RpcStringFreeA((RPC_CSTR *)&uuidstr);
	MStdStrToLower(buf);
	return true;

	////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	uuid_t newuuid;
	uuid_generate(newuuid);
	uuid_unparse(newuuid,buf);
	MStdStrToLower(buf);
	return true;

	#endif // MSTDLIB_OS_WINDOWS	
	return true;
	}

///////////////////////////////////////////////////
unsigned int MStdGetTimeOfDay(void)
	{
	////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)
		time_t val;
		time(&val);
		return (unsigned int)val;

	#elif (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	return GetTickCount();

	////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	// Defined int <sys/time.h>
	struct timeval tv;
	gettimeofday(&tv,0);

	return ((unsigned int)tv.tv_sec)*1000
			+((unsigned int)tv.tv_usec)/1000;

	#endif // MSTDLIB_OS_WINDOWS
	}


////////////////////////////////////////////////////
bool MStdLog(const char *logentry,const char *file,int lineno)
	{
	MStdPrintf("**Log : %s (%s:%d)\n",logentry,file,lineno);
	return true;
	}


/////////////////////////////////////////////////////
// Global Constants
// Reference Constants: http://en.wikipedia.org/wiki/Pi
const double MStdLibConstPi=3.14159265358979323846;


//////////////////////////////////////////////////////
bool MStdGetMachineName(char *buffer,int bufferlen)
	{
	MStdAssert(buffer!=0 && bufferlen>40);
	
	////////////////////////////////////////
	#if defined(MSTDLIB_OS_WINDOWSRT)
	if(buffer!=NULL) { *buffer=0; }
	return false;

	#elif ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	DWORD dwbufferlen=bufferlen;
	if(GetComputerNameA(buffer,&dwbufferlen)==FALSE)
		{
		return false;
		}

	return true;

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(gethostname(buffer,bufferlen)!=0)
		{
		return false;
		}

	return true;

	#endif
	}


//////////////////////////////////////////////////////////
unsigned int MStdGetProcessID(void)
	{
	#if ( defined(MSTDLIB_OS_WINDOWSRT) || defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	return (unsigned int)GetCurrentProcessId();

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )

	return (unsigned int) getpid();

	#endif	
	}


//////////////////////////////////////////////////////////
bool MStdGetOSRoot(char *buffer,int bufferlen)
	{
	MStdAssert(buffer!=0 && bufferlen>40);
	////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_WINDOWSRT) )
	if(MStdStrCpy(buffer,bufferlen,"c:/")==false)
		{
		if(bufferlen>0) { buffer[0]=0; }
		return false;
		}

	return true;

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(MStdStrCpy(buffer,bufferlen,"/")==false)
		{
		if(bufferlen>0) { buffer[0]=0; }
		return false;
		}

	return true;

	#endif	
	}


/////////////////////////////////////////////////////////
bool MStdGetOSPathSeperator(char *buffer,int bufferlen)
	{
	MStdAssert(buffer!=0 && bufferlen>40);
	////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_WINDOWSRT) )
	if(MStdStrCpy(buffer,bufferlen,";")==false)
		{
		if(bufferlen>0) { buffer[0]=0; }
		return false;
		}

	return true;

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	if(MStdStrCpy(buffer,bufferlen,":")==false)
		{
		if(bufferlen>0) { buffer[0]=0; }
		return false;
		}

	return true;

	#endif	
	}


///////////////////////////////////////////////////////////
bool MStdIsUnix(void)
	{
	////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	return false;

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	return true;

	#endif		

	return false;
	}


////////////////////////////////////////////////////////////
bool MStdIsWindows(void)
	{
	////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) || defined(MSTDLIB_OS_WINDOWSRT) )
	return true;

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	return false;

	#endif		

	return false;
	}


//////////////////////////////////////////////////////////////
bool MStdDirGet(char *buffer,int bufferlen)
	{
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	MStdAssert(bufferlen>1 && buffer!=NULL);
	DWORD dlen;
	dlen=GetCurrentDirectoryA(bufferlen-1,buffer);
	if(dlen>=DWORD(bufferlen) || dlen<=1)
		{
		
		return false;
		}

	// Ref http://msdn.microsoft.com/en-us/library/windows/desktop/aa364934%28v=vs.85%29.aspx
	if(buffer[dlen-1]!='\\' && buffer[dlen-1]!='/' )
		{
		buffer[dlen]='/';
		buffer[dlen+1]=0;
		}

	// Convert all \ -> /
	int i;
	for(i=0;buffer[i]!=0;++i)
		{
		if(buffer[i]=='\\') { buffer[i]='/'; }
		}
	
	return true;	

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	MStdAssert(bufferlen>1 && buffer!=NULL);
	const char *retval=getcwd(buffer,bufferlen-1);
	if(retval==NULL)
		{
		return false;
		}

	const int pathlen=MStdStrLen(buffer);
	if(buffer[pathlen-1]!='/')
		{
		buffer[pathlen]='/';
		buffer[pathlen+1]=0;
		}

	return true;

	#endif		

	return false;
	}


/////////////////////////////////////////////////////////////////////////////
bool MStdDirSet(const char *dirpath)
	{
	MStdAssert(dirpath!=NULL && *dirpath!=0);

	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	BOOL retval;
	retval=SetCurrentDirectoryA(dirpath);
	if(retval==FALSE)
		{
		return false;
		}
	
	return true;	

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	int retval;
	retval=chdir(dirpath);
	if(retval!=0)
		{
		return false;
		}

	return true;
	#endif

	return false;
	}


/////////////////////////////////////////////////////////////
bool MStdDirCreate(const char *dirpath)
	{
	MStdAssert(dirpath!=NULL && *dirpath!=0);

	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	BOOL retval;
	retval=CreateDirectoryA(dirpath,NULL);
	if(retval==FALSE)
		{
		return false;
		}
	
	return true;	

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	int retval;
	retval=mkdir(dirpath,0777);
	if(retval!=0)
		{
		return false;
		}

	return true;
	#endif

	return false;
	}


////////////////////////////////////////////////////////////
bool MStdDirDestroy(const char *dirpath,bool generror)
	{
	MStdAssert(dirpath!=NULL && *dirpath!=0);

	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	BOOL retval;
	retval=RemoveDirectoryA(dirpath);
	if(retval==FALSE)
		{
		if(generror==true)
			{
			return false;
			}

		return false;
		}
	
	return true;	

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	int retval;
	retval=rmdir(dirpath);
	if(retval!=0)
		{
		if(generror==true)
			{
			return false;
			}

		return false;
		}

	return true;
	#endif

	return false;
	}


//////////////////////////////////////////////////////
bool MStdGetUserName(char buf[],int buflength)
	{
	MStdAssert(buflength>=30 && buf!=0);

	///////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	DWORD buflen=buflength;
	BOOL ret=GetUserNameA(buf,&buflen);
	if(ret==FALSE)
		{
		buf[0]=0;
		return false;
		}

	return true;

	///////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )

	struct passwd *pdata=getpwuid(getuid());
	if(pdata==NULL)
		{
		buf[0]=0;
		return false;
		}

	MStdStrCpy(buf,pdata->pw_name);
	return true;
	#endif
	}
	

//////////////////////////////////////////////////////
bool MStdStrCpy(char *outstr,int maxoutlen,const wchar_t *str)
	{
	MStdAssert(str!=0 && outstr!=0 && maxoutlen>0);
	for(int i=0;i<maxoutlen-1;++i)
		{
		const wchar_t ch=str[i];
		outstr[i]=(char)ch;
		if(ch==0) { return true; }
		}

	outstr[maxoutlen-1]=0;
	return true;
	}


//////////////////////////////////////////////////////////
bool MStdStrCpy(wchar_t *outstr,int maxoutlen,const char *str)
	{
	MStdAssert(str!=0 && outstr!=0 && maxoutlen>=0);
	for(int i=0;i<maxoutlen-1;++i)
		{
		const char ch=str[i];
		outstr[i] = ch;
		if(ch==0) { return true; }
		}

	outstr[maxoutlen-1]=0;
	return true;
	}


/////////////////////////////////////////////////////////////////////////////
bool MStdStrCpy(MStdArray<wchar_t> &strout,const char *str)
	{
	MStdAssert(str!=0);
	const int length=MStdStrLen(str)+1;
	if(strout.Create(length)==false)
		{
		return false;
		}

	MStdStrCpy(strout.Get(),strout.GetLength(),str);
	return true;
	}


/////////////////////////////////////////////////////////////////////////////
bool MStdStrCpy(MStdArray<char> &strout,const wchar_t *str)
	{
	MStdAssert(str!=0);
	const int length=MStdStrLen(str)+1;
	if(strout.Create(length)==false)
		{
		return false;
		}

	MStdStrCpy(strout.Get(),strout.GetLength(),str);
	return true;
	}


//////////////////////////////////////////////////
bool MStdPathSetSlash(char *path)
	{
	MStdAssert(path!=NULL);
	char *p;
	for(p=path;*p!=0;++p)
		{
		if(*p=='\\'){  *p = '/';  }
		}

	// Remove Trailing / if it exists
	if(p>path && *(p-1)=='/')
		{
		*(p-1)=0;
		}

	return true;	
	}


//////////////////////////////////////////////////////////
bool MStdPathGetAbsolute(const char *filename,MStdArray<char> &absolutepath)
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	if(absolutepath.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	/////////////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )

	char *filenameonly=NULL;
	DWORD length=GetFullPathNameA(filename,absolutepath.GetLength()-2,
			absolutepath.Get(),&filenameonly);
	if(length==0)
		{
		return false;
		}

	if(length>=(DWORD)absolutepath.GetLength())
		{
		return false;
		}

	// Convert to Canonical Path with \ converted to /
	MStdPathSetSlash(absolutepath.Get() );
	
	return true;
	/////////////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )

	if(MStdFileExists(filename)==true)
		{
		if(realpath(filename,absolutepath.Get())==NULL)
			{
			return false;
			}

		return true;
		}
	else if(*filename=='/')
		{
		//File is already an absolute path
		MStdStrCpy(absolutepath.Get(),absolutepath.GetLength()-2,filename);
		return true;
		}
	else
		{
		// Build up path using cwd
		char *bufptr=absolutepath.Get();
		getcwd(bufptr,absolutepath.GetLength() );

		int length=MStdStrLen(bufptr);
		if(length<=0)
			{
			return false;
			}

		if(bufptr[length-1]!='/')
			{
			MStdStrCat(bufptr,absolutepath.GetLength(),"/");
			}

		MStdStrCat(bufptr,absolutepath.GetLength(),filename);
		return true;
		}

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////////////////////
bool MStdDirGet(MStdArray<char> &path)
	{
	if(path.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	
	DWORD dlen;
	char *buffer=path.Get();
	dlen=GetCurrentDirectoryA(path.GetLength()-2,path.Get());
	if(dlen>=DWORD(path.GetLength()-2) || dlen<=1)
		{
		
		return false;
		}

	// Ref http://msdn.microsoft.com/en-us/library/windows/desktop/aa364934%28v=vs.85%29.aspx
	if(buffer[dlen-1]!='\\' && buffer[dlen-1]!='/' )
		{
		buffer[dlen]='/';
		buffer[dlen+1]=0;
		}

	// Convert all \ -> /
	int i;
	for(i=0;buffer[i]!=0;++i)
		{
		if(buffer[i]=='\\') { buffer[i]='/'; }
		}
	
	return true;	

	////////////////////////////////////////
	#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) || defined(MSTDLIB_OS_IPHONE) )
	char *buffer=path.Get();
	const char *retval=getcwd(buffer,path.GetLength()-2);
	if(retval==NULL)
		{
		return false;
		}

	const int pathlen=MStdStrLen(buffer);
	if(buffer[pathlen-1]!='/')
		{
		buffer[pathlen]='/';
		buffer[pathlen+1]=0;
		}

	return true;

	#endif

	return false;	
	}


/////////////////////////////////////////////////////////////
bool MStdFileReadText(const char *filename,MStdArray<char> &data)
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	// Check if file exists
	if(MStdFileExists(filename)==false)
		{
		return false;
		}

	// Check if file is binary
	if(MStdFileIsBinary(filename)==true)
		{
		return false;
		}

	// Read from file
	MStdFileHandle file=MStdFileOpen(filename,"rb");
	if(file==NULL)
		{
		return true;
		}

	// seek to end of file
	MStdFileSeek(file,0,2);

	long filesize=MStdFileTell(file);
	if(filesize<0)
		{
		MStdFileClose(file);
		return true;
		}

	// Seek back to beginning
	MStdFileSeek(file,0,0);

	// Allocate buffer space
	if(data.Create(filesize+1)==false)
		{
		MStdFileClose(file);
		return false;
		}

	int len=MStdFileRead(file,data.Get(),1,filesize);
	if(len!=filesize)
		{
		data.Destroy();
		MStdFileClose(file);
		return false;
		}

	MStdFileClose(file);
	data.Get()[filesize]=0;
	return true;
	}


