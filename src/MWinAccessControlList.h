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


//v1.3 copyright Comine.com 20170707F0051
#ifndef MWinAccessControlList_h
#define MWinAccessControlList_h

////////////////////////////////////////////////////
#include <windows.h>
#include "MStdLib.h"
#include "TVector.h"

//******************************************************
//**  MWinAccessControlList class
//******************************************************
class MWinAccessControlList
	{
	////////////////////////////////////////////////
	TVector<char> mMemoryBlock;
	PACL mpACL;
	
	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MWinAccessControlList(void);
	MWinAccessControlList(int maxacecount);
	~MWinAccessControlList(void);
	bool Create(int maxacecount=10);
	bool Destroy(void);
	// Permission GENERAIC_READ GENERIC_WRITE  GENERIC_EXECUTE and GENERIC_ALL
	bool AddAccessAllow(const char *accountname,DWORD permission);
	bool AddAccessDeny(const char *accountname,DWORD permission);
	PACL GetACL(void);
	};

#endif // MWinAccessControlList_h

