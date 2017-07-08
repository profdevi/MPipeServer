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


//v1.11 copyright Comine.com 20150813R1241
#ifndef MPathNameList_h
#define MPathNameList_h

//////////////////////////////////////////////////
#include "MBuffer.h"

//******************************************************
//**  MPathNameList class
//******************************************************
class MPathNameList
	{
	////////////////////////////////////////////////
	char mDriveLetter;
	char **mList;
	int mMaxLen;
	int mLen;
	
	////////////////////////////////////////////////
	void ClearObject(void);
	bool InitArray(const char *path);

	////////////////////////////////////////////////
	public:
	MPathNameList(bool create=false);
	~MPathNameList(void);
	bool Create(void);
	bool Create(const char *absdirpath);
	bool Destroy(void);
	const char *Get(int index);
	int GetElementCount(void);
	bool GetFullPath(MBuffer &buffer);	
	bool Push(const char *element);			// Push Element into list
	bool Push(MPathNameList &paths);		// Push More Elements at end
	bool Pop(void);							// Pop Element off list
	bool Pop(int count);					// Pop Till Count elements exist
	const char *GetTop(void);				// Get Last Element
	char GetDriveLetter(void);				// Get Drive Letter
	bool IsSamePath(MPathNameList &lst);	// =true if paths are same
	bool IsSamePath(const char *path);		// =true if paths are same
	};

#endif // MPathNameList_h

