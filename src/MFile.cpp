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


//v3.0 copyright Comine.com 20150810M0924
#include "MStdLib.h"

/////////////////////////////
#if ( defined(MSTDLIB_SPI_WINDOWS) )
#undef UNICODE
#include <windows.h>

#endif // MSTDLIB_API_WINDOWS

#include "MIReader.h"
#include "MIWriter.h"
#include "MFile.h"


//******************************************************
//**  MFileOutput class
//******************************************************
//////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
void MFileOutput::ClearObject(void)
	{
	mFile=INVALID_HANDLE_VALUE;
	}

////////////////////////////
#else
void MFileOutput::ClearObject(void)
	{
	mFile=NULL;
	}

#endif  // MSTDLIB_API_WINDOWS

////////////////////////////////////////////////
MFileOutput::MFileOutput(void)
	{
	ClearObject();
	}


////////////////////////////////////////////////
MFileOutput::MFileOutput(const char *filename,bool append)
	{
	ClearObject();
	if(Create(filename,append)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MFileOutput::~MFileOutput(void)
	{  Destroy();  }


///////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileOutput::Create(const char *filename,bool append)
	{
	Destroy();
	if(append==true)
		{
		mFile=CreateFileA(filename,FILE_APPEND_DATA,0,NULL,OPEN_ALWAYS
				,FILE_ATTRIBUTE_NORMAL,NULL);
		// Move to end of file
		SetFilePointer(mFile,0,0,FILE_END);
		}
	else
		{
		mFile=CreateFileA(filename,GENERIC_WRITE,0,NULL
				,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(mFile==INVALID_HANDLE_VALUE)
			{
			// File Does not exist,try opening normally
			mFile=CreateFileA(filename,GENERIC_WRITE,0,NULL
					,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			}
		}


	if(mFile==INVALID_HANDLE_VALUE)
		{
		Destroy();
		return false;
		}

	return true;
	}

////////////////////////////////////
#else	//MSTDLIB_API_WINDOWS
bool MFileOutput::Create(const char *filename,bool append)
	{
	Destroy();
	if(append==true)
		{
		mFile=(MStdFileHandle )MStdFileOpen(filename,"ab");
		}
	else
		{
		mFile=(MStdFileHandle )MStdFileOpen(filename,"wb");
		}

	if(mFile==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}

#endif  // MSTDLIB_API_WINDOWS


///////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileOutput::Destroy(void)
	{
	if(mFile!=INVALID_HANDLE_VALUE)
		{
		CloseHandle(mFile);
		mFile=INVALID_HANDLE_VALUE;
		}

	ClearObject();
	return true;
	}

//////////////////////////////
#else
bool MFileOutput::Destroy(void)
	{
	if(mFile!=NULL)
		{
		MStdFileClose((MStdFileHandle )mFile);
		mFile=NULL;
		}

	ClearObject();
	return true;
	}

#endif // MSTDLIB_API_WINDOWS


////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileOutput::OnWrite(const void *block,int size)
	{
	DWORD written=0;
	if(WriteFile(mFile,block,size,&written,NULL)==FALSE)
		{
		return false;
		}

	return true;	
	}

/////////////////////////////////////
#else
bool MFileOutput::OnWrite(const void *block,int size)
	{
	if(MStdFileWrite(mFile,block,size,1)!=1 )
		{
		return false;
		}

	return true;
	}

#endif // MSTDLIB_API_WINDOWS


///////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
int MFileOutput::Tell(void)
	{
	LONG distancehigh=0;
	DWORD pos=SetFilePointer(mFile,0,&distancehigh,FILE_CURRENT);
	if(pos==INVALID_SET_FILE_POINTER && GetLastError()!=ERROR_SUCCESS)
		{
		return -1;
		}

	if(distancehigh>0)
		{
		return -1;
		}

	return  (int)distancehigh;
	}

////////////////////////////
#else 
int MFileOutput::Tell(void)
	{
	return MStdFileTell(mFile);
	}

#endif // MSTDLIB_API_WINDOWS


//////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
HANDLE MFileOutput::GetHandle(void)
	{
	return mFile;
	}

//////////////////////////////////
#else
MStdFileHandle MFileOutput::GetHandle(void)
	{
	return mFile;
	}

#endif // MSTDLIB_API_WINDOWS


//******************************************************
//**  MFileInput class
//******************************************************
/////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
void MFileInput::ClearObject(void)
	{
	mFile=INVALID_HANDLE_VALUE;
	}

/////////////////////////////
#else
void MFileInput::ClearObject(void)
	{
	mFile=NULL;
	}
#endif // MSTDLIB_API_WINDOWS


//////////////////////////////////////////////////
MFileInput::MFileInput(void)
	{
	ClearObject();
	}


//////////////////////////////////////////////////
MFileInput::MFileInput(const char *filename)
	{
	ClearObject();
	if(Create(filename)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////
MFileInput::~MFileInput(void)
	{  Destroy();  }


///////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileInput::Create(const char *filename)
	{
	Destroy();
	mFile=CreateFileA(filename,GENERIC_READ,0,NULL,OPEN_EXISTING
			,FILE_ATTRIBUTE_NORMAL,NULL);
	if(mFile==INVALID_HANDLE_VALUE)
		{
		Destroy();
		return false;
		}

	return true;
	}


//////////////////////////////////////
#else
bool MFileInput::Create(const char *filename)
	{
	Destroy();
	mFile=(MStdFileHandle )MStdFileOpen(filename,"rb");
	if(mFile==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}
#endif // MSTDLIB_API_WINDOWS


////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileInput::Destroy(void)
	{
	if(mFile!=INVALID_HANDLE_VALUE)
		{
		CloseHandle(mFile);
		mFile=INVALID_HANDLE_VALUE;
		}

	ClearObject();
	return true;
	}

////////////////////////////////
#else 
bool MFileInput::Destroy(void)
	{
	if(mFile!=NULL)
		{
		MStdFileClose(mFile);
		mFile=NULL;
		}

	ClearObject();
	return true;
	}

#endif // MSTDLIB_API_WINDOWS



/////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileInput::OnRead(void *block,int size)
	{
	DWORD bytesread=0;
	if(ReadFile(mFile,block,size,&bytesread,NULL)==FALSE)
		{
		return false;
		}

	if(bytesread==0)
		{
		//=reached EOF
		return false;
		}

	return true;
	}

/////////////////////////////////////
#else 
bool MFileInput::OnRead(void *block,int size)
	{
	if(MStdFileRead(mFile,block,size,1)!=1 )
		{
		if(MStdFileEOF(mFile)==true) { return false; }

		return false;
		}

	return true;
	}

#endif // MSTDLIB_API_WINDOWS


//////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
int MFileInput::ReadToBuffer(void *buffer,int size)
	{
	DWORD bytesread=0;
	if(ReadFile(mFile,buffer,size,&bytesread,NULL)==FALSE)
		{
		return 0;
		}

	return (int)bytesread;
	}

///////////////////////////////
#else
int MFileInput::ReadToBuffer(void *buffer,int size)
	{
	int len;
	len=MStdFileRead(mFile,buffer,1,size);
	if(len<0)
		{
		return 0;
		}

	return len;
	}

#endif // MSTDLIB_API_WINDOWS


/////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
bool MFileInput::Seek(int offset,int relative) // relative=0(beginning) 1(current) 2(end)
	{
	// Change to Current position
	DWORD relpos;
	if(relative==0)
		{ relpos=FILE_BEGIN;  }
	else if(relative==2)
		{  relpos=FILE_END; }
	else
		{  relpos=FILE_CURRENT; }

	if(SetFilePointer(mFile,offset,NULL,relpos)==INVALID_SET_FILE_POINTER
			&& GetLastError()!=ERROR_SUCCESS)
		{
		return false;
		}

	return true;
	}

/////////////////////////////////
#else 
bool MFileInput::Seek(int offset,int relative) // relative=0(beginning) 1(current) 2(end)
	{
	// Change to Current position
	MStdAssert(relative>=0 && relative<=2);
	if(MStdFileSeek(mFile,offset,relative)==true)
		{
		//=Succesful Move
		return true;
		}

	return false;
	}

#endif // MSTDLIB_API_WINDOWS


////////////////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
int MFileInput::Tell(void)
	{
	LONG hipos=0;
	DWORD position=SetFilePointer(mFile,0,&hipos,FILE_CURRENT);
	if(position==INVALID_SET_FILE_POINTER && GetLastError()!=ERROR_SUCCESS)
		{
		return false;
		}

	if(hipos!=0)
		{
		return position;
		}

	return position;
	}

//////////////////////////////////
#else 
int MFileInput::Tell(void)
	{
	return MStdFileTell(mFile);
	}
#endif // MSTDLIB_API_WINDOWS


///////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
HANDLE MFileInput::GetHandle(void)
	{
	return mFile;
	}

/////////////////////////////
#else 
MStdFileHandle MFileInput::GetHandle(void)
	{
	return mFile;
	}

#endif // MSTDLIB_API_WINDOWS


