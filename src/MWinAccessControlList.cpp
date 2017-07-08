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
#include <Windows.h>
#include "MStdLib.h"
#include "MWinAccessControlList.h"


//******************************************************
//**  Module Elements
//******************************************************
static const int GACLBufferSize=2048;

//******************************************************
//**  MWinAccessControlList class
//******************************************************
void MWinAccessControlList::ClearObject(void)
	{
	mpACL=0;
	}


////////////////////////////////////////////////
MWinAccessControlList::MWinAccessControlList(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinAccessControlList::MWinAccessControlList(int maxacecount)
	{
	ClearObject();
	if(Create(maxacecount)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////
MWinAccessControlList::~MWinAccessControlList(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinAccessControlList::Create(int maxacecount)
	{
	Destroy();

	if(maxacecount<=0)
		{
		return false;
		}

	// Allocate Access Control List
	if(mMemoryBlock.Create(GACLBufferSize)==false)
		{
		Destroy();
		return false;
		}

	MStdMemZero(mMemoryBlock.Get(),mMemoryBlock.GetCount());

	mpACL=(PACL)mMemoryBlock.Get();

	if(InitializeAcl(mpACL,GACLBufferSize,ACL_REVISION)==FALSE)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinAccessControlList::Destroy(void)
	{
	mMemoryBlock.Destroy();
	ClearObject();
	return true;
	}


/////////////////////////////////////////////////
bool MWinAccessControlList::AddAccessAllow(const char *accountname,DWORD permission)
	{
	BYTE sidbuffer[100];
	PSID psid=(SID *)&sidbuffer;
	DWORD sidsize=sizeof(sidbuffer);
	char domainbuffer[100];
	DWORD domainbuffersize=sizeof(domainbuffer);
	SID_NAME_USE snu;

	if(LookupAccountNameA(NULL,accountname,psid,&sidsize
			,(LPSTR)&domainbuffer,&domainbuffersize,&snu)==FALSE)
		{
		return false;
		}

	if(AddAccessAllowedAce(mpACL,ACL_REVISION,permission,psid)==false)
		{
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////////////////////
bool MWinAccessControlList::AddAccessDeny(const char *accountname,DWORD permission)
	{
	BYTE sidbuffer[100];
	PSID psid=(SID *)&sidbuffer;
	DWORD sidsize=sizeof(sidbuffer);
	char domainbuffer[100];
	DWORD domainbuffersize=sizeof(domainbuffer);
	SID_NAME_USE snu;

	if(LookupAccountNameA(NULL,accountname,psid,&sidsize
			,(LPSTR)&domainbuffer,&domainbuffersize,&snu)==FALSE)
		{
		return false;
		}

	if(AddAccessDeniedAce(mpACL,ACL_REVISION,permission,psid)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////////
PACL MWinAccessControlList::GetACL(void)
	{
	return mpACL;
	}

