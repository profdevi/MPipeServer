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


//v1.6 copyright Comine.com 20170418T1028
#ifndef MWinAccessToken_h
#define MWinAccessToken_h

////////////////////////////////////////////////////
#include <windows.h>
#include "MStdLib.h"
#include "MString.h"

//******************************************************
//**  MWinAccessToken class
//******************************************************
class MWinAccessToken
	{
	bool mTakeOwnership;
	HANDLE mhAccessToken;

	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MWinAccessToken(void);
	~MWinAccessToken(void);
	bool Create(DWORD access=TOKEN_WRITE | TOKEN_QUERY);								// Get Current Process Handle
	bool Create(HANDLE haccesstoken,bool takeownship=false);
	bool Destroy(void);
	bool AddPrivilege(const wchar_t *privledge);	// Add a privledge
	bool DelPrivilege(const wchar_t *privledge);	// Once a priv is removed, it may not be added(some os)
	bool HasPrivilege(const wchar_t *privledge);	//=true if privledge is held
	bool RemoveAllPrivileges(void);					// Remove All Privledges from current token
	bool GetOwner(MString &owner,SID_NAME_USE &usertype);		// Get Owner
	bool Print(void);								// Print Info about current access Token Privledges
	};

#endif // MWinAccessToken_h
