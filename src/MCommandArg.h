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


//v2.3 copyright Comine.com 20170412W1700
#ifndef MCommandArg_h
#define MCommandArg_h

///////////////////////////////////////////////////////
class MCommandArg
	{
	int mArg;											// Total Number of argum
	char **mArgv;										// Argument List
	char *mBlock;										// Block of memory for all args
	int mStartIndex;									// starting index
	bool mUnitTestOutput;								// unit test output

	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MCommandArg(void);
	MCommandArg(int arg,const char *argv[]);	// AutoCreate
	MCommandArg(int arg,char **argv);
	MCommandArg(const char *arguments);
	~MCommandArg(void);
	bool Create(int arg,const char *argv[]);
	bool Create(int arg,char **argv);
	bool Create(const char *arguments);
	bool Destroy(void);
	bool CheckArgHelp(void);
	bool CheckRemoveHelp(void);							// Check If Help Is Called
	bool RemoveArg(int index);
	int GetArgCount(void);
	const char *GetArg(int index);
	bool IsArgument(const char *argvalue,bool casesensitive=false);
	bool RemoveArg(const char *argvalue,bool casesensitive=false);
	bool CheckRemoveArg(const char *argvalue,bool casesense=false);
	bool GetNameValue(const char *name,const char *&value
			,int &index,bool casesense=false);//name="-size="
	bool Show(void);									// Show all the arguments
	};


#endif // MCommandArg_h


