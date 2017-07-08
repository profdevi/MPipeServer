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


//v1.0 copyright Comine.com 20170418T0841
#ifndef MWinSID_h
#define MWinSID_h

/////////////////////////////////////////////////
#include <Windows.h>
#include <sddl.h>
#include "MStdLib.h"
#include "MString.h"
#include "MBuffer.h"

/*
	Note: This implementation assumes that a SID is a user or group 
	account.  This is not true, as a SID can refer to other identities 
	such as machine name, Domains,...  Testing for this has not been
	done.
*/

//******************************************************
//**  MWinSID class
//******************************************************
class MWinSID
	{
	////////////////////////////////////////////////
	MString mSIDString;
	MBuffer mSIDBinary;
	MBuffer mAccountName;
	MBuffer mDomainnName;
	SID_NAME_USE mSIDType;
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MWinSID(void);
	~MWinSID(void);
	bool CreateFromComputerName(void);
	bool CreateFromSIDBinary(PSID psid,const char *systemname=0);
	bool CreateFromAccountName(const char *name,const char *systemname=0);
	bool CreateFromSIDString(const char *name,const char *systemname=0);
	bool Destroy(void);	
	const char *GetSIDString(void);
	PSID GetSIDBinary(void);
	int GetSIDBinaryLength(void);
	const char *GetAccountName(void);
	const char *GetDomain(void);
	SID_NAME_USE GetSIDType(void);
	const char *GetSIDTypeString(void);
	bool IsOk(void);
	bool Print(void);
	bool IsSame(MWinSID &othersid);
	};

#endif // MWinSID_h

