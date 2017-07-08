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
#ifndef MWinNamedPipe_h
#define MWinNamedPipe_h

/////////////////////////////////////////////////
#include <Windows.h>
#include "MStdLib.h"

//******************************************************
//**  MWinNamedPipeServer class
//******************************************************
class MWinNamedPipeServer
	{
	////////////////////////////////////////////////
	HANDLE mhPipe;
	bool mClientConnected;
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWinNamedPipeServer(void);
	~MWinNamedPipeServer(void);

	// Pipe names are of form "\\\\.\\pipe\\ThePipeName"
	bool Create(const char *pipename,DWORD openmode=PIPE_ACCESS_DUPLEX|FILE_FLAG_FIRST_PIPE_INSTANCE
			,DWORD pipemode=PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,int maxinstances=1
			,int inbuffersize=16*1024,int outbuffersize=16*1024,int timeout=NMPWAIT_USE_DEFAULT_WAIT
			,bool showstarterror=false);
	bool Destroy(void);
	bool Connect(void);							// Block till a client connects to pipe
	bool Disconnect(void);						// Close connection to client
	int Write(const char buf[],int buflen);		//=Amount Written from buffer to pipe
	int Read(char buf[], int buflen);			//=Amount Read to buffer from pipe
	};


//******************************************************
//**  MWinNamedPipeClient class
//******************************************************
class MWinNamedPipeClient
	{
	////////////////////////////////////////////////
	HANDLE mhPipe;
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWinNamedPipeClient(void);
	~MWinNamedPipeClient(void);
	bool Create(const char *pipename,DWORD waittime=NMPWAIT_WAIT_FOREVER,bool timeouterror=false);
	bool Destroy(void);
	int Write(const char buf[],int buflen);		//=Amount Written from buffer to pipe
	int Read(char buf[], int buflen);			//=Amount Read to buffer from pipe
	};


#endif // MWinNamedPipe_h

