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


//v2.0 copyright Comine.com 20150810M1158
#ifndef MCommandParser_h
#define MCommandParser_h

/////////////////////////////////////////////////////
#include "MString.h"
#include "MStringArray.h"

/////////////////////////////////////////////////////
class MCommandParser
	{
	/////////////////////////////////////////////////
	MString mSeperatorList;
	MStringArray mArray;

	/////////////////////////////////////////////////
	void ClearObject(void);
	bool IsSeperator(char ch);

	/////////////////////////////////////////////////
	public:
	MCommandParser(void);
	~MCommandParser(void);
	bool Create(const char *seperatorlist="\t\r\n =");
	bool Destroy(void);
	bool Clear(void);
	bool ParseString(const char *string);
	int GetCount(void);
	const char *GetString(int index);
	};


#endif // MCommandParser_h

