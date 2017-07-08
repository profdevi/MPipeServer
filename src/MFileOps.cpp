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


//v1.49 copyright Comine.com 20170406R1314
#include "MStdLib.h"

//////////////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) )
#undef UNICODE
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

//////////////////////////////////////////////
#elif ( defined(MSTDLIB_API_LINUX) )
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#endif // MSTDLIB_API_WINDOWS

/////////////////////////////////////////////
#include "MBuffer.h"
#include "MString.h"
#include "MPathNameList.h"
#include "MFile.h"
#include "MFileOps.h"


////////////////////////////////////////////
#if ( defined(MSTDLIB_API_WINDOWS) ) 
	// Extra defines for VS6.0
	#ifndef INVALID_FILE_ATTRIBUTES
		#define INVALID_FILE_ATTRIBUTES		((DWORD)(-1))
	#endif // INVALID_FILE_ATTRIBUTES

	#ifndef FILE_ATTRIBUTE_DEVICE
		#define FILE_ATTRIBUTE_DEVICE               0x00000040
	#endif // FILE_ATTRIBUTE_DEVICE

//////////////////////////////////////////////////
#pragma comment(lib,"advapi32.lib")

#endif // MSTDLIB_API_WINDOWS

//******************************************************
//**  Module Elements
//******************************************************
static const char *GTempFileSuffix="__MFileOps__";

////////////////////////////////////////////////
#if (defined(MSTDLIB_API_WINDOWS) )
static int GTempFileNameID=0;
#endif


////////////////////////////////////////////////
static bool GConvertToCanonicalPath(char *path)
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

//******************************************************
//**  MFileOps class
//******************************************************
void MFileOps::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MFileOps::MFileOps(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MFileOps::MFileOps(bool autocreate)
	{
	if(autocreate==true && Create()==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MFileOps::~MFileOps(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MFileOps::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MFileOps::Destroy(void)
	{
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MFileOps::Move(const char *srcfile,const char *dstfile,bool erroronfail)
	{
	/////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(MoveFileA(srcfile,dstfile)==FALSE && erroronfail==true)
		{
		return false;
		}

	return true;

	////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(rename(srcfile,dstfile)!=0 && erroronfail==true)
		{
		return false;
		}

	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


/////////////////////////////////////////////////
bool MFileOps::MoveOnReboot(const char *srcfile,const char *dstfile)	// Move File after reboot
	{
	///////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(MoveFileExA(srcfile,dstfile,MOVEFILE_DELAY_UNTIL_REBOOT)==FALSE)
		{
		// Move File Might have just moved the file anyway. But force move.
		MoveFileA(srcfile,dstfile);

		// Check if Administrator
		MBuffer buffer(100);
		DWORD size=buffer.GetSize()-1;
		if(GetUserNameA(buffer.GetBuffer(),&size)==FALSE)
			{
			return false;
			}

		if(MStdStrICmp(buffer.GetBuffer(),"administrator")==0)
			{
			return false;
			}

		// Not an administrator, so just spit out success
		return true;
		}

	return true;

	///////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(rename(srcfile,dstfile)!=0)
		{
		return false;
		}

	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////
bool MFileOps::Delete(const char *filename,bool erroronfail)
	{
	/////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(DeleteFileA(filename)==FALSE && erroronfail==true)
		{
		return false;
		}

	return true;

	/////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(remove(filename)!=0 && erroronfail==true)
		{
		return false;
		}

	return true;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


////////////////////////////////////////////////
bool MFileOps::DeleteOnReboot(const char *filename)
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	/////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	// Problem: Newer Version of windows, MOVEFILE_DELAY_UNTIL_REBOOT only works as administrator.
	if(MoveFileExA(filename,NULL,MOVEFILE_DELAY_UNTIL_REBOOT)==FALSE)
		{
		// Attempt Delete 
		Delete(filename);

		// Check if Administrator
		MBuffer buffer(100);
		DWORD size=(DWORD)buffer.GetSize()-1;
		if(GetUserNameA(buffer.GetBuffer(),&size)==0)
			{
			return false;
			}

		if(MStdStrICmp(buffer.GetBuffer(),"administrator")==0)
			{
			return false;
			}

		// Not Administrator, so return success.
		return true;
		}

	return true;

	///////////////////////////////////////
	#else
	return false;

	#endif // MSTDLIB_API_WINDOWS
	}


////////////////////////////////////////////////
bool MFileOps::OverWriteDelete(const char *filename) // Overwrite and delete file
	{
	MStdFileHandle file;
	file=MStdFileOpen(filename,"r+b");
	if(file==NULL)
		{
		return  false;
		}

	// Basic Code only deals with 2GByte File <
	MStdFileSeek(file,0,2);
	long filelength=MStdFileTell(file);
	MStdFileSeek(file,0,0);
	
	MBuffer buf;
	if(buf.Create(512*4)==false)
		{
		MStdFileClose(file);
		return false;
		}

	int writtenbytes;
	for(int i=0;i<filelength;i+=writtenbytes)
		{
		for(int k=0;k<buf.GetSize();++k)
			{
			buf[k]=MStdRand()%256;
			}
	
		writtenbytes=MStdFileWrite(file,buf.GetBuffer(),1,buf.GetSize());
		if(writtenbytes<=0)
			{
			MStdFileClose(file);
			return false;
			}
		}
	
	MStdFileClose(file);

	if(MStdFileRemove(filename)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MFileOps::Copy(const char *srcfile,const char *dstfile,bool stopifexists,bool erroronfail)
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
	if(stopifexists==true && Exists(dstfile)==true)
		{
		return false;
		}

	MBuffer buffer;
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
		readamount=read(fdsrc,buffer.GetBuffer(),buffer.GetSize());
		if(readamount<=0)
			{
			break;
			}

		writeamount=write(fddst,buffer.GetBuffer(),readamount);
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


///////////////////////////////////////////////
bool MFileOps::Exists(const char *filename)						// Check if file exists
	{
	///////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD fileinfo=GetFileAttributesA(filename);
	if(fileinfo==INVALID_FILE_ATTRIBUTES) { return false; }
	if((fileinfo & FILE_ATTRIBUTE_DIRECTORY)>0) { return false; }
	if((fileinfo & FILE_ATTRIBUTE_DEVICE)>0) { return false; }

	return true;

	///////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(access(filename,F_OK)==0)
		{
		return true;
		}

	return false;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


//////////////////////////////////////////////
bool MFileOps::IsBinary(const char *filename)
	{
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


/////////////////////////////////////////////////////////////
bool MFileOps::IsOpenable(const char *filename)
	{
	MStdFileHandle file=MStdFileOpen(filename,"rb");
	if(file==NULL)
		{  return false;  }

	MStdFileClose(file);
	return true;
	}


/////////////////////////////////////////////////////////////
bool MFileOps::AreSame(const char *file1,const char *file2)		// Compare two files
	{
	MStdFileHandle fin1=MStdFileOpen(file1,"rb");
	if(fin1==NULL)
		{
		return false;
		}

	MStdFileHandle fin2=MStdFileOpen(file2,"rb");
	if(fin2==NULL)
		{
		MStdFileClose(fin1);
		return false;
		}

	for(;;)
		{
		unsigned char data1,data2;
		if(MStdFileRead(fin1,&data1,1,1)<1)
			{
			if(MStdFileRead(fin2,&data2,1,1)<1)
				{
				MStdFileClose(fin1);
				MStdFileClose(fin2);
				return true;
				}

			//=Still More data in the first file==> Not Same

			MStdFileClose(fin1);
			MStdFileClose(fin2);
			return false;
			}

		//=Got One Byte from file 1
	
		if(MStdFileRead(fin2,&data2,1,1)<1)
			{
			MStdFileClose(fin1);
			MStdFileClose(fin2);
			return false;
			}

		//=Got Two Bytes

		// Compare both bytes
		if(data1!=data2)
			{
			MStdFileClose(fin1);
			MStdFileClose(fin2);
			return false;
			}
		}
	}


//////////////////////////////////////////////
bool MFileOps::ConvertTextUnixToDos(const char *filename)		// Converts to Dos Text
	{
	if(IsBinary(filename)==true)
		{
		return false;
		}

	// Create tmp file name
	MBuffer tmpfilename;
	if(tmpfilename.Create(1000)==false)
		{
		return false;
		}

	MStdSPrintf(tmpfilename,tmpfilename.GetSize()
			,"%s%s",filename,GTempFileSuffix);

	// Open Input File
	MStdFileHandle finput=MStdFileOpen(filename,"rb");
	if(finput==NULL)
		{
		return false;
		}

	// Open Output File
	MStdFileHandle foutput=MStdFileOpen(tmpfilename.GetBuffer(),"wb");
	if(foutput==NULL)
		{
		MStdFileClose(finput);
		return false;
		}

	// Process rest of file
	enum {START,STATER} state=START;
	int i;
	for(i=0; ;++i)
		{
		unsigned char inbyte;
		if(MStdFileRead(finput,&inbyte,1,1)!=1) { break; }

		if(state==START)
			{
			if(inbyte=='\r')
				{
				state=STATER;
				continue;
				}

			if(inbyte=='\n')
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}
					

				tmpchar='\n';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}

				continue;
				}

			// Otherwise process character as normal
			if(MStdFileWrite(foutput,&inbyte,1,1)!=1)
				{
				MStdFileClose(finput);
				MStdFileClose(foutput);
				Delete(tmpfilename);
				return false;
				}

			continue;
			}			

		if(state==STATER)
			{
			if(inbyte=='\r')
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}					

				tmpchar='\n';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}
				
				state=STATER;
				continue;
				}

			if(inbyte=='\n')
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}					

				tmpchar='\n';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}

				state=START;
				continue;				
				}

			//=Any Other Character
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}					

				tmpchar='\n';
				if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}

				if(MStdFileWrite(foutput,&inbyte,1,1)!=1)
					{
					MStdFileClose(finput);
					MStdFileClose(foutput);
					Delete(tmpfilename);
					return false;
					}

				state=START;
				continue;				
				}
			}
		}

	// Check End
	if(state=='\r')
		{
		unsigned char tmpchar;
		tmpchar='\r';
		if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
			{
			MStdFileClose(finput);
			MStdFileClose(foutput);
			Delete(tmpfilename);
			return false;
			}					

		tmpchar='\n';
		if(MStdFileWrite(foutput,&tmpchar,1,1)!=1)
			{
			MStdFileClose(finput);
			MStdFileClose(foutput);
			Delete(tmpfilename);
			return false;
			}		
		}

	// Close Files
	MStdFileClose(finput);
	MStdFileClose(foutput);

	// Wish we could have a transaction here
	if(Delete(filename,true)==false)
		{
		Delete(tmpfilename);
		return false; // we did not delete the original file
		}

	if(Move(tmpfilename,filename)==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////
bool MFileOps::ConvertTextDosToUnix(const char *filename)		// Converts to Unix Text
	{
	if(IsBinary(filename)==true)
		{
		return false;
		}

	// Create tmp file name
	MBuffer tmpfilename;
	if(tmpfilename.Create(1000)==false)
		{
		return false;
		}

	MStdSPrintf(tmpfilename,tmpfilename.GetSize(),"%s%s",filename,GTempFileSuffix);

	// Open Input File
	MStdFileHandle finput=MStdFileOpen(filename,"rb");
	if(finput==NULL)
		{
		return false;
		}

	// Open Output File
	MStdFileHandle foutput=MStdFileOpen(tmpfilename.GetBuffer(),"wb");
	if(foutput==NULL)
		{
		MStdFileClose(finput);
		return false;
		}

	// Process rest of file
	int i;
	for(i=0; ;++i)
		{
		unsigned char inbyte;
		if(MStdFileRead(finput,&inbyte,1,1)!=1) { break; }

		// drop all '\r' characters
		if(inbyte=='\r') { continue; }

		// Continue copying
		if(MStdFileWrite(foutput,&inbyte,1,1)!=1)
			{
			MStdFileClose(finput);
			MStdFileClose(foutput);
			Delete(tmpfilename);
			return false;
			}
		}

	// Close Files
	MStdFileClose(finput);
	MStdFileClose(foutput);

	// Wish we could have a transaction here
	if(Delete(filename,true)==false)
		{
		Delete(tmpfilename);
		return false; // we did not delete the original file
		}

	if(Move(tmpfilename,filename)==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////
bool MFileOps::CleanToDosText(const char *filename)				// Cleans to Dos Text
	{
	// Process binary file to dos text
	// Create tmp file name
	MBuffer tmpfilename;
	if(tmpfilename.Create(1000)==false)
		{
		return false;
		}

	MStdSPrintf(tmpfilename,tmpfilename.GetSize(),"%s%s",filename,GTempFileSuffix);

	// Open Input File
	MFileInput finput;
	if(finput.Create(filename)==false)
		{
		return false;
		}

	// Open Output File
	MFileOutput foutput;
	if(foutput.Create(tmpfilename.GetBuffer())==false)
		{
		return false;
		}

	// Process rest of file
	enum {START,STATER} state=START;
	int i;
	for(i=0; ;++i)
		{
		unsigned char inbyte;
		if(finput.Read(inbyte)==false)
			{
			break;
			}

		if(inbyte>127 || (inbyte<32 && inbyte!='\t' && inbyte!='\n' && inbyte!='\r') )
			{
			continue;
			}
		
		if(state==START)
			{
			if(inbyte=='\r')
				{
				state=STATER;
				continue;
				}

			if(inbyte=='\n')
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}
					

				tmpchar='\n';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}

				continue;
				}

			// Otherwise process character as normal
			if(foutput.Write(inbyte)==false)
				{
				Delete(tmpfilename);
				return false;
				}

			continue;
			}			

		if(state==STATER)
			{
			if(inbyte=='\r')
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}					

				tmpchar='\n';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}
				
				state=STATER;
				continue;
				}

			if(inbyte=='\n')
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}					

				tmpchar='\n';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}

				state=START;
				continue;				
				}

			//=Any Other Character
				{
				unsigned char tmpchar;
				tmpchar='\r';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}					

				tmpchar='\n';
				if(foutput.Write(tmpchar)==false)
					{
					Delete(tmpfilename);
					return false;
					}

				if(foutput.Write(inbyte)==false)
					{
					Delete(tmpfilename);
					return false;
					}

				state=START;
				continue;				
				}
			}
		}

	// Check End
	if(state=='\r')
		{
		unsigned char tmpchar;
		tmpchar='\r';
		if(foutput.Write(tmpchar)==false)
			{
			Delete(tmpfilename);
			return false;
			}					

		tmpchar='\n';
		if(foutput.Write(tmpchar)==false)
			{
			Delete(tmpfilename);
			return false;
			}		
		}

	finput.Destroy();
	foutput.Destroy();

	// Wish we could have a transaction here! 

	if(Delete(filename,true)==false)
		{
		Delete(tmpfilename);
		return false; // we did not delete the original file
		}

	if(Move(tmpfilename,filename)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MFileOps::IsUnixTextFile(const char *filename)		// Check if unix text file
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	if(IsBinary(filename)==true) { return false; }

	// Start reading file
	MStdFileHandle file=MStdFileOpen(filename,"rb");
	if(file==false)
		{
		return false;
		}

	unsigned char ch;
	while(MStdFileRead(file,&ch,1,1)==1)
		{		
		if(ch=='\r')
			{
			MStdFileClose(file);
			return false;
			}
		}

	MStdFileClose(file);
	return true;
	}


////////////////////////////////////////////////////////////
bool MFileOps::IsDosTextFile(const char *filename)					// Check of dos text file
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	// Check if file is binary
	if(IsBinary(filename)==true) { return false; }

	// Start reading file
	MStdFileHandle file=MStdFileOpen(filename,"rb");
	if(file==false)
		{
		return false;
		}

	unsigned char ch;
	bool boolret=false;
	while(MStdFileRead(file,&ch,1,1)==1)
		{
		if(boolret==false)
			{
			if(ch=='\r') {  boolret=true; continue; }
			if(ch=='\n')
				{
				MStdFileClose(file);
				return false;
				}
			}
		else	//= boolret==true
			{
			if(ch=='\n')
				{
				boolret=false;
				continue;
				}

			MStdFileClose(file);
			return false;
			}
		}

	//Check if last character is '\r'
	if(boolret==true)
		{
		MStdFileClose(file);
		return false;
		}
	
	MStdFileClose(file);
	return true;
	}


/////////////////////////////////////////////////////////////
bool MFileOps::Append(const char *filename,const char *extrafile)
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	if(extrafile==NULL || *extrafile==0)
		{
		return false;
		}

	// Open tmp file for append
	MStdFileHandle filedst=MStdFileOpen(filename,"ab");
	if(filedst==NULL)
		{
		return false;
		}

	MStdFileHandle filesrc=MStdFileOpen(extrafile,"rb");
	if(filesrc==NULL)
		{
		MStdFileClose(filedst); return false;
		}

	MBuffer  buf;
	if(buf.Create(2048)==false)
		{
		MStdFileClose(filedst);  MStdFileClose(filesrc);
		return false;;		
		}

	for(;;)
		{
		// Read as many bytes as buffer can handle
		int len=MStdFileRead(filesrc,buf.GetBuffer(),1,buf.GetSize());
		if(len<=0) { break; }

		int writelen=MStdFileWrite(filedst,buf.GetBuffer(),1,len);
		if(writelen<=0)
			{
			//=Unabel to write data to files
			MStdFileClose(filedst);  MStdFileClose(filesrc);
			return false;;
			}
		}

	MStdFileClose(filesrc);  MStdFileClose(filedst);

	return true;
	}


///////////////////////////////////////////////////////////
bool MFileOps::MakeNewFile(const char *filename)			// Create fresh blank file
	{
	// Open file for writing
	MStdFileHandle file=MStdFileOpen(filename,"wb");
	if(file==NULL)
		{
		return false;
		}

	MStdFileClose(file);
	return true;
	}


///////////////////////////////////////////////////////////
bool MFileOps::IsDirectory(const char *filename)		// = true if filename is dir
	{
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if((data & FILE_ATTRIBUTE_DIRECTORY)!=0) {  return true; }

	return false;

	//////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	struct stat st;
	if(stat(filename,&st)<0)
		{
		return false;
		}

	if((st.st_mode & S_IFDIR)>0)
		{
		return true;
		}

	return false;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


//////////////////////////////////////////////////////////
bool MFileOps::IsReadOnly(const char *filename)					// =true if filename
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	/////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if((data & FILE_ATTRIBUTE_READONLY)!=0)
		{
		return true;
		}

	return false;

	///////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	if(access(filename,W_OK)==0)
		{
		return false;
		}

	return true;

	#endif
	
	return false;
	}


//////////////////////////////////////////////////////////
bool MFileOps::SetReadOnly(const char *filename,bool val)			// Set File name
	{
	/////////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	// Get File Attributes
	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if(val==true)
		{  data |= FILE_ATTRIBUTE_READONLY;  }
	else
		{  data &= (~FILE_ATTRIBUTE_READONLY);  }

	if(SetFileAttributesA(filename,data)==FALSE)
		{
		return false;
		}
	
	return true;

	/////////////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	struct stat st;
	if(stat(filename,&st)<0)
		{
		return false;
		}


	mode_t newmode;
	newmode=st.st_mode;
	
	if(val==true)
		{
		newmode = ( newmode & (~S_IRWXU) );
		}
	else
		{
		newmode = ( newmode | S_IRWXU );
		}

	
	if(chmod(filename,newmode)<0)
		{
		return false;
		}


	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


//////////////////////////////////////////////////////////
bool MFileOps::IsSystemFile(const char *filename)					// =true if filename
	{
	///////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if((data & FILE_ATTRIBUTE_SYSTEM)!=0) {  return true; }

	return false;

	///////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


//////////////////////////////////////////////////////////
bool MFileOps::SetSystemFile(const char *filename,bool val)			// Set File name
	{
	/////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	// Get File Attributes
	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if(val==true)
		{  data |= FILE_ATTRIBUTE_SYSTEM;  }
	else
		{  data &= (~FILE_ATTRIBUTE_SYSTEM);  }

	if(SetFileAttributesA(filename,data)==FALSE)
		{
		return false;
		}
	
	return true;

	/////////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


//////////////////////////////////////////////////////////
bool MFileOps::IsHiddenFile(const char *filename)					// =true if filename
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	//////////////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if((data & FILE_ATTRIBUTE_HIDDEN)!=0) {  return true; }

	return false;

	#elif (defined(MSTDLIB_API_LINUX) )
	int endpos;
	for(endpos=0;filename[endpos]!=0;++endpos)
		{
		// Do Nothing here
		}

	// Shift 1 pos back
	endpos=endpos - 1;

	for( ; ; )
		{
		if(filename[endpos]=='/')
			{
			return false;
			}

		if(filename[endpos]=='.')
			{
			if(endpos==0)
				{
				return true;
				}

			if(filename[endpos-1]=='/')
				{
				return true;
				}
			}
		}

	return false;

	#endif // MSTDLIB_API_WINDOWS

	return false;	
	}


//////////////////////////////////////////////////////////
bool MFileOps::SetHiddenFile(const char *filename,bool val)			// Set File name
	{
	///////////////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	// Get File Attributes
	DWORD data=::GetFileAttributesA(filename);
	if(data==INVALID_FILE_ATTRIBUTES)
		{
		return false;
		}

	if(val==true)
		{  data |= FILE_ATTRIBUTE_HIDDEN;  }
	else
		{  data &= (~FILE_ATTRIBUTE_HIDDEN);  }

	if(SetFileAttributesA(filename,data)==FALSE)
		{
		return false;
		}
	
	return true;

	///////////////////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	return false;	

	#endif // MSTDLIB_API_WINDOWS

	return false;		
	}


//////////////////////////////////////////////////////////
bool MFileOps::WriteTextFile(const char *filename,const char *data)		// Create fresh file
	{
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

	MStdFileClose(file);  return true;
	}


///////////////////////////////////////////////////////////////////
bool MFileOps::WriteBinaryFile(const char *filename,const char *data,int length)
	{
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

	if(MStdFileWrite(file,data,1,length)!=length)
		{
		MStdFileClose(file);
		return false;
		}

	MStdFileClose(file);
	return true;
	}


////////////////////////////////////////////////////////////////////
bool MFileOps::AppendTextFile(const char *filename,const char *extra)
	{
	if(filename==NULL || *filename==0 || extra==NULL )
		{
		return false;
		}

	// Open the file for writing
	MStdFileHandle file=MStdFileOpen(filename,"ab");
	if(file==NULL)
		{
		return false;
		}

	int len=MStdStrLen(extra);
	if(MStdFileWrite(file,extra,1,len)!=len)
		{
		MStdFileClose(file);  return false;
		}

	MStdFileClose(file);
	return true;
	}


///////////////////////////////////////////////////////////////////
bool MFileOps::ReadTextFile(const char *filename,char *buffer,int &size)
	{
	if(filename==NULL || *filename==0 || buffer==NULL || size<=1)
		{
		size=0; return false;
		}

	// Check if file is binary
	if(Exists(filename)==false || IsBinary(filename)==true)
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


//////////////////////////////////////////////////////////
bool MFileOps::ReadTextFile(const char *filename,MBuffer &buffer)
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	// Check if file exists
	if(Exists(filename)==false)
		{
		return false;
		}

	// Check if file is binary
	if(IsBinary(filename)==true)
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
	if(buffer.Create(filesize+1)==false)
		{
		MStdFileClose(file);  return false;
		}

	int len=MStdFileRead(file,buffer.GetBuffer(),1,filesize);
	if(len!=filesize)
		{
		buffer.Destroy();  MStdFileClose(file);
		return false;
		}

	MStdFileClose(file);
	buffer.GetBuffer()[filesize]=0;
	return true;
	}
	

//////////////////////////////////////////////////////////
bool MFileOps::ReadBinaryFile(const char *filename,MBuffer &buffer)	// Read from file into buffer
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	// Check if file exists
	if(Exists(filename)==false)
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
	if(buffer.Create(filesize)==false)
		{
		MStdFileClose(file);
		return false;
		}

	int len=MStdFileRead(file,buffer.GetBuffer(),1,filesize);
	if(len!=filesize)
		{
		buffer.Destroy();
		MStdFileClose(file);
		return false;
		}

	MStdFileClose(file);
	return true;
	}


//////////////////////////////////////////////////////////
bool MFileOps::GetAbsolutePath(const char *filename
		,MString &absolutepath)
	{
	/////////////////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	MBuffer buffer;
	if(buffer.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	if(filename==NULL || *filename==0)
		{
		return false;
		}

	char *filenameonly=NULL;
	DWORD length=GetFullPathNameA(filename,buffer.GetSize()
			,buffer.GetBuffer(),&filenameonly);
	if(length==0)
		{
		return false;
		}

	if(length>=(DWORD)buffer.GetSize())
		{
		return false;
		}

	// Convert to Canonical Path with \ converted to /
	GConvertToCanonicalPath(buffer.GetBuffer() );
	
	if(absolutepath.Create(buffer.GetBuffer())==false)
		{
		return false;
		}

	return true;

	/////////////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	MBuffer buffer;
	if(buffer.Create(PATH_MAX)==false)
		{
		return false;
		}

	if(Exists(filename)==true)
		{
		if(realpath(filename,buffer.GetBuffer())==NULL)
			{
			return false;
			}
		}
	else if(*filename=='/')
		{
		//File is already an absolute path
		buffer.SetString(filename);
		}
	else
		{
		// Build up path using cwd
		char *bufptr=buffer.GetBuffer();
		getcwd(bufptr,buffer.GetSize() );

		int length=MStdStrLen(bufptr);
		if(length<=0)
			{
			return false;
			}

		if(bufptr[length-1]!='/')
			{
			buffer.StringAppend("/");
			}

		buffer.StringAppend(filename);
		}
	

	if(absolutepath.Create(buffer.GetBuffer())==false)
		{
		return false;
		}

	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}


/////////////////////////////////////////////////////
bool MFileOps::GetTemperoryFileName(MString &filename)			// Get Full path Temp File
	{
	#if ( defined(MSTDLIB_API_WINDOWS) )
	// Get Temp Directory
	MBuffer tmpdir;
	if(tmpdir.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	DWORD length=::GetTempPathA(tmpdir.GetSize(),tmpdir.GetBuffer());
	if(length<1)
		{
		return false;
		}

	//Get Temp File Name
	MBuffer buffer;
	if(buffer.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	// Bump up Temp File Name ID
	GTempFileNameID = GTempFileNameID + 1;

	if(::GetTempFileNameA(tmpdir.GetBuffer(),"_TMP_"
				,GTempFileNameID,buffer.GetBuffer())==0)
		{
		return false;
		}

	// Convert to Canonical Path
	GConvertToCanonicalPath(buffer.GetBuffer() );
	
	if(filename.Create(buffer.GetBuffer())==false)
		{
		return false;
		}

	
	return true;

	////////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	MBuffer buffer;
	if(buffer.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	for(;;)
		{
		MStdSPrintf(buffer.GetBuffer(),buffer.GetSize()-1,
			"/tmp/_MFileOps_Tmp_.%d",MStdRand() );

		if(Exists(buffer.GetBuffer())==false) { break; }
		}

	if(filename.Create(buffer.GetBuffer())==false)
		{
		return false;
		}
	
	return true;
	#endif // MSTDLIB_API_WINDOWS

	return false;
	}



//////////////////////////////////////////////////////////////
bool MFileOps::WriteToExistingFile(const char *filename
		,const char *data,int length,int offset)						// Write to existing binary file
	{
	if(filename==NULL || *filename==0)
		{
		return false;
		}

	// Check if file exists
	if(Exists(filename)==false)
		{
		return false;
		}

	// Open the file for writing
	MStdFileHandle file=MStdFileOpen(filename,"r+b");
	if(file==NULL)
		{
		return false;
		}

	// Seek to position
	if(MStdFileSeek(file,offset,0)==false)
		{
		MStdFileClose(file);
		return false;
		}

	if(MStdFileWrite(file,data,1,length)!=length)
		{
		MStdFileClose(file);
		return false;
		}

	MStdFileClose(file);
	return true;
	}


/////////////////////////////////////////////////////////////////////
bool MFileOps::GetTempFileNameInDir(const char *directory,MString &filename)
	{
	// Find a tmp file in given directory
	// Build up a random name
	MPathNameList pathname;
	if(pathname.Create(directory)==false)
		{
		return false;
		}
	
	for(;;)
		{
		static const char charset[]="abcdefghijklmnopqrstuvwxyz0123456789";
		char tmpname[30];
		int k;
		for(k=0;k<10;++k)
			{
			const int tmpindex=MStdRand(36);
			tmpname[k] = charset[tmpindex];
			}

		tmpname[10]=0;
		MStdStrCat(tmpname,sizeof(tmpname)-1,".tmp");

		// Constuct Path to filename
		if(pathname.Push(tmpname)==false)
			{
			return false;
			}

		// Check if file exists
		MBuffer fullpath;
		if(pathname.GetFullPath(fullpath)==false)
			{
			return false;
			}

		if(Exists(fullpath.GetBuffer())==false)
			{
			if(filename.Create(fullpath.GetBuffer())==false)
				{
				return false;
				}

			return true;
			}

		// Pop the last name
		if(pathname.Pop()==false)
			{
			return false;
			}
		}
	}


//////////////////////////////////////////////////////////////
bool MFileOps::GetCanonicalName(const char *path,MString &canonname)
	{
	MBuffer buffer;
	if(buffer.Create(MStdPathMaxSize)==false)
		{
		return false;
		}

	if(buffer.SetString(path)==false)
		{
		return false;
		}

	// Convert to Canonical Path
	GConvertToCanonicalPath(buffer.GetBuffer() );

	if(canonname.Create(buffer.GetBuffer())==false)
		{
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////////////////
bool MFileOps::PrintFile(const char *filename)
	{
	MFileInput fin;
	if(fin.Create(filename)==false)
		{
		return false;
		}

	char ch;
	while(fin.Read(ch)==true)
		{  MStdPrintf("%c",ch);  }
	
	fin.Destroy();
	return true;
	}


/////////////////////////////////////////////////////////////
int MFileOps::GetSize(const char *filename)
	{
	MFileInput fin;
	if(fin.Create(filename)==false)
		{
		return 0;
		}

	if(fin.Seek(0,2)==false)
		{
		return 0;
		}

	int filesize=fin.Tell();
	fin.Destroy();

	return filesize;
	}


/////////////////////////////////////////////////////////////
bool MFileOps::SetSize(const char *filename,int newsize)
	{
	int size=GetSize(filename);
	if(size<0)
		{
		return false;
		}

	if(newsize==size)
		{ return true; }

	if(newsize>size)
		{
		MFileOutput fileout;
		if(fileout.Create(filename,true)==false)
			{
			return false;
			}
		
		char bf=0;
		int length;
		for(length=newsize-size;length>0;--length)
			{
			if(fileout.Write(&bf,1)==false)
				{
				return false;
				}
			}

		fileout.Destroy();
		
		return true;
		}

	//=We have smaller file size then expected
	
	MString tmpfile;
	if(GetTemperoryFileName(tmpfile)==false)
		{
		return false;
		}

	MFileInput fin;
	if(fin.Create(filename)==false)
		{
		return false;
		}

	MFileOutput fout;
	if(fout.Create(tmpfile.Get())==false)
		{
		return false;
		}

	int i;
	for(i=0;i<newsize;++i)
		{
		char buf;
		if(fin.Read((void *)&buf,1)==false)
			{
			fout.Destroy();
			Delete(tmpfile.Get());
			return false;
			}

		if(fout.Write(&buf,1)==false)
			{
			fout.Destroy();
			Delete(tmpfile.Get());
			return false;
			}
		}
	
	fin.Destroy();
	fout.Destroy();

	// We now copy tmp file back to source
	if(Copy(tmpfile.Get(),filename)==false)
		{
		// Delete(tmpfile.Get() );
		return false;
		}

	Delete(tmpfile.Get());
	return true;
	}


//////////////////////////////////////////////////////
int MFileOps::GetLineCount(const char *filename)
	{
	MFileInput fileinput;
	if(fileinput.Create(filename)==false)
		{
		return false;
		}

	// Finite state machine code
	enum { SSTART, SCHAR } state;
	state=SSTART;

	char ch;
	int linecount=0;
	while(fileinput.Read(ch)==true)
		{
		if(state==SSTART)
			{
			if(ch=='\n')
				{
				linecount = linecount + 1;
				state=SSTART;
				continue;
				}

			linecount = linecount + 1;
			state=SCHAR;
			continue;
			}

		// State SCHAR
		if(ch=='\n')
			{
			state=SSTART;
			}
		}
	
	fileinput.Destroy();
	return linecount;
	}


//////////////////////////////////////////////////////////
bool MFileOps::GetAppPath(MString &apppathname)
	{
	MBuffer buffer;
	if(buffer.Create(1000)==false)
		{
		return false;
		}

	int count;

	/////////////////////////////////////////
	#if ( defined(MSTDLIB_API_WINDOWS) )
	count=::GetModuleFileName(NULL,buffer.GetBuffer(),buffer.GetSize()-1);
	if(count==0)
		{
		return false;
		}

	// Convert to Canonical Path
	GConvertToCanonicalPath(buffer.GetBuffer() );

	////////////////////////////////////////
	#elif (defined(MSTDLIB_API_LINUX) )
	count=readlink("/proc/self/exe",buffer.GetBuffer(),buffer.GetSize()-1);
	if(count<=0 || count>=buffer.GetSize() )
		{
		return false;
		}

	buffer[count]=0;

	#endif // MSTDLIB_API_WINDOWS
	
	if(apppathname.Create(buffer.GetBuffer() )==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////////////
bool MFileOps::GetAppConfigFile(MString &configfile)
	{
	MString exepath;
	if(GetAppPath(exepath)==false)
		{
		return false;
		}

	MBuffer buffer;
	if(buffer.Create(1000)==false)
		{
		return false;
		}

	buffer.SetString(exepath.Get() );

	if(MStdStrIEnds(buffer.GetBuffer(),".exe")==true)
		{
		const int length=buffer.GetStringLength();
		buffer[length-4]=0;
		}

	buffer.StringAppend(".ini");

	if(configfile.Create(buffer.GetBuffer() )==false)
		{
		return false;
		}

	return true;
	}


