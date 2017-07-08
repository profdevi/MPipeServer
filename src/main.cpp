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


//v1.1 copyright Comine.com 20170708S1352
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MWinNamedPipe.h"
#include "MLicenseGPL.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="MPipeServer";	// Used in Help
static const char *GApplicationVersion="1.1";	// Used in Help

////////////////////////////////////////////////////
static void GDisplayHelp(void);

////////////////////////////////////////////////////
int main(int argn,const char *argv[])
	{
	MCommandArg args(argn,argv);

	///////////////////////////////////////////////
	if(args.GetArgCount()<2)
		{
		GDisplayHelp();
		return 0;
		}

	if(args.CheckRemoveArg("-gpl")==true)
		{
		MLicenseGPL license(true);
		license.Print();
		return 0;
		}

	if(args.CheckRemoveHelp()==true)
		{
		GDisplayHelp();
		return 0;
		}

	const char *pipename=args.GetArg(1);

	MWinNamedPipeServer server;
	if(server.Create(pipename)==false)
		{
		MStdPrintf("**Unable to start server for pipe %s\n",pipename);
		return 1;
		}

	MStdPrintf("Starting Pipe Server on %s\n",pipename);

	for(;;)
		{
		if(server.Connect()==false)
			{
			MStdPrintf("**Connection Failed\n");
			continue;
			}

		char buf[100];
		const int msglen=server.Read(buf,sizeof(buf)-2);
		if(msglen<0)
			{
			MStdPrintf("**Message receive failed\n");
			return false;
			}

		buf[msglen]=0;
		MStdPrintf("Message Received: %s\n",buf);

		server.Disconnect();
		}

	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MStdPrintf(	"\n"
				"   usage:  %s [-?|-gpl] <pipename>\n"
				"           v%s copyright Comine.com\n"
				"           -gpl will display the Gnu Public License\n"
				"\n"
				"   Program starts up a named pipe passed as an argument.  All messages sent\n"
				"   from the MPipeClient.exe will be displayed. Pipe names must follow the \n"
				"   format:    \\\\.\\pipe\\(Pipe Name)\n"								
				"\n"
				,GApplicationName,GApplicationVersion);
	}


