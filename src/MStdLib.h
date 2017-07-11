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
#ifndef MStdLib_h
#define MStdLib_h

/* 
Note Visual Studio
	MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
	MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
	MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
	MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
	MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
	MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
	MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
	MSVC++ 7.0  _MSC_VER == 1300
	MSVC++ 6.0  _MSC_VER == 1200
	MSVC++ 5.0  _MSC_VER == 1100
*/

//********************************************
//** Begin of Platform Definitions
//********************************************
#if defined(_MSC_VER)

	// Older version of MSVC did not have wchar.h
	#if (_MSC_VER>1300 )

		#if defined(WINAPI_FAMILY)
			#if defined(WINAPI_FAMILY_DESKTOP_APP)
				#ifndef MSTDLIB_OS_WINDOWS
				#define MSTDLIB_OS_WINDOWS							(1)
				#endif // MSTDLIB_OS_WINDOWS			
			#else
				#ifndef MSTDLIB_OS_WINDOWSRT
				#define MSTDLIB_OS_WINDOWSRT						(1)
				#endif // MSTDLIB_OS_WINDOWSRT
			#endif 
		#else 

			#ifndef MSTDLIB_OS_WINDOWS
			#define MSTDLIB_OS_WINDOWS							(1)
			#endif // MSTDLIB_OS_WINDOWS

		#endif
	#else

		#ifndef MSTDLIB_OS_WINDOWSOLD
		#define MSTDLIB_OS_WINDOWSOLD						(1)
		#endif // MSTDLIB_OS_WINDOWSOLD

	#endif // _MSVCER

/////////////////////////////////////////
#elif ( defined(__MINGW32__) || defined(__MINGW64__) )

	#ifndef MSTDLIB_OS_MINGW
	#define MSTDLIB_OS_MINGW							(1)
	#endif  //MSTDLIB_OS_MINGW

/////////////////////////////////////////
#elif defined(__linux__)

	#ifndef MSTDLIB_OS_LINUX
	#define MSTDLIB_OS_LINUX							(1)
	#endif // MSTDLIB_OS_LINUX

#elif defined(__APPLE__)

	#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
		#ifndef MSTDLIB_OS_IPHONE
		#define MSTDLIB_OS_IPHONE							(1)
		#endif // MSTDLIB_OS_IPHONE

	#else
	
		#ifndef MSTDLIB_OS_MACOS
		#define MSTDLIB_OS_MACOS							(1)
		#endif // MSTDLIB_OS_MACOS
	
	#endif // __IPHONE_OS_VERSION_MIN_REQUIRED

/////////////////////////////////////////
#else

	#ifndef MSTDLIB_OS_OTHER
	#define MSTDLIB_OS_OTHER						(1)
	#endif // MSTDLIB_OS_OTHER

#endif

//********************************************
//** End of Platform Definitions
//********************************************


//********************************************
//** API Defines for simpler defines: Windows, Linux, Other
//********************************************
#if (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	#define	MSTDLIB_API_WINDOWS							(1)

#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_MACOS) || defined(MSTDLIB_IPHONE) )
	#define MSTDLIB_API_LINUX							(1)

#else
	#define MSTDLIB_API_OTHER							(1)

#endif



//*******************************************
//* Display Standard Header Files
//*******************************************
#if defined(MSTDLIB_OS_WINDOWS)
#include <time.h>
#include <math.h>
#include <new>

#elif (defined(MSTDLIB_OS_WINDOWSRT) )
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <new>
#include <stdarg.h>

#elif (defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
#include <time.h>
#include <math.h>
#include <wchar.h>
#include <new>

#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) || defined(MSTDLIB_OS_MACOS) )
#include <time.h>
#include <math.h>
#include <wchar.h>
#include <new>

#elif defined(MSTDLIB_OS_IPHONE)
#include <time.h>
#include <math.h>
#include <wchar.h>
#include <new>

#endif // MSTDLIB_OS_WINDOWS

///////////////////////////////////////////////
#ifndef	NULL
#define NULL			(0)
#endif

//********************************************
//** Assert Routines
//********************************************
#ifdef NDEBUG
#define MStdAssert(exp)
#else
////////////////////////////////////////////////////////////////
#define MStdAssert(exp)	MStdAssertInternal(exp,#exp,__FILE__,__LINE__)
#endif // NDEBUG

//********************************************
//** Error Routines
//********************************************
#ifndef MStdDebug
#define MStdDebug(infostr)		MStdError(infostr,__FILE__,__LINE__)
#endif // MStdDebug

///////////////////////////////////////////////////
bool MStdPrintInfo(void);		// Print out information

//////////////////////////////////////////////////
void MStdAssertInternal(bool flag,const char *exp,const char *filename,int lineno);
void MStdError(const char *info,const char *filename,int lineno);
void MStdBreak(void);									// Break Point in Code

//************************************************
//*  Limits
//************************************************
const int MStdPathMaxSize=4096;


/////////////////////////////////////////////////
// Abstract File Handles
typedef void *MStdFileHandle;

//////////////////////////////////////////////////
MStdFileHandle MStdFileOpen(const char *filename,const char *mode);		// Open file
bool MStdSetStdInMode(bool binary=false);				// Reset Mode to binary/text
bool MStdSetStdOutMode(bool binary=false);				// Open StdInput
bool MStdFileClose(MStdFileHandle handle);				// Close File Handle
MStdFileHandle MStdGetStdIn(void);						// Get File stdin
MStdFileHandle MStdGetStdOut(void);						// Get File stdout
bool MStdFileWrite(MStdFileHandle handle,const char *stri);		// Write a string
int MStdFileWrite(MStdFileHandle handle,const void *buffer
		,int elementsize,int elementcount);				// Write to file
int MStdFileRead(MStdFileHandle handle,void *buffer,int elementsize
		,int elementcount);								// Read file
bool MStdFileGetString(MStdFileHandle handle,char *buffer,int buffersize);
bool MStdFileFlush(MStdFileHandle handle);				// Flush the file
bool MStdFileSeek(MStdFileHandle handle,int offset,int type);	//type 0-start,1-middle,2-end
long MStdFileTell(MStdFileHandle handle);				// Get Position in file
bool MStdFileEOF(MStdFileHandle handle);				// =true if endof file

//////////////////////////////////////////////////
// Console Operations
int MStdPrintf(const char *format,...);					// Console Output
int MStdErrPrintf(const char *format,...);				// Console Error
bool MStdGetString(char *buffer,int size);				// Read Console String
bool MStdOpenConsoleIO(void);							// Reopen console Input
bool MStdSetOutput(const char *filename);				// Set Standard ouput to file
bool MStdSetInput(const char *filename);				// Set Standard Input from file
bool MStdSetError(const char *filename);				// Set Standard Error to file

//////////////////////////////////////////////////
// String Operations
int MStdSPrintf(char *target,int targetsize,const char *format,...);	// String Formated Printf
const char *MStdStr(bool value);						// Build a temp string
const char *MStdStr(int value);							// Build a temp string
const char *MStdStr(float value);						// Build a temp string
const char *MStdStr(double value);						// Build a temp string

bool MStdStr(bool value,char strout[],int stroutlen);	// Convert type to string		
bool MStdStr(int value,char strout[],int stroutlen);	// Convert type to string		
bool MStdStr(float value,char strout[],int stroutlen);	// Convert type to string		
bool MStdStr(double value,char strout[],int stroutlen);	// Convert type to string		


/////////////////////////////////////////////////
// Wide String Operations
int MStdSPrintf(wchar_t *target,int targetsize,const wchar_t *format,...);	// String Formated Printf
const wchar_t *MStdStrWide(bool value);					// Build a temp string
const wchar_t *MStdStrWide(int value);					// Build a temp string
const wchar_t *MStdStrWide(float value);				// Build a temp string
const wchar_t *MStdStrWide(double value);				// Build a temp string

bool MStdStrWide(bool value,wchar_t strout[],int stroutlen);	// Convert type to string		
bool MStdStrWide(int value,wchar_t strout[],int stroutlen);		// Convert type to string		
bool MStdStrWide(float value,wchar_t strout[],int stroutlen);	// Convert type to string		
bool MStdStrWide(double value,wchar_t strout[],int stroutlen);	// Convert type to string		


//////////////////////////////////////////////////
bool MStdStrIsMember(char ch,const char *str);			// Check if char is in string
bool MStdStrIsMember(wchar_t ch,const wchar_t *str);	// Check if char is in string
bool MStdStrRemoveChars(char *modifystr,const char *removechars);
bool MStdStrRemoveChars(wchar_t *modifystr,const wchar_t *removechars);

//////////////////////////////////////////////////
// Extra Functions
bool MStdWindowOutput(const char *title,const char *info);	// Dialog Box if availible
bool MStdSleep(int ms);									// Sleep for some ms
bool MStdExit(int value);								// Exit application

//////////////////////////////////////////////////
// Char Functions
bool MStdIsNan(double val);								// Check if float is Not A Number
bool MStdIsFinite(double val);							// Check if number is finite

/////////////////////////////////////////////////
bool MStdGetEnvVar(const char *var,char *buf,int buflen);		// Get Environment variable
bool MStdGetUserHome(char *buf,int buflen);				// Return the home directory of user


//////////////////////////////////////////////////
// Memory Functions
bool MStdMemCpy(void *target,const void *src,int count);		// Copy block of memory
bool MStdMemSet(void *,int value,int size);				// Set Buffer with zero
bool MStdMemZero(void *,int size);						// Set buffer to zero
int MStdMemCmp(const void *src,const void *dst,int size);		// Compare two memory blocks
bool MStdMemMove(void *dst,void *src,int count);		// Move Memory
void *MStdMalloc(int size);								// Allocate block of memory
bool MStdFree(void *memblock);							// Free Block of memory
void *MStdReAlloc(void *memblock,int newsize);			// Realloc memory with different size

//////////////////////////////////////////////////
bool MStdKbHit(void);									// Check if keyboard clicked
int MStdGetCh(bool forcestdio=false);					// Get Entered char

//////////////////////////////////////////////////
time_t MStdGetTime(void);								// Get second count from epoch 1970
bool MStdCTime(char *buf,int bufsize,time_t *tm);		// Convert time into a string
bool MStdAtExit(void fun(void));						// Exit Function
bool MStdFileRemove(const char *filename);				// Remove a file
bool MStdFileRename(const char *filesrc,const char *filetarget);	// Rename a file
bool MStdFileCopy(const char *srcfile,const char *dstfile
	,bool stopifexists=false,bool erroronfail=false);	// Copy source file to target
bool MStdFileExists(const char *filename);				// Check if file exists
bool MStdFileIsBinary(const char *filename);			// Check if file is binary
bool MStdFileReadText(const char *filename,char *buffer
		,int &size);									// Read from file into buffer
bool MStdFileWriteText(const char *filename,const char *data);	// Write a text file
bool MStdExec(const char *cmd);							// Execute Command

///////////////////////////////////////////////////
// Conversion Functions
int MStdAToI(const char *str);							// Convert string to integer
int MStdAToI(const wchar_t *str);						// Convert string to integer
bool MStdIToA(int inval,char *buffer,int length);		// Convert an integer to a string
bool MStdIToA(int inval,wchar_t *buffer,int length);	// Convert an integer to a string
unsigned int MStdHToI(const char *str);					// Convert hex string  to integer
unsigned int MStdHToI(const wchar_t *str);				// Convert hex string to integer
double MStdAToF(const char *str);						// Convert string to double
double MStdAToF(const wchar_t *str);					// Convert string to double
void *MStdIToP(int val);								// Convert integer to pointer
int MStdPToI(void *val);								// Convert pointer to integer

//// Get Sum of array
int MStdGetMidIndex(const double *data,int datacount);	// Get the first index of the approximate middle element assuming mostly positive data

//////////////////////////////////////////////////
// Random Number Generation
void MStdSRand(void);									// Seed based on time
void MStdSRand(int seed);								// Seed
int MStdRand(int range=32767);							// Weak Random Number

/////////////////////////////////////////////////
bool MStdGetUUID(char buf[],int buflen);

//*******************************************************************
//** Template Functions
//*******************************************************************
////////////////////////////////////////////////////////
// Check if type is printable character
template <typename Type>
bool MStdIsPrintable(Type ch)
	{
	if(ch>=32 && ch<=126) { return true; }
	return false;
	};


////////////////////////////////////////////////////////
// Check if character is a space
template <typename Type>
bool MStdIsSpace(Type ch)
	{
	if(ch==' ' || ch=='\r' || ch=='\n' || ch=='\t')
		{
		return true;
		}

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is a digit
template <typename Type>
bool MStdIsDigit(Type ch)
	{
	if(ch>='0' && ch<='9')
		{
		return true;
		}

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is an alphabetic
template <typename Type>
bool MStdIsAlpha(Type ch)
	{
	if(ch>='a' && ch<='z')
		{
		return true;
		}

	if(ch>='A' && ch<='Z')
		{
		return true;
		}

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is a alpgabetic or a digit
template <typename Type>
bool MStdIsAlphaNumeric(Type ch)
	{
	if(ch>='a' && ch<='z')
		{
		return true;
		}

	if(ch>='A' && ch<='Z')
		{
		return true;
		}

	if(ch>='0' && ch<='9')
		{
		return true;
		}

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is uppercase
template <typename Type>
bool MStdIsUpper(Type ch)
	{
	if(ch>='A' && ch<='Z')
		{
		return true;
		}

	return false;	
	};


////////////////////////////////////////////////////////
// Check if character is lowercase
template <typename Type>
bool MStdIsLower(Type ch)
	{
	if(ch>='a' && ch<='z')
		{
		return true;
		}

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is binary
template <typename Type>
bool MStdIsBinary(Type ch)
	{
	if(ch=='0' || ch =='1')
		{ return true; }

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is octal
template <typename Type>
bool MStdIsOctal(Type ch)
	{
	if(ch>='0' && ch <='7')
		{ return true; }

	return false;
	};


////////////////////////////////////////////////////////
// Check if character is a digit
template <typename Type>
bool MStdIsHex(Type ch)
	{
	if(ch>='0' && ch <='9')
		{ return true; }

	if(ch>='a' && ch<='f')
		{ return true;  }

	if(ch>='A' && ch<='F')
		{ return true;  }

	return false;	
	};


////////////////////////////////////////////////////////
// Convert character from lower case to uppercase
template<typename Type>
Type MStdToLower(Type ch)
	{
	if(ch>='A' && ch<='Z')
		{ ch = ch - 'A' + 'a'; }

	return ch;
	};


///////////////////////////////////////////////////////
// Convert character to upper case
template <typename Type>
Type MStdToUpper(Type ch)
	{
	if(ch>='a' && ch<='z')
		{ ch = ch - 'a' + 'A'; }

	return ch;
	};


////////////////////////////////////////////////////////
// Convert Binary Character to integer
template<typename Type>
int MStdFromBinary(Type ch)
	{
	MStdAssert(ch=='0' || ch=='1');
	if(ch=='0')
		{  return 0; }

	if(ch=='1')
		{  return 1;  }

	MStdDebug("Unable to convert char to binary");
	return 0;
	}


////////////////////////////////////////////////////////
// Convert Octal Character to integer
template<typename Type>
int MStdFromOctal(Type ch)
	{
	MStdAssert(ch>='0' || ch<='7');
	return ch-'0';
	}


////////////////////////////////////////////////////////
// Convert Hexadecimal Character to integer
template<typename Type>
int MStdFromHex(Type ch)
	{
	if(ch>='0' && ch<='9')
		{  return ch-'0';  }

	if(ch>='a' && ch<='f')
		{  return 10+ch-'a';  }

	if(ch>='A' && ch<='F')
		{  return 10+ch-'A';  }

	return 0;
	}


//////////////////////////////////////////////////
// Place Random Integers into array
template<typename Type>
bool MStdRand(Type ar[],int count)
	{
	Type *p;
	for(p=ar+count-1;p>=ar;--p)
		{
		*p = (Type) MStdRand();
		}

	return true;
	}


//////////////////////////////////////////////////
// Compare Two Numbers
template<typename Type>
bool MStdCompare(const Type &val1,const Type &val2,const Type &error)
	{
	MStdAssert(error>=0);
	if(val1>=val2-error && val1<=val2+error)
		{
		return true;
		}

	return false;
	};

//////////////////////////////////////////////////
// Compare Two Numbers
template<typename Type>
bool MStdInRange(const Type &val, const Type &minval, const Type &maxval)
	{
	MStdAssert(minval<=maxval);
	if (val>= minval && val <=maxval)
		{
		return true;
		}

	return false;
	};


//////////////////////////////////////////////////
// Time Functions
unsigned int MStdGetTimeOfDay(void);


//////////////////////////////////////////////////
// Log File
bool MStdLog(const char *logentry,const char *file,int lineno);

//////////////////////////////////////////////////
// Power Function
inline double MStdPower(const double &base,const double &exp)
	{  return pow(base,exp);  }

//////////////////////////////////////////////////
// sqrt function
inline double MStdSqrt(const double &val)
	{  return sqrt(val);  }


//////////////////////////////////////////////////
// Some Well Known Constants
extern const double MStdLibConstPi;

//////////////////////////////////////////////////
// Platform Specific Information
bool MStdGetMachineName(char *buffer,int bufferlen);
unsigned int MStdGetProcessID(void);							// Get Current Process ID
bool MStdGetOSRoot(char *buffer,int bufferlen);					// returns: "c:/" or "/" based on os
bool MStdGetOSPathSeperator(char *buffer,int bufferlen);		// returns: ";" or ":"
bool MStdIsUnix(void);											//=true if UNIX like OS (Linux,...)
bool MStdIsWindows(void);										//=true if on MS Windows OS
bool MStdDirGet(char *buffer,int bufferlen);					// Gets the current working directory "C:/../". "/" is always in the directory
bool MStdDirSet(const char *dirpath);							// Sets the current directory
bool MStdDirCreate(const char *dirpath);						// Create a new directory
bool MStdDirDestroy(const char *dirpath,bool generror=false);	// Remove a directory
bool MStdGetUserName(char buf[],int buflength);					// Get the user name of current user


//////////////////////////////////////////////////
// Call back functions
struct MStdLibIAction
	{
	virtual bool OnAction(void)=0;
	};


//////////////////////////////////////////////////
// Simple Tamplate Functions
//////////////////////////////////////////////////

/////////////////////////////////////////////////
// Template Functor
template<typename Type>
struct MStdFunctor
	{
	virtual Type operator()(Type ref)=0;
	};


//////////////////////////////////////////////
template<typename Type>
void MStdSwap(Type &one,Type &two)
	{
	Type tmp;
	tmp=one;
	one=two;
	two=tmp;
	}


//////////////////////////////////////////////
// Sort of two variables
template<typename Type>
void MStdSortAscending(Type &var1,Type &var2)
	{
	if(var1>var2) {  MStdSwap(var1,var2);  }
	};


//////////////////////////////////////////////
// Sort Three Variables
template<typename Type>
void MStdSortAscending(Type &var1, Type &var2,Type &var3)
	{
	if (var1>var2) { MStdSwap(var1,var2); }
	if(var2>var3) { MStdSwap(var2,var3); }
	if(var1>var2) { MStdSwap(var1,var2); }
	};


//////////////////////////////////////////////
// Sort of passed arrays into ascending order
template<typename Type>
bool MStdSortAscending(Type ar[],int count)
	{
	MStdAssert(count>0);
	// Simple \Theta(n^2) sort
	Type *p,*q;
	for(p=ar+count-1;p>ar;--p)
		{
		for(q=ar;q<p;++q)
			{
			if(*p < *q) { MStdSwap(*p,*q); }
			}
		}

	return true;
	};


//////////////////////////////////////////////
// Sort of two variables
template<typename Type>
void MStdSortDescending(Type &var1,Type &var2)
	{
	if(var1<var2) {  MStdSwap(var1,var2);  }
	};


//////////////////////////////////////////////
// Sort Three Variables
template<typename Type>
void MStdSortDescending(Type &var1, Type &var2,Type &var3)
	{
	if (var1<var2) { MStdSwap(var1,var2); }
	if(var2<var3) { MStdSwap(var2,var3); }
	if(var1<var2) { MStdSwap(var1,var2); }
	};



//////////////////////////////////////////////
// Sort of passed arrays into descending order
template<typename Type>
bool MStdSortDescending(Type ar[],int count)
	{
	MStdAssert(count>0);
	// Simple \Theta(n^2) sort
	Type *p,*q;
	for(p=ar+count-1;p>ar;--p)
		{
		for(q=ar;q<p;++q)
			{
			if(*p > *q) { MStdSwap(*p,*q); }
			}
		}

	return true;
	};


///////////////////////////////////////
// check if array is increasing array
template<typename Type>
bool MStdIsNonDecreasing(Type ar[],int count)
	{
	MStdAssert(count>0);

	Type *p;
	for(p=ar+count-1;p>ar;--p)
		{
		if(*(p-1)>*p) { return false; }
		}
	
	return true;
	};


///////////////////////////////////////
// Check if array is in decreasing array
template<typename Type>
bool MStdIsNonIncreasing(Type ar[],int count)
	{
	MStdAssert(count>0);

	Type *p;
	for(p=ar+count-1;p>ar;--p)
		{
		if(*(p-1)<*p) { return false; }
		}
	
	return true;
	};


///////////////////////////////////////////////////
// Get Maximum Value
template<typename Type>
Type MStdGetMax(Type value1,Type value2)
	{
	if(value1>value2) { return value1; }
	return value2;
	};


///////////////////////////////////////////////////
// Get Minimum Value
template<typename Type>
Type MStdGetMin(Type value1,Type value2)
	{
	if(value1<value2) { return value1; }
	return value2;
	};


///////////////////////////////////////////////////
// Get Absolute Value
template<typename Type>
Type MStdGetAbs(Type val)
	{
	if(val<0) { return -val; }
	return val;
	};


///////////////////////////////////////////////////
// Get the Min Value in the array
template<typename Type>
Type MStdGetMin(Type const * const ar,int datacount)
	{
	MStdAssert(ar!=NULL && datacount>0);
	Type const * const endpos=ar+datacount;
	Type minval=*ar;
	Type const *p;
	for(p=ar+1;p<endpos;++p)
		{
		if(*p<minval) { minval=*p;  }
		}

	return minval;
	};


///////////////////////////////////////////////////
// Get the Max Value in the array
template<typename Type>
Type MStdGetMax(Type const * const ar,int datacount)
	{
	MStdAssert(ar!=NULL && datacount>0);
	Type const * const endpos=ar+datacount;
	Type maxval=*ar;
	Type const *p;
	for(p=ar+1;p<endpos;++p)
		{
		if(*p>maxval) { maxval=*p;  }
		}

	return maxval;
	};


///////////////////////////////////////////////////
// Get the Max Index Position
template<typename Type>
int MStdGetMaxIndex(const Type data[],int datacount)
	{
	int maxindex=0;
	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>data[maxindex]) { maxindex=i; }
		}

	return maxindex;
	};


///////////////////////////////////////////////////
// Get the Minimum Index position
template<typename Type>
int MStdGetMinIndex(const Type data[],int datacount)
	{
	int minindex=0;
	int i;
	for(i=1;i<datacount;++i)
		{
		if(data[i]>data[minindex]) { minindex=i; }
		}

	return minindex;
	};


///////////////////////////////////////////////////
// Find the sum of an array
template<typename Type>
Type MStdGetSum(const Type ar[],int datacount)
	{
	Type sum=0;
	int i;
	for(i=0;i<datacount;++i)
		{
		sum = sum + ar[i];
		}

	return sum;
	};

///////////////////////////////////////////////////
// Check if member is in array
template<typename Type>
bool MStdIsMember(Type member,const Type ar[],int datacount)
	{
	for(int i=0;i<datacount;++i)
		{
		if(ar[i]==member) { return true; }
		}

	return false;
	}


////////////////////////////////////////////////////
// Find the absolute of a passed value
template <typename Type>
Type MStdAbs(const Type &val)
	{
	if(val<0) { return -val; }
	return val;
	};


////////////////////////////////////////////////
// String Copy
template<typename Type>
bool MStdStrCpy(Type *dst,const Type *src)
	{
	Type *q=dst;
	const Type *s=src;
	while((*q++=*s++)!=0)
		{
		// Empty Body
		}

	return true;
	}


//////////////////////////////////////////////
// String Copy
template <typename Type>
bool MStdStrCpy(Type *dst,int dstsize,const Type *src)
	{
	Type *q=dst;
	const Type *s=src;
	int length;
	for(length=1;(*q++=*s++)!=0;length=length+1)
		{
		if(length>=dstsize)
			{
			dst[dstsize-1]=0;
			return false;
			}
		}
	
	return true;	
	};


///////////////////////////////////////////////////
// String Length
template <typename Type>
int MStdStrLen(const Type *src)
	{
	int length;
	for(length=0;*src!=0;++src)
		{  length=length+1;  }

	return length;	
	};


////////////////////////////////////////////
// String Copy
template<typename Type>
int MStdStrCmp(const Type *str1,const Type *str2)
	{
	const Type *p=str1;
	const Type *q=str2;
	for(;;)
		{
		if(*p!=*q)
			{
			// Return first difference
			return *p - *q;
			}

		// check for exit
		if(*p==0) { return 0; }

		// Increase pointers
		p=p+1;
		q=q+1;
		}	
	};


///////////////////////////////////////
// Insensitive Compare
template <typename Type>
int MStdStrICmp(const Type *str1,const Type *str2)
	{
	Type ch1;
	Type ch2;
	const Type *p=str1;
	const Type *q=str2;
	for(;;)
		{
		ch1=MStdToLower(*p);
		ch2=MStdToLower(*q);
		if(ch1!=ch2)
			{
			// Return first difference
			return ch1 - ch2;
			}

		// check for exit
		if(ch1==0) { return 0; }

		// Increase pointers
		p=p+1;
		q=q+1;
		}		
	};


////////////////////////////////////////////////
// Catenate one string to another
template<typename Type>
bool MStdStrCat(Type *dst,int dstsize,const Type *src)
	{
	// Find end of string
	int endindex;
	for(endindex=0;dst[endindex]!=0;endindex=endindex+1)
		{
		// Nothing to Do
		}

	int srcendindex;
	for(srcendindex=0;src[srcendindex]!=0;++srcendindex)
		{
		// Nothing to do
		}

	//=End index is at the \0 of the first string

	if(endindex+srcendindex+1>dstsize)
		{
		return false;
		}

	int i;
	for(i=0; ;++i)
		{
		dst[endindex]=src[i];
		if(src[i]==0) { return true; }

		// Increase the endindex by 1
		endindex = endindex + 1;
		}	
	};


/////////////////////////////////////////////////////
// Check if a prefix string starts another
template<typename Type>
bool MStdStrBegins(const Type *str,const Type *prefix)
	{
	if(str==NULL || prefix==NULL || *prefix==0)
		{
		// Bad Input strings
		return false;
		}

	int i;
	for(i=0;prefix[i]!=0;++i)
		{
		if(str[i]!=prefix[i])
			{
			return false;
			}
		}

	//=We checked all the chars upto 0

	return true;
	};


//////////////////////////////////////////////////////
// Check if a case insensitve prefix string starts another
template <typename Type>
bool MStdStrIBegins(const Type *str,const Type *prefix)
	{
	if(str==NULL || prefix==NULL || *prefix==0)
		{
		// Bad Input strings
		return false;
		}

	int i;
	for(i=0;prefix[i]!=0;++i)
		{
		if(MStdToUpper(str[i])!=MStdToUpper(prefix[i]) )
			{
			return false;
			}
		}

	//=We checked all the chars upto 0

	return true;	
	};


///////////////////////////////////////////////////////
// Check if a string is a suffix of another string
template<typename Type>
bool MStdStrEnds(const Type *str,const Type *suffix)
	{
	MStdAssert(str!=NULL);
	MStdAssert(suffix!=NULL);

	// Find the end of the string
	int length1=MStdStrLen(str);
	int length2=MStdStrLen(suffix);

	if(length2>length1)
		{  return false;  }

	if(length2==length1)
		{  return MStdStrBegins(str,suffix);  }
	
	//=Now we should check for comparison
	
	return MStdStrBegins(str+length1-length2,suffix);
	};


////////////////////////////////////////////////////////
// Check if a case insensitive string is a suffix of a string
template<typename Type>
bool MStdStrIEnds(const Type *str,const Type *suffix)
	{
	MStdAssert(str!=NULL);
	MStdAssert(suffix!=NULL);

	// Find the end of the string
	int length1=MStdStrLen(str);
	int length2=MStdStrLen(suffix);

	if(length2>length1)
		{  return false;  }

	if(length2==length1)
		{  return MStdStrIBegins(str,suffix);  }
	
	//=Now we should check for comparison
	
	return MStdStrIBegins(str+length1-length2,suffix);
	};


/////////////////////////////////////////////////////////
// Check if substring is in a string
template<typename Type>
bool MStdIsSubStr(const Type *str,const Type *substring)
	{
	// Check if either are strings
	if(str==NULL || substring==NULL || *substring==0)
		{
		return false;
		}

	int i;
	for(i=0;str[i]!=0;++i)
		{
		if(MStdStrBegins(str+i,substring)==true)
			{
			//=We have found the substring
			return true;
			}
		}

	return false;
	};


//////////////////////////////////////////////////////////
// Check if the case insensitive substring is in a string
template <typename Type>
bool MStdIsISubStr(const Type *str,const Type *substring)
	{
	// Check if either are strings
	if(str==NULL || substring==NULL || *substring==0)
		{
		return false;
		}

	int i;
	for(i=0;str[i]!=0;++i)
		{
		if(MStdStrIBegins(str+i,substring)==true)
			{
			//=We have found the substring
			return true;
			}
		}

	return false;
	};


////////////////////////////////////////////////////
// Lowers the String Case
template<typename Type>
bool MStdStrToLower(Type *str)
	{
	if(str==NULL)
		{  return false;  }

	int i;
	for(i=0;str[i]!=0;++i)
		{  str[i]=MStdToLower(str[i]);  }

	return true;
	};


/////////////////////////////////////////////
// Upper Cases the string
template <typename Type>
bool MStdStrToUpper(Type *str)
	{
	if(str==NULL)
		{  return false;  }

	int i;
	for(i=0;str[i]!=0;++i)
		{  str[i]=MStdToUpper(str[i]);  }

	return true;
	};


///////////////////////////////////////////////////
// Remove a sub portion of a string
template<typename Type>
bool MStdStrRemove(Type *target,int pos,int length)
	{
	if(target==NULL)
		{  return false; }

	int strlength=MStdStrLen(target);
	if(pos<0 || pos>strlength)
		{
		return false;
		}

	if(pos+length>strlength)
		{  length=strlength-pos;  }

	// Now we start copying overwriting original
	int i;
	for(i=0; ;++i)
		{
		target[pos+i]=target[pos+length+i];
		if(target[pos+i]==0)
			{  break;  }
		}

	return true;
	};


/////////////////////////////////////////////////////////
// Insert a string into another
template<typename Type>
bool MStdStrInsert(Type *target,int maxtargetsize,int pos,const Type *insert)
	{
	if(target==NULL || insert==NULL)
		{  return false;  }

	int targetlength=MStdStrLen(target);
	int sourcelength=MStdStrLen(insert);
	if(targetlength+sourcelength>=maxtargetsize)
		{
		return false;
		}

	if(pos<0 || pos>targetlength)
		{
		return false;
		}
	
	// Copy backwards
	int i;
	for(i=targetlength;i>=pos;--i)
		{
		target[i+sourcelength]=target[i];
		}

	// Place String at position
	for(i=0;i<sourcelength;++i)
		{
		target[pos+i]=insert[i];
		}

	return true;
	};


/////////////////////////////////////////////
// Find count of Type Values in a String
template<typename Type>
int MStdStrCharCount(const Type *str,char ch)
	{
	if(str==NULL)
		{  return 0; }

	const Type *p;
	int count;

	count=0;
	for(p=str;*p!=0;++p)
		{
		if(*p==ch)
			{ count=count+1;  }
		}

	return count;
	};


/////////////////////////////////////////////////////////
// Trim the string for blanks
template<typename Type>
bool MStdStrTrim(Type *str)
	{
	Type *src;
	Type *dst;
	if(str==NULL)
		{
		return false;
		}

	for(src=dst=str;*src!=0;++src)
		{
		if(MStdIsSpace(*src)==true)
			{ continue; }

		*dst++=*src;
		}

	*dst=0;
	return true;
	};


/////////////////////////////////////////////////////////
// Trim spaces from the right of the string
template<typename Type>
bool MStdStrTrimRight(Type *str)
	{
	Type *src;
	for(src=str;*src!=0;++src) { } // Find End

	for(src=src-1; src>=str ;--src)
		{
		if(MStdIsSpace(*src)==false)
			{ break; }

		*src=0;
		}

	return true;	
	};


////////////////////////////////////////////////////////
// Trim Spaces from the beginning of a string
template<typename Type>
bool MStdStrTrimLeft(Type *str)
	{
	Type *src;
	Type *dst;
	for(src=str;*src!=0;++src)
		{
		if(MStdIsSpace(*src)==false)
			{ break; }
		}

	// Copy String
	for(dst=str; ;++dst)
		{
		if((*dst=*src)==0)
			{ break; }

		src=src+1;
		}

	return true;	
	};


/////////////////////////////////////////////////////////
// Clean out any non printable characters
template <typename Type>
bool MStdStrClean(Type *buffer)
	{
	Type *src;
	Type *dst;
	for(src=dst=buffer; ;++src )
		{
		if(*src==0)
			{
			*dst = 0;
			return true;
			}

		if(MStdIsPrintable(*src)==true)
			{
			*dst=*src;
			dst = dst + 1;
			continue;
			}
		}

	return true;
	};


////////////////////////////////////////////////////////////
template <typename DataType>
class MStdUniquePtr
	{
	DataType *mObjectPtr;

	/////////////////////////////////////
	public:
	MStdUniquePtr(void)
		{
		mObjectPtr=0;
		}


	/////////////////////////////////////
	MStdUniquePtr(DataType *newobject)
		{
		mObjectPtr=newobject;
		}

	/////////////////////////////////////
	~MStdUniquePtr(void)
		{
		Destroy();
		}

	/////////////////////////////////////
	bool Create(DataType *newobject)
		{
		Destroy();
		mObjectPtr=newobject;
		return true;
		}

	/////////////////////////////////////
	bool Destroy(void)
		{
		if(mObjectPtr!=0){  delete mObjectPtr;  }
		mObjectPtr=0;
		return true;
		}

	//////////////////////////////////////
	DataType *Get(void) const
		{
		return mObjectPtr;
		}

	//////////////////////////////////////
	DataType & operator *(void)
		{
		MStdAssert(mObjectPtr!=0);
		return *mObjectPtr;
		}

	/////////////////////////////////////
	DataType * operator->(void)
		{
		MStdAssert(mObjectPtr!=0);
		return mObjectPtr;
		}

	/////////////////////////////////////
	DataType *Release(void)
		{
		DataType *retpointer=mObjectPtr;
		mObjectPtr=0;
		return retpointer;
		}
	};


///////////////////////////////////////////////////////////
// Simple Value Type Array
template <typename DataType>
class MStdArray
	{
	DataType *mArray;
	int mArrayLength;

	//////////////////////////////////////////////////////
	public:
	MStdArray(void)
		{
		mArray=0;
		mArrayLength=0;
		}

	//////////////////////////////////////////////////////
	MStdArray(int length)
		{
		mArray = 0;
		mArrayLength = 0;

		if(Create(length)==false)
			{
			return;
			}
		}


	//////////////////////////////////////////////////////
	MStdArray(MStdArray &refobj,int newlength=0)
		{
		mArray = 0;
		mArrayLength = 0;

		if(Create(refobj,newlength)==false)
			{
			return;
			}
		}


	//////////////////////////////////////////////////////
	~MStdArray(void)
		{
		Destroy();
		}

	////////////////////////////////////////////////////////
	bool Create(int length)
		{
		Destroy();

		mArray=new(std::nothrow) DataType[length];
		if(mArray==0)
			{
			return false;
			}

		mArrayLength=length;
		return true;
		}


	////////////////////////////////////////////////////////
	bool Create(MStdArray &refobj,int newlength=0)
		{
		MStdAssert(refobj.mArrayLength>0);

		// If Just Copy Constructed from original, keep same size
		if(newlength<=0) { newlength=refobj.mArrayLength; }

		// Implemenation should not modify members when refobj is current object.
		DataType *newarray=new(std::nothrow) DataType[newlength];
		if(newarray==0)
			{
			return false;
			}

		const int maxcopy=MStdGetMin(newlength,refobj.mArrayLength);
		for(int i=0;i<maxcopy;++i)
			{
			// Copy Construct new elements
			newarray[i] = refobj.mArray[i];
			}

		Destroy();
		mArray=newarray;
		mArrayLength=newlength;

		return true;
		}


	////////////////////////////////////////////////////////
	bool Destroy(void)
		{
		if(mArray!=0)
			{
			delete []mArray;
			}

		mArray=0;
		mArrayLength=0;
		return true;		
		}


	////////////////////////////////////////////////////
	operator DataType *(void)
		{
		return mArray;
		}

	///////////////////////////////////////////////////
	DataType *Get(void) const
		{
		return mArray;
		}

	///////////////////////////////////////////////////
	DataType & operator[](int index)
		{
		MStdAssert(index>=0 && index<mArrayLength);
		return mArray[index];
		}

	//////////////////////////////////////////////////
	int GetLength(void) const
		{
		return mArrayLength;
		}

	/////////////////////////////////////////////////
	bool Swap(MStdArray &refobj)
		{
		int tmplength=refobj.mArrayLength;
		refobj.mArrayLength=mArrayLength;
		mArrayLength=tmplength;

		DataType *tmparray=refobj.mArray;
		refobj.mArray=mArray;
		mArray=tmparray;

		return true;
		}

	//////////////////////////////////////////////////
	bool operator=(MStdArray &refobj)
		{
		return Create(refobj);
		}
	};


////////////////////////////////////////////////////////////////
// Quick String Allocator for char/widechar MStdArray
bool MStdStrCpy(char *outstr,int maxoutlen,const wchar_t *str);		// Convert wide asciiz string to asciiz
bool MStdStrCpy(wchar_t *outstr,int maxoutlen,const char *str);		// Convert asciiz string to wide unicode
bool MStdStrCpy(MStdArray<wchar_t> &strout,const char *str);		// Convert ascii strin to wide
bool MStdStrCpy(MStdArray<char> &strout,const wchar_t *str);		// Convert wide string to asciiz

////////////////////////////////////////////////
// Path Operations
bool MStdPathSetSlash(char *path);										// Set slashes to forward slash
bool MStdPathGetAbsolute(const char *path,MStdArray<char> &abspath);	// Get Absolute path
bool MStdDirGet(MStdArray<char> &path);									// Get Current working directory
bool MStdFileReadText(const char *filename,MStdArray<char> &data);		// Read from file into buffer


#endif // MStdLib_h

