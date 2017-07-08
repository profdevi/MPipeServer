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


//v1.20 copyright Comine.com 20150813R1238

///////////////////////////////////////
#include "MStdLib.h"

//////////////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
#undef UNICODE
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

// v6.0 Visual Studio
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES		((DWORD)(-1))
#endif // INVALID_FILE_ATTRIBUTES

//////////////////////////////////////////////
#elif (defined(MSTDLIB_API_LINUX) )
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#endif // MSTDLIB_API_WINDOWS

///////////////////////////////////////////////
#include "MBuffer.h"
#include "MFileOps.h"
#include "MPathNameList.h"
#include "MBuffer.h"
#include "MDirOps.h"


//******************************************************
//**  Module Elements
//******************************************************
static const int GMaxDirLength=1000;

///////////////////////////////////////////////////////
#if ( defined(MSTDLIB_OS_WINDOWS) )
static bool GConvertToCanonicalPath(char *path)
	{
	MStdAssert(path!=NULL);
	char *p;
	for(p=path;*p!=0;++p)
		{
		if(*p=='\\') { *p='/'; }
		}

	return true;
	}
#endif //MSTDLIB_OS_WINDOWS

//******************************************************
//**  MDirOps class
//******************************************************
void MDirOps::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MDirOps::MDirOps(bool create)
	{
	ClearObject();
	if(create==true && Create()==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MDirOps::~MDirOps(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MDirOps::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MDirOps::Destroy(void)
	{
	ClearObject();
	return true;
	}


/////////////////////////////////////////////////
bool MDirOps::MakeDir(const char *dirname,bool erroronfail)
	{
	////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(CreateDirectoryA(dirname,NULL)==FALSE && erroronfail==true)
		{
		return false;
		}

	return true;
	
	////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(mkdir(dirname,0777)<0 && erroronfail==true)
		{
		return false;
		}

	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


/////////////////////////////////////////////////
bool MDirOps::MakeDirNested(const char *dirname)
	{
	//Get Absolute Path
	MFileOps fileops(true);
	MString absdirpath;
	if(fileops.GetAbsolutePath(dirname,absdirpath)==false)
		{
		return false;
		}

	MPathNameList pathlist;
	if(pathlist.Create(absdirpath.Get() )==false)
		{
		return false;
		}

	const int pathelementcount=pathlist.GetElementCount();

	int i;
	for(i=1;i<=pathelementcount;++i)
		{
		MPathNameList tmppathlist;
		if(tmppathlist.Create(pathlist.Get(0))==false)
			{
			return false;
			}

		int k;
		for(k=1;k<i;++k)
			{
			if(tmppathlist.Push(pathlist.Get(k) )==false)
				{
				return false;
				}
			}

		MBuffer fulldirpath;
		if(tmppathlist.GetFullPath(fulldirpath)==false)
			{
			return false;
			}
		
		// In the first run directory might fail
		if(Exists(fulldirpath.GetBuffer())==false) 
			{
			if(MakeDir(fulldirpath.GetBuffer() )==false)
				{
				return false;
				}
			}
		}
	
	return true;
	}


/////////////////////////////////////////////////
bool MDirOps::RemoveDir(const char *dirname,bool erroronfail)
	{
	////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(RemoveDirectoryA(dirname)==FALSE && erroronfail==true)
		{
		return false;
		}

	return true;	

	////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(rmdir(dirname)<0 && erroronfail==true)
		{
		return false;
		}

	return true;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


//////////////////////////////////////////////////
bool MDirOps::Exists(const char *dirname)
	{
	////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD flags=GetFileAttributesA(dirname);
	if(flags==INVALID_FILE_ATTRIBUTES) { return false; }
	if((flags&FILE_ATTRIBUTE_DIRECTORY)==0) { return false; }
	return true;

	////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	struct stat statinfo;
	if(stat(dirname,&statinfo)<0)
		{
		// Unable to get stat info
		return false;
		}

	if(S_ISDIR(statinfo.st_mode)>0)
		{
		return true;
		}

	return false;
	#endif // MSTDLIB_OS_WINDOWS

	return false;	
	}


/////////////////////////////////////////////////////
bool MDirOps::IsSystem(const char *dirname)
	{
	////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD flags=GetFileAttributesA(dirname);
	if(flags==INVALID_FILE_ATTRIBUTES) { return false; }
	if((flags&FILE_ATTRIBUTE_SYSTEM)==0) { return false; }
	
	return true;	

	////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////////
bool MDirOps::IsReadOnly(const char *dirname)
	{
	/////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD flags=GetFileAttributesA(dirname);
	if(flags==INVALID_FILE_ATTRIBUTES) { return false; }
	if((flags&FILE_ATTRIBUTE_READONLY)==0) { return false; }
	return true;		

	/////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(access(dirname,W_OK)==0)
		{
		return true;
		}

	return false;

	#endif //MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////////
bool MDirOps::IsHidden(const char *dirname)
	{
	///////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD flags=GetFileAttributesA(dirname);
	if(flags==INVALID_FILE_ATTRIBUTES) { return false; }
	if((flags&FILE_ATTRIBUTE_HIDDEN)==0) { return false; }
	return true;

	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////////
bool MDirOps::IsEncrypted(const char *dirname)
	{
	///////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD flags=GetFileAttributesA(dirname);
	if(flags==INVALID_FILE_ATTRIBUTES) { return false; }
	if((flags&FILE_ATTRIBUTE_ENCRYPTED)==0) { return false; }
	return true;

	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


/////////////////////////////////////////////////////
bool MDirOps::IsCompressed(const char *dirname)
	{
	/////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD flags=GetFileAttributesA(dirname);
	if(flags==INVALID_FILE_ATTRIBUTES) { return false; }
	if((flags&FILE_ATTRIBUTE_COMPRESSED)==0) { return false; }
	return true;

	/////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////
bool MDirOps::Move(const char *srcfile,const char *dstfile,bool erroronfail)
	{
	/////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(MoveFileA(srcfile,dstfile)==FALSE && erroronfail==true)
		{
		return false;
		}

	return true;

	/////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(rename(srcfile,dstfile)<0 && erroronfail==true)
		{
		return false;
		}

	return true;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


/////////////////////////////////////////////////
bool MDirOps::MoveOnReboot(const char *srcfile,const char *dstfile)	// Move File after reboot
	{
	/////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(MoveFileExA(srcfile,dstfile,MOVEFILE_DELAY_UNTIL_REBOOT)==FALSE)
		{
		return false;
		}

	return true;

	////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;
	#endif // MSTDLIB_OS_WINDOWS

	return false;	
	}


////////////////////////////////////////////////////
bool MDirOps::GetCurrentDir(MBuffer &dir)			// Return Current Directory
	{
	if(dir.Create(GMaxDirLength)==false)
		{
		return false;
		}

	/////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) )
	DWORD len=GetCurrentDirectoryA(dir.GetSize()
			,dir.GetBuffer() );
	if(len==0 || len>=(unsigned int)dir.GetSize() )
		{
		return false;
		}

	// Convert to Canonical form
	GConvertToCanonicalPath(dir.GetBuffer() );

	return true;

	/////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(getcwd(dir.GetBuffer(),dir.GetSize()-1)==NULL)
		{
		return false;
		}

	return true;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////////
bool MDirOps::GetCurrentDir(char drive,MBuffer &dir)
	{
	///////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(dir.Create(GMaxDirLength)==false)
		{
		return false;
		}

	char strdrive[4];
	strdrive[0]=drive;
	strdrive[1]=':';
	strdrive[2]=0;

	DWORD len=GetFullPathNameA(strdrive,dir.GetSize(),dir.GetBuffer(),NULL);
	if(len==0 || len>=(unsigned int)dir.GetSize() )
		{
		return false;
		}

	// Convert to Canonical Path
	GConvertToCanonicalPath(dir.GetBuffer() );

	return true;

	///////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


/////////////////////////////////////////////////////
bool MDirOps::GetTempDir(MBuffer &buf)						// Get Temp Directory 
	{
	if(buf.Create(GMaxDirLength)==false)
		{
		return false;
		}

	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD length=::GetTempPathA(buf.GetSize(),buf.GetBuffer());
	if(length<1)
		{
		return false;
		}

	// Convert to Canonical Path
	GConvertToCanonicalPath(buf.GetBuffer() );

	return true;

	#elif (defined(MSTDLIB_API_LINUX) )
	buf.SetString("/tmp");
	return true;

	#endif // MSTDLIB_API_WINDOWS

	return false;	
	}


///////////////////////////////////////////////////////
bool MDirOps::SetDir(const char *dir)
	{
	////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(SetCurrentDirectoryA(dir)==FALSE)
		{
		return false;
		}

	return true;

	/////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(chdir(dir)<0)
		{
		return false;
		}

	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


///////////////////////////////////////////////////////
bool MDirOps::IsReadable(const char *dir)
	{
	//////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	MFileOps fileops(true);
	MString fullfilepath;
	if(fileops.GetAbsolutePath(dir,fullfilepath)==false)
		{
		return false;
		}

	//**Build up all files
	MPathNameList pathnamelist;
	if(pathnamelist.Create(fullfilepath.Get() )==false)
		{
		return false;
		}

	if(pathnamelist.Push("*.*")==false)
		{
		return false;
		}

	MBuffer fullpath;
	if(pathnamelist.GetFullPath(fullpath)==false)
		{
		return false;
		}

	WIN32_FIND_DATA data;
	MStdMemSet(&data,0,sizeof(data) );

	HANDLE dirhandle=FindFirstFileA(fullpath.GetBuffer() ,&data);
	if(dirhandle==INVALID_HANDLE_VALUE)
		{  return false;  }

	FindClose(dirhandle);
	return true;

	//////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(access(dir,R_OK)==0)
		{
		return false;
		}

	return true;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


