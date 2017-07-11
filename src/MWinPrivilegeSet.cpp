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
#include <Windows.h>
#include "MStdLib.h"
#include "TVector.h"
#include "MString.h"
#include "MWinPrivilegeSet.h"



//******************************************************
//**  MWinPrivilegeSet class
//******************************************************
void MWinPrivilegeSet::ClearObject(void)
	{
	mMaxPrivileges=0;
	mCount=0;
	}


////////////////////////////////////////////////
MWinPrivilegeSet::MWinPrivilegeSet(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinPrivilegeSet::~MWinPrivilegeSet(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinPrivilegeSet::Create(int maxprivileges)
	{
	Destroy();
	MStdAssert(maxprivileges>0);
	mMaxPrivileges=maxprivileges;
	
	const int dataneeded=sizeof(TOKEN_PRIVILEGES)+(maxprivileges-1)*sizeof(LUID_AND_ATTRIBUTES);
	if(mPrivilegeData.Create(dataneeded)==false)
		{
		Destroy();
		return false;
		}

	mpPrivilegeData=(TOKEN_PRIVILEGES *)mPrivilegeData.Get();
	MStdMemZero(mpPrivilegeData,dataneeded);

	mpPrivilegeData->PrivilegeCount=0;
	mCount=0;
	return true;
	}


////////////////////////////////////////////////
bool MWinPrivilegeSet::CreateFromProcess(HANDLE haccesstoken)
	{
	Destroy();

	// Get information buffer size
	DWORD length=0;
	if(GetTokenInformation(haccesstoken,TOKEN_INFORMATION_CLASS::TokenPrivileges
				,NULL,0,&length)==FALSE  && length==0)
		{
		return false;
		}

	if(mPrivilegeData.Create(length)==false)
		{
		Destroy();
		return false;
		}

	
	if(GetTokenInformation(haccesstoken,TOKEN_INFORMATION_CLASS::TokenPrivileges,mPrivilegeData.Get()
				,mPrivilegeData.GetCount(),&length)==FALSE)
		{
		Destroy();
		return false;
		}

	mpPrivilegeData=(TOKEN_PRIVILEGES *)mPrivilegeData.Get();
	mCount=(int)mpPrivilegeData->PrivilegeCount;
	mMaxPrivileges=mCount;

	return true;
	}


////////////////////////////////////////////////
bool MWinPrivilegeSet::Destroy(void)
	{
	mPrivilegeData.Destroy();
	ClearObject();
	return true;
	}



////////////////////////////////////////////////
TOKEN_PRIVILEGES *MWinPrivilegeSet::GetPrivileges(void)
	{
	return mpPrivilegeData;
	}


///////////////////////////////////////////////
bool MWinPrivilegeSet::EnablePrivilege(const wchar_t *privledge,bool enable)
	{
	if(mCount>=mMaxPrivileges)
		{
		return false;
		}

	if(LookupPrivilegeValueW(NULL, privledge,&(mpPrivilegeData->Privileges[mCount].Luid) )==FALSE)
		{
		return false;
		}

	// Enable the privilege
	if(enable==true)
		{
		mpPrivilegeData->Privileges[mCount].Attributes = SE_PRIVILEGE_ENABLED; 
		}
	else
		{
		mpPrivilegeData->Privileges[mCount].Attributes = 0; 
		}

	// Update count of privledges
	mCount=mCount+1;
	mpPrivilegeData->PrivilegeCount=mCount;
	return true;
	}


//////////////////////////////////////////////////////////
bool MWinPrivilegeSet::Print(void)
	{
	TVector<wchar_t> buffer(2048);
	buffer.Set(0);

	DWORD buffercap=buffer.GetCount();
	for(int i=0;i<mCount;++i)
		{
		buffer.Set(0);
		buffercap=buffer.GetCount();
		if(LookupPrivilegeNameW(NULL,&(mpPrivilegeData->Privileges[i].Luid),buffer.Get(),&buffercap)==FALSE)
			{	
			return false;
			}

		MStdPrintf("%-30S : %u   ",buffer.Get(),mpPrivilegeData->Privileges[i].Attributes);

		if(mpPrivilegeData->Privileges[i].Attributes==0) { MStdPrintf("SE_PRIVILEGE_USED_FOR_ACCESS   "); }

		if((mpPrivilegeData->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED)>0) {  MStdPrintf("SE_PRIVILEGE_ENABLED   ");  }
		if((mpPrivilegeData->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED_BY_DEFAULT)>0) {  MStdPrintf("SE_PRIVILEGE_ENABLED_BY_DEFAULT   ");  }
		if((mpPrivilegeData->Privileges[i].Attributes & SE_PRIVILEGE_REMOVED)>0) {  MStdPrintf("SE_PRIVILEGE_REMOVED   ");  }
		MStdPrintf("\n");
		}	

	return true;
	}


////////////////////////////////////////////////////////////////
int MWinPrivilegeSet::GetCount(void) const
	{
	return mCount;
	}


////////////////////////////////////////////////////////////////
int MWinPrivilegeSet::GetMaxCount(void) const
	{
	return mMaxPrivileges;
	}


////////////////////////////////////////////////////////////////
DWORD MWinPrivilegeSet::GetAttributes(int index) const
	{
	MStdAssert(index>=0 && index<mCount);
	return mpPrivilegeData->Privileges[index].Attributes;
	}
	


////////////////////////////////////////////////////////////////
LUID MWinPrivilegeSet::GetLUID(int index) const
	{
	MStdAssert(index>=0 && index<mCount);
	return mpPrivilegeData->Privileges[index].Luid;
	}


bool MWinPrivilegeSet::GetLUIDName(int index,MString &luidname) const
	{
	MStdAssert(index>=0 && index<mCount);
	LUID luid=mpPrivilegeData->Privileges[index].Luid;

	TVector<wchar_t> wideluidname(1024);
	DWORD wideluidnamespace=wideluidname.GetCount();
	if(LookupPrivilegeNameW(NULL,&(mpPrivilegeData->Privileges[index].Luid),wideluidname.Get()
			,&wideluidnamespace)==FALSE)
		{
		return false;
		}

	if(luidname.Create(wideluidname.Get())==false)
		{
		return false;
		}

	return true;
	}




