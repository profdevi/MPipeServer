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
#include <Windows.h>
#include "MStdLib.h"
#include "MString.h"
#include "MBuffer.h"
#include "MWinSID.h"


////////////////////////////////////////////
#pragma comment(lib,"Advapi32.lib")

//******************************************************
//**  MWinSID class
//******************************************************
void MWinSID::ClearObject(void)
	{
	mSIDType=SID_NAME_USE::SidTypeUnknown;
	}


////////////////////////////////////////////////
MWinSID::MWinSID(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinSID::~MWinSID(void)
	{  Destroy();  }


/////////////////////////////////////////////////
bool MWinSID::CreateFromComputerName(void)
	{
	Destroy();

	MBuffer computername(1024);
	MStdGetMachineName(computername.GetBuffer(),computername.GetSize()-2);

	return CreateFromAccountName(computername.GetBuffer(),0);
	}


/////////////////////////////////////////////////
bool MWinSID::CreateFromSIDBinary(PSID psid,const char *systemname)
	{
	Destroy();

	// Get Normal User Name
	DWORD namesize=0;
	DWORD domainsize=0;
	BOOL bret=LookupAccountSidA(systemname,psid,NULL,&namesize,NULL,&domainsize,&mSIDType);
	if(bret==TRUE)
		{
		Destroy();
		return false;
		}

	if(mAccountName.Create(namesize+2)==false || mDomainnName.Create(domainsize+2)==false )
		{
		Destroy();
		return false;
		}

	bret=LookupAccountSidA(systemname,psid,mAccountName.GetBuffer(),&namesize
			,mDomainnName.GetBuffer(),&domainsize,&mSIDType);
	if(bret==FALSE)
		{
		Destroy();
		return false;
		}

	DWORD sidlength=GetLengthSid(psid);
	if(sidlength<=0)
		{
		Destroy();
		return false;
		}

	if(mSIDBinary.Create(sidlength)==false)
		{
		Destroy();
		return false;
		}

	MStdMemCpy(mSIDBinary.GetBuffer(),psid,sidlength);
	
	char *sidstr=0;
	if(ConvertSidToStringSidA(psid,&sidstr)==FALSE)
		{
		Destroy();
		return false;
		}

	if(mSIDString.Create(sidstr)==false)
		{
		LocalFree(sidstr);
		Destroy();
		return false;
		}

	LocalFree(sidstr);
	return true;
	}


////////////////////////////////////////////////
bool MWinSID::CreateFromAccountName(const char *name,const char *systemname)
	{
	Destroy();

	// Get Normal User Name
	DWORD sidsize=0;
	DWORD domnamesize=0;
	BOOL bret=LookupAccountNameA(systemname,name,NULL,&sidsize,NULL,&domnamesize,&mSIDType);
	if(bret==TRUE)
		{
		Destroy();
		return false;
		}

	// Allocate domain name size
	if(sidsize<=0 || mSIDBinary.Create(sidsize)==false || mDomainnName.Create(domnamesize+2)==false 
				|| mAccountName.Create(MStdStrLen(name)+2)==false)
		{
		Destroy();
		return false;
		}

	mAccountName.SetString(name);

	bret=LookupAccountNameA(systemname,name,(PSID)mSIDBinary.GetBuffer(),&sidsize,mDomainnName.GetBuffer(),&domnamesize,&mSIDType);
	if(bret==FALSE)
		{
		Destroy();
		return false;
		}

	// Convert SID Binary to string
	char *sidstring=0;
	bret=ConvertSidToStringSidA((PSID)mSIDBinary.GetBuffer(),&sidstring);
	if(bret==FALSE)
		{
		Destroy();
		return false;
		}

	if(mSIDString.Create(sidstring)==false)
		{
		LocalFree(sidstring);
		Destroy();
		return false;
		}

	LocalFree(sidstring);

	return true;
	}


////////////////////////////////////////////////
bool MWinSID::CreateFromSIDString(const char *sidstring,const char *systemname)
	{
	Destroy();

	
	// First Convert name to SID
	PSID psid;
	if(ConvertStringSidToSidA(sidstring,&psid)==FALSE)
		{
		Destroy();
		return false;
		}

	const int sidlength=GetLengthSid(psid);
	if(sidlength<=0)
		{
		LocalFree(psid);
		Destroy();
		return false;
		}

	if(mSIDBinary.Create(sidlength)==false)
		{
		LocalFree(psid);
		Destroy();
		return false;
		}

	// Copy Sid + release memory
	MStdMemCpy(mSIDBinary.GetBuffer(),psid,sidlength);
	LocalFree(psid);

	//** Get Info from Binary SID
	DWORD namelen=0;
	DWORD domainnamesize=0;
	BOOL bret=LookupAccountSidA(systemname,(PSID)mSIDBinary.GetBuffer(),NULL,&namelen
			,NULL,&domainnamesize,&mSIDType);
	if(bret==TRUE)
		{
		Destroy();
		return false;
		}

	// Allocate domain name size
	if(mAccountName.Create(namelen+2)==false || mDomainnName.Create(domainnamesize+2)==false)
		{
		Destroy();
		return false;
		}

	bret=LookupAccountSidA(systemname,(PSID)mSIDBinary.GetBuffer(),mAccountName.GetBuffer(),&namelen
			,mDomainnName.GetBuffer(),&domainnamesize,&mSIDType);
	if(bret==FALSE)
		{
		Destroy();
		return false;
		}

	// Convert SID Binary to string
	char *newsidstring=0;
	bret=ConvertSidToStringSidA((PSID)mSIDBinary.GetBuffer(),&newsidstring);
	if(bret==FALSE)
		{
		Destroy();
		return false;
		}

	if(mSIDString.Create(sidstring)==false)
		{
		LocalFree(newsidstring);
		Destroy();
		return false;
		}

	LocalFree(newsidstring);

	return true;
	}


////////////////////////////////////////////////
bool MWinSID::Destroy(void)
	{
	mSIDString.Destroy();
	mSIDBinary.Destroy();
	mAccountName.Destroy();
	mDomainnName.Destroy();
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
const char *MWinSID::GetSIDString(void)
	{
	return mSIDString.Get();
	}


////////////////////////////////////////////////
PSID MWinSID::GetSIDBinary(void)
	{
	return (PSID)(void *)mSIDBinary.GetBuffer();
	}


////////////////////////////////////////////////
int MWinSID::GetSIDBinaryLength(void)
	{
	return mSIDBinary.GetSize();
	}


////////////////////////////////////////////////
const char *MWinSID::GetAccountName(void)
	{
	return mAccountName.GetBuffer();
	}


////////////////////////////////////////////////
const char *MWinSID::GetDomain(void)
	{
	return mDomainnName.GetBuffer();
	}


///////////////////////////////////////////////
SID_NAME_USE MWinSID::GetSIDType(void)
	{
	return mSIDType;
	}


///////////////////////////////////////////////
const char *MWinSID::GetSIDTypeString(void)
	{
	switch(mSIDType)
		{
		case SidTypeUser:				return "User";
		case SidTypeGroup:				return "Group";
		case SidTypeDomain:				return "Domain";
		case SidTypeAlias:				return "Alias";
		case SidTypeWellKnownGroup:		return "KnownGroup";
		case SidTypeDeletedAccount:		return "DeletedAccount";
		case SidTypeInvalid:			return "Invalid";
		case SidTypeUnknown:			return "Unknown";
		case SidTypeComputer:			return "Computer";
		case SidTypeLabel:				return "Label";
		case SidTypeLogonSession:		return "LogonSession";
		default:						return "?";
		}
	}

///////////////////////////////////////////////
bool MWinSID::IsOk(void)
	{
	if(IsValidSid((PSID)mSIDBinary.GetBuffer() )==FALSE)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////
bool MWinSID::Print(void)
	{
	MStdPrintf("SID=%s   Account=%s   Type=%s   Domain=%s\n",mSIDString.Get()
			,mAccountName.GetBuffer(),GetSIDTypeString(),mDomainnName.GetBuffer());

	return true;
	}


/////////////////////////////////////////////////
bool MWinSID::IsSame(MWinSID &othersid)
	{
	return true;
	}


