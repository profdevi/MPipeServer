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


//v1.1 copyright Comine.com 20170708S1243
#include <Windows.h>
#include <thread>
#include "MStdLib.h"
#include "MWinAccessControlList.h"
#include "MWinSecurityDescriptor.h"
#include "MWinNamedPipe.h"


// Coding Reference : https://stackoverflow.com/questions/26561604/create-named-pipe-c-windows

//******************************************************
//**  MWinNamedPipeServer class
//******************************************************
void MWinNamedPipeServer::ClearObject(void)
	{
	mhPipe=INVALID_HANDLE_VALUE;
	mClientConnected=false;
	}


////////////////////////////////////////////////
MWinNamedPipeServer::MWinNamedPipeServer(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinNamedPipeServer::~MWinNamedPipeServer(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinNamedPipeServer::Create(const char *pipename,DWORD openmode,DWORD pipemode
		,int maxinstances,int inbuffersize,int outbuffersize,int timeout,bool showstarterror)
	{
	Destroy();

	MWinAccessControlList dacl(10);
	dacl.AddAccessAllow("Everyone",GENERIC_READ|GENERIC_WRITE);

	MWinSecurityDescriptor secdes;
	if(secdes.Create()==false || secdes.SetDACL(dacl)==false)
		{
		Destroy();
		return false;
		}

	SECURITY_ATTRIBUTES secattr;
	MStdMemZero(&secattr,sizeof(secattr));
	secattr.bInheritHandle=FALSE;
	secattr.lpSecurityDescriptor=secdes.GetPSD();
	secattr.nLength=sizeof(secattr);

	mhPipe=CreateNamedPipeA(pipename,openmode,pipemode,(DWORD)maxinstances,(DWORD)inbuffersize
			,(DWORD)outbuffersize,(DWORD)timeout,&secattr);
	if(mhPipe==INVALID_HANDLE_VALUE)
		{
		if(showstarterror==true) {  MStdPrintf("**Unable to start up server\n");  }
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinNamedPipeServer::Destroy(void)
	{
	if(mhPipe!=INVALID_HANDLE_VALUE)
		{
		if(mClientConnected==true)
			{
			DisconnectNamedPipe(mhPipe);
			mClientConnected=false;
			}

		CloseHandle(mhPipe);
		mhPipe=INVALID_HANDLE_VALUE;
		}
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MWinNamedPipeServer::Connect(void)
	{
	if(mClientConnected==true)
		{
		DisconnectNamedPipe(mhPipe);
		}

	if(ConnectNamedPipe(mhPipe,NULL)==FALSE)
		{
		// It is possible that a client connects before the ConnectNamedPipe is invoked after CreateNamed Pipe.
		// Connections is still good! 
		//  Ref: https://msdn.microsoft.com/query/dev15.query?appId=Dev15IDEF1&l=EN-US&k=k(NAMEDPIPEAPI%2FConnectNamedPipe);k(ConnectNamedPipe);k(DevLang-C%2B%2B);k(TargetOS-Windows)&rd=true
		DWORD error=GetLastError();
		if(error==ERROR_PIPE_CONNECTED)
			{
			mClientConnected=true;
			return true;
			}

		mClientConnected=false;
		return false;
		}

	mClientConnected=true;
	return true;
	}


/////////////////////////////////////////////////
bool MWinNamedPipeServer::Disconnect(void)
	{
	mClientConnected=false;
	if(DisconnectNamedPipe(mhPipe)==FALSE)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
int MWinNamedPipeServer::Write(const char buf[],int buflen)
	{
	MStdAssert(buflen>0);

	DWORD written=0;
	if(WriteFile(mhPipe,buf,(DWORD)buflen,&written,NULL)==FALSE)
		{
		return -1;
		}

	return (int)written;
	}


///////////////////////////////////////////////////
int MWinNamedPipeServer::Read(char buf[], int buflen)
	{
	DWORD readcount=0;
	if(ReadFile(mhPipe,buf,(DWORD)buflen,&readcount,NULL)==FALSE)
		{
		return -1;
		}

	return (int)readcount;
	}



//******************************************************
//**  MWinNamedPipeClient class
//******************************************************
void MWinNamedPipeClient::ClearObject(void)
	{
	mhPipe=INVALID_HANDLE_VALUE;
	}


////////////////////////////////////////////////
MWinNamedPipeClient::MWinNamedPipeClient(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinNamedPipeClient::~MWinNamedPipeClient(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinNamedPipeClient::Create(const char *pipename,DWORD waittime,bool timeouterror)
	{
	Destroy();
	
	if (WaitNamedPipeA(pipename, waittime) == FALSE)
		{
		if(timeouterror==true)  {  MStdPrintf("**Unable to connect to Pipe Server\n");  }	
		Destroy();
		return false;
		}

	mhPipe=CreateFileA(pipename,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(mhPipe==INVALID_HANDLE_VALUE)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinNamedPipeClient::Destroy(void)
	{
	if(mhPipe!=INVALID_HANDLE_VALUE)
		{
		CloseHandle(mhPipe);
		mhPipe=INVALID_HANDLE_VALUE;
		}

	ClearObject();
	return true;
	}


////////////////////////////////////////////////
int MWinNamedPipeClient::Write(const char buf[],int buflen)
	{
	MStdAssert(buflen>0);

	DWORD written=0;
	if(WriteFile(mhPipe,buf,(DWORD)buflen,&written,NULL)==FALSE)
		{
		return -1;
		}

	return (int)written;
	}


///////////////////////////////////////////////////
int MWinNamedPipeClient::Read(char buf[], int buflen)
	{
	DWORD readcount=0;
	if(ReadFile(mhPipe,buf,(DWORD)buflen,&readcount,NULL)==FALSE)
		{
		return -1;
		}

	return (int)readcount;
	}


