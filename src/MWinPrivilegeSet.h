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


//v1.0 copyright Comine.com 20170711T1226
#ifndef MWinPrivilegeSet_h
#define MWinPrivilegeSet_h

/////////////////////////////////////////////////
#include <Windows.h>
#include "MStdLib.h"
#include "TVector.h"
#include "MString.h"

//******************************************************
//**  MWinPrivilegeSet class
//******************************************************
class MWinPrivilegeSet
	{
	////////////////////////////////////////////////
	int mMaxPrivileges;
	int mCount;
	TVector<char> mPrivilegeData;
	TOKEN_PRIVILEGES *mpPrivilegeData;
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWinPrivilegeSet(void);
	~MWinPrivilegeSet(void);
	bool Create(int maxprivledges);
	bool CreateFromProcess(HANDLE haccesstoken);
	bool Destroy(void);
	TOKEN_PRIVILEGES *GetPrivileges(void);
	bool EnablePrivilege(const wchar_t *privledge,bool enable=true);		// enable==false to disable privledge
	bool Print(void);										// Print Privledges
	int GetCount(void) const;								// Get Count of priviledges set
	int GetMaxCount(void) const;							// Get Max capactity of privileges
	DWORD GetAttributes(int index) const;					// Get Attribute for privilege
	LUID GetLUID(int index) const;							// GET LUID for privilege
	bool GetLUIDName(int index,MString &luidname) const;	// Get Name of LUID
	};

#endif // MWinPrivilegeSet_h

