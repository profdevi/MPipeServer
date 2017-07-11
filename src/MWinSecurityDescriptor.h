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
#ifndef MWinSecurityDescriptor_h
#define MWinSecurityDescriptor_h

////////////////////////////////////////////////////
#include <windows.h>
#include <AccCtrl.h>
#include "MStdLib.h"
#include "THeapPtr.h"
#include "TVector.h"
#include "MWinSID.h"
#include "MWinAccessControlList.h"

//******************************************************
//**  MWinSecurityDescriptor class
//******************************************************
class MWinSecurityDescriptor
	{
	////////////////////////////////////////////////
	THeapPtr<SECURITY_DESCRIPTOR> mDescriptor;
	THeapPtr<MWinSID> mOwnerSID;
	THeapPtr<MWinSID> mGroupSID;
	
	////////////////////////////////////////////////
	bool mDACLSet;
	bool mSACLSet;

	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MWinSecurityDescriptor(void);
	~MWinSecurityDescriptor(void);
	bool Create(void);					// Maximum Access Control Entries
	bool Destroy(void);
	bool SetOwner(const char *newowner);		// Set new Owner
	bool SetGroup(const char *newgroup);		// Set new Group
	bool SetDACL(MWinAccessControlList &dacl);	// Set the Descretionary Access Control List
	bool SetSACL(MWinAccessControlList &sacl);	// Set the System Access Control List
	bool AssignFile(const char *filename);		// Assign to a file	(Depricated)
	PSECURITY_DESCRIPTOR GetPSD(void);			// Get Pointer to Security Descriptor
	};


//*************************************************************
//** Functions
//*************************************************************
// Set SE_ object type
bool MWinSecurityDescriptorSet(const char *objectname,SE_OBJECT_TYPE objecttype=SE_FILE_OBJECT
		,MWinAccessControlList *pdaclobj=0,MWinAccessControlList *psaclobj=0,MWinSID *powner=0,MWinSID *pgroup=0);



#endif // MWinSecurityDescriptor_h

