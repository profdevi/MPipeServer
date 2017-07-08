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
#include <windows.h>
#include "MStdLib.h"
#include "MString.h"
#include "MBuffer.h"
#include "MWinAccessToken.h"


//******************************************************
//**  MWinAccessToken class
//******************************************************
void MWinAccessToken::ClearObject(void)
	{
	mhAccessToken=INVALID_HANDLE_VALUE;
	mTakeOwnership=false;
	}


////////////////////////////////////////////////
MWinAccessToken::MWinAccessToken(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinAccessToken::~MWinAccessToken(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinAccessToken::Create(DWORD access)
	{
	Destroy();
	if (OpenProcessToken(GetCurrentProcess(),access, &mhAccessToken)==FALSE)
		{
		Destroy();
		return false;
		}

	// Take Ownership of handle to release on destruction
	mTakeOwnership=true;

	return true;
	}


////////////////////////////////////////////////
bool MWinAccessToken::Create(HANDLE haccesstoken,bool takeownship)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MWinAccessToken::Destroy(void)
	{
	if(mhAccessToken!=INVALID_HANDLE_VALUE && mTakeOwnership==true)
		{
		CloseHandle(mhAccessToken);
		mhAccessToken=INVALID_HANDLE_VALUE;
		}

	ClearObject();
	return true;
	}


///////////////////////////////////////////////
bool MWinAccessToken::AddPrivilege(const wchar_t *privledge)
	{
	TOKEN_PRIVILEGES tokpriv;
	MStdMemSet(&tokpriv,0,sizeof(tokpriv) );

	if(LookupPrivilegeValueW(NULL, privledge,&(tokpriv.Privileges[0].Luid) )==FALSE)
		{
		return false;
		}

	// Change Privledges
	tokpriv.PrivilegeCount = 1;
	tokpriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	if(AdjustTokenPrivileges(mhAccessToken, FALSE, &tokpriv, 0,(PTOKEN_PRIVILEGES)NULL, 0)==FALSE)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////
bool MWinAccessToken::DelPrivilege(const wchar_t *privledge)
	{
	TOKEN_PRIVILEGES tokpriv;
	MStdMemSet(&tokpriv,0,sizeof(tokpriv) );

	if(LookupPrivilegeValueW(NULL, privledge,&(tokpriv.Privileges[0].Luid) )==FALSE)
		{
		return false;
		}

	// Change Privledges
	tokpriv.PrivilegeCount = 1;
	tokpriv.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED; 

	if(AdjustTokenPrivileges(mhAccessToken, FALSE, &tokpriv, 0,(PTOKEN_PRIVILEGES)NULL, 0)==FALSE)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////
bool MWinAccessToken::HasPrivilege(const wchar_t *privledge)
	{
	// Lookup Privilege Locally Unique ID(LUID)
	LUID luid;
	if(LookupPrivilegeValue(NULL,privledge,&luid)==FALSE)
		{
		return false;
		}

	// Get information buffer size
	DWORD length=0;
	if(GetTokenInformation(mhAccessToken,TokenPrivileges,NULL,0,&length)==FALSE  && length==0)
		{
		return false;
		}

	// Now We Allocate space for the privldge array
	MBuffer buffer(length);
	if(GetTokenInformation(mhAccessToken,TokenPrivileges,buffer.GetBuffer()
			,buffer.GetSize(),&length)==FALSE)
		{
		return false;
		}

	TOKEN_PRIVILEGES *privileges;
	privileges=(TOKEN_PRIVILEGES *)buffer.GetBuffer();
	
	unsigned int i;
	for(i=0;i<privileges->PrivilegeCount;++i)
		{
		if(privileges->Privileges[i].Luid.HighPart != luid.HighPart
				|| privileges->Privileges[i].Luid.LowPart !=luid.LowPart )
			{  continue;  }

		//=We have found the luid
		DWORD attribute=privileges->Privileges[i].Attributes;
		
		if((attribute|SE_PRIVILEGE_ENABLED)>0)
			{
			return true;
			}
		}
	
	return false;
	}


////////////////////////////////////////////////////////
bool MWinAccessToken::RemoveAllPrivileges(void)
	{
	// Adjust all token privledges
	if(AdjustTokenPrivileges(mhAccessToken, TRUE, NULL, 0,NULL, 0)==FALSE)
		{
		return false;
		}

	return true;	
	}


////////////////////////////////////////////////
bool MWinAccessToken::GetOwner(MString &owner,SID_NAME_USE &usertype)
	{
	DWORD length=0;
	if(GetTokenInformation(mhAccessToken,TokenOwner,NULL,0,&length)==FALSE 
				&& length==0)
		{
		return false;
		}

	MBuffer buffer(length);
	if(GetTokenInformation(mhAccessToken,TokenOwner,buffer.GetBuffer(),buffer.GetSize(),&length)==FALSE)
		{
		return false;
		}

	TOKEN_OWNER *ptrtokenowner=(TOKEN_OWNER *)buffer.GetBuffer();

	MBuffer name(250);
	BOOL ret;
	DWORD tmpsize=name.GetSize()-1;
	MBuffer refdomainname(250);
	DWORD refdomainlength=refdomainname.GetSize()-1;

	ret=LookupAccountSidA(NULL,ptrtokenowner->Owner,name.GetBuffer()
			,&tmpsize,refdomainname.GetBuffer(),&refdomainlength,&usertype);
	if(ret==FALSE)
		{
		return false;
		}

	if(owner.Create(name.GetBuffer())==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////
bool MWinAccessToken::Print(void)
	{
	MString name;
	SID_NAME_USE nametype;

	if(GetOwner(name,nametype)==false)
		{
		return false;
		}

	MStdPrintf("Token Owner  : %s\n",name.Get() );

	/*
	SidTypeUser = 1,
    SidTypeGroup,
    SidTypeDomain,
    SidTypeAlias,
    SidTypeWellKnownGroup,
    SidTypeDeletedAccount,
    SidTypeInvalid,
    SidTypeUnknown,
    SidTypeComputer,
    SidTypeLabel
	*/

	switch(nametype)
		{
		/////////////////////////////////////
		case SidTypeUser:
			MStdPrintf("Token Type   : SidTypeUser\n");
			break;

		/////////////////////////////////////
		case SidTypeGroup:
			MStdPrintf("Token Type   : SidTypeGroup\n");
			break;

		/////////////////////////////////////
		case SidTypeDomain:
			MStdPrintf("Token Type   : SidTypeDomain\n");
			break;

		/////////////////////////////////////
		case SidTypeAlias:
			MStdPrintf("Token Type   : SidTypeAlias\n");
			break;

		/////////////////////////////////////
		case SidTypeWellKnownGroup:
			MStdPrintf("Token Type   : SidTypeWellKnownGroup\n");
			break;

		/////////////////////////////////////
		case SidTypeDeletedAccount:
			MStdPrintf("Token Type   : SidTypeDeletedAccount\n");
			break;

		/////////////////////////////////////
		case SidTypeInvalid:
			MStdPrintf("Token Type   : SidTypeInvalid\n");
			break;

		/////////////////////////////////////
		case SidTypeUnknown:
			MStdPrintf("Token Type   : SidTypeUnknown\n");
			break;

		/////////////////////////////////////
		case SidTypeComputer:
			MStdPrintf("Token Type   : SidTypeComputer\n");
			break;

		/////////////////////////////////////
		case SidTypeLabel:
			MStdPrintf("Token Type   : SidTypeLabel\n");
			break;

		////////////////////////////////
		default:
			MStdPrintf("Token Type   : Unknown\n");
			break;
		};

	// Show Privledges
	MStdPrintf("Privledges:\n");

	#define GCHECKPRIVLEDGE(x)			if(HasPrivilege(x)==true)\
											{\
											MStdPrintf("\t" #x "\n");\
											}

	GCHECKPRIVLEDGE(SE_CREATE_TOKEN_NAME);
	GCHECKPRIVLEDGE(SE_ASSIGNPRIMARYTOKEN_NAME);
	GCHECKPRIVLEDGE(SE_LOCK_MEMORY_NAME);
	GCHECKPRIVLEDGE(SE_INCREASE_QUOTA_NAME);
	//GCHECKPRIVLEDGE(SE_UNSOLICITED_INPUT_NAME);
	GCHECKPRIVLEDGE(SE_MACHINE_ACCOUNT_NAME);
	GCHECKPRIVLEDGE(SE_TCB_NAME);
	GCHECKPRIVLEDGE(SE_SECURITY_NAME);
	GCHECKPRIVLEDGE(SE_TAKE_OWNERSHIP_NAME);
	GCHECKPRIVLEDGE(SE_LOAD_DRIVER_NAME);
	GCHECKPRIVLEDGE(SE_SYSTEM_PROFILE_NAME);
	GCHECKPRIVLEDGE(SE_SYSTEMTIME_NAME);
	GCHECKPRIVLEDGE(SE_PROF_SINGLE_PROCESS_NAME);
	GCHECKPRIVLEDGE(SE_INC_BASE_PRIORITY_NAME);
	GCHECKPRIVLEDGE(SE_CREATE_PAGEFILE_NAME);
	GCHECKPRIVLEDGE(SE_CREATE_PERMANENT_NAME);
	GCHECKPRIVLEDGE(SE_BACKUP_NAME);
	GCHECKPRIVLEDGE(SE_RESTORE_NAME);
	GCHECKPRIVLEDGE(SE_SHUTDOWN_NAME);
	GCHECKPRIVLEDGE(SE_DEBUG_NAME);
	GCHECKPRIVLEDGE(SE_AUDIT_NAME);
	GCHECKPRIVLEDGE(SE_SYSTEM_ENVIRONMENT_NAME);
	GCHECKPRIVLEDGE(SE_CHANGE_NOTIFY_NAME);
	GCHECKPRIVLEDGE(SE_REMOTE_SHUTDOWN_NAME);
	GCHECKPRIVLEDGE(SE_UNDOCK_NAME);
	GCHECKPRIVLEDGE(SE_SYNC_AGENT_NAME);
	GCHECKPRIVLEDGE(SE_ENABLE_DELEGATION_NAME);
	GCHECKPRIVLEDGE(SE_MANAGE_VOLUME_NAME);
	GCHECKPRIVLEDGE(SE_IMPERSONATE_NAME);
	GCHECKPRIVLEDGE(SE_CREATE_GLOBAL_NAME);
	GCHECKPRIVLEDGE(SE_TRUSTED_CREDMAN_ACCESS_NAME);
	GCHECKPRIVLEDGE(SE_RELABEL_NAME);
	GCHECKPRIVLEDGE(SE_INC_WORKING_SET_NAME);
	GCHECKPRIVLEDGE(SE_TIME_ZONE_NAME);
	GCHECKPRIVLEDGE(SE_CREATE_SYMBOLIC_LINK_NAME);
	
	#undef GCHECKPRIVLEDGE
	
	return true;
	}

