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


//v1.3 copyright Comine.com 20170711S1323
#include <windows.h>
#include <AccCtrl.h>
#include <AclAPI.h>
#include "MStdLib.h"
#include "THeapPtr.h"
#include "TVector.h"
#include "MWinAccessToken.h"
#include "MWinAccessControlList.h"
#include "MWinSecurityDescriptor.h"


///////////////////////////////////////////////
#pragma comment(lib,"advapi32.lib")

//******************************************************
//**  MWinSecurityDescriptor class
//******************************************************
void MWinSecurityDescriptor::ClearObject(void)
	{
	mDACLSet=false;
	mSACLSet=false;
	}


////////////////////////////////////////////////
MWinSecurityDescriptor::MWinSecurityDescriptor(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWinSecurityDescriptor::~MWinSecurityDescriptor(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWinSecurityDescriptor::Create(void)
	{
	Destroy();

	//**Convert Process to have security
	MWinAccessToken currenttoken;
	if(currenttoken.Create()==false)
		{
		Destroy();
		return false;
		}

	if(currenttoken.EnablePrivilege(SE_TAKE_OWNERSHIP_NAME,true)==false)
		{
		Destroy();
		return false;
		}

	if(currenttoken.EnablePrivilege(SE_RESTORE_NAME,true)==false)
		{
		Destroy();
		return false;
		}

	if(currenttoken.EnablePrivilege(SE_BACKUP_NAME,true)==false)
		{
		Destroy();
		return false;
		}

	if(currenttoken.EnablePrivilege(SE_CHANGE_NOTIFY_NAME,true)==false)
		{
		Destroy();
		return false;
		}

	currenttoken.Destroy();


	if(mDescriptor.Create()==false)
		{
		Destroy();
		return false;
		}

	// Initialized absolute security descriptor
	if(InitializeSecurityDescriptor(mDescriptor.Get(),SECURITY_DESCRIPTOR_REVISION)==FALSE)
		{
		Destroy();
		return false;
		}

	if(IsValidSecurityDescriptor(mDescriptor.Get() )==FALSE)
		{
		Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWinSecurityDescriptor::Destroy(void)
	{
	mDescriptor.Destroy();
	mOwnerSID.Destroy();
	mGroupSID.Destroy();
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MWinSecurityDescriptor::SetOwner(const char *owner)
	{
	if(mOwnerSID.Create()==false || mOwnerSID->CreateFromAccountName(owner)==false ) 
		{
		mOwnerSID.Destroy();
		return false;
		}

	if(SetSecurityDescriptorOwner(mDescriptor.Get(),mOwnerSID->GetSIDBinary(),FALSE)==FALSE)
		{
		mOwnerSID.Destroy();
		return false;
		}

	if(IsValidSecurityDescriptor(mDescriptor.Get())==FALSE)
		{
		mOwnerSID.Destroy();
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////
bool MWinSecurityDescriptor::SetGroup(const char *newgroup)
	{
	if(mGroupSID.Create()==false || mGroupSID->CreateFromAccountName(newgroup)==false)
		{
		mGroupSID.Destroy();
		return false;
		}

	//=We Now have a SID for account

	if(SetSecurityDescriptorGroup(mDescriptor.Get(),mGroupSID->GetSIDBinary(),FALSE)==FALSE)
		{
		mGroupSID.Destroy();
		return false;
		}

	if(IsValidSecurityDescriptor(mDescriptor.Get())==FALSE)
		{
		mGroupSID.Destroy();
		return false;
		}
	
	return true;	
	}


////////////////////////////////////////////////////////
bool MWinSecurityDescriptor::SetDACL(MWinAccessControlList &dacl)
	{
	if(SetSecurityDescriptorDacl(mDescriptor.Get(),TRUE,dacl.GetACL(),FALSE)==FALSE)
		{
		return false;
		}
	
	mDACLSet=true;
	return true;
	}


/////////////////////////////////////////////////////////
bool MWinSecurityDescriptor::SetSACL(MWinAccessControlList &sacl)
	{
	if(SetSecurityDescriptorSacl(mDescriptor.Get(),TRUE,sacl.GetACL(),FALSE)==FALSE)
		{
		return false;
		}
	
	mSACLSet=true;
	return true;
	}



///////////////////////////////////////////////////////
bool MWinSecurityDescriptor::AssignFile(const char *filename)	
	{
	DWORD flags=0;
	if(mOwnerSID.Get()!=0) {  flags |= OWNER_SECURITY_INFORMATION;  }
	if(mGroupSID.Get()!=0) {  flags |= GROUP_SECURITY_INFORMATION;  }
	if(mDACLSet==true) { flags |= DACL_SECURITY_INFORMATION;  }
	if(mSACLSet==true) { flags |= SACL_SECURITY_INFORMATION;  }

	if(SetFileSecurityA(filename,flags,mDescriptor.Get() )==FALSE)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////////////////
PSECURITY_DESCRIPTOR MWinSecurityDescriptor::GetPSD(void)
	{
	return mDescriptor.Get();
	}


//*************************************************************
//** Functions
//*************************************************************
bool MWinSecurityDescriptorSet(const char *objectname,SE_OBJECT_TYPE objecttype
		,MWinAccessControlList *pdaclobj,MWinAccessControlList *psaclobj,MWinSID *powner,MWinSID *pgroup)
	{
	MStdAssert(objectname!=0);

	// Update security info + reference SIDS/ACLs
	SECURITY_INFORMATION secinfo=0;

	PSID pownersid=0;
	if(powner!=0)
		{
		pownersid=powner->GetSIDBinary();
		secinfo |=OWNER_SECURITY_INFORMATION;
		}

	PSID pgroupsid=0;
	if(pgroup!=0)
		{
		pgroupsid=pgroup->GetSIDBinary();
		secinfo |=GROUP_SECURITY_INFORMATION;
		}

	PACL pdacl=0;
	if(pdaclobj!=0)
		{
		pdacl=pdaclobj->GetACL();
		secinfo |= DACL_SECURITY_INFORMATION;
		}

	PACL psacl=0;
	if(psacl!=0)
		{
		psacl=psaclobj->GetACL();
		secinfo |= SACL_SECURITY_INFORMATION;
		}

	MStringWide objectnamewide=objectname;
	

	// Call Windows API to set security info
	DWORD retval;
	retval=SetNamedSecurityInfoA((char*)objectname,objecttype,secinfo,pownersid,pgroupsid,pdacl,psacl);
	if(retval!=ERROR_SUCCESS)
		{
		return false;
		}

	return true;
	}




