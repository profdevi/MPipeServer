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


//v3.0 copyright Comine.com 20150810M1138
#ifndef MStringArray_h
#define MStringArray_h

/////////////////////////////////////////////////
#include "MStdLib.h"
#include "MString.h"
#include "TVector.h"
#include "TAutoPtr.h"

/////////////////////////////////////////////////
class MStringArray
	{
	/////////////////////////////////////////////////
	bool mNullStringsAllowed;
	TVector<TAutoPtr<MString> > mList;
	int mCount;

	/////////////////////////////////////////////////
	void ClearObject(void);
	bool UpdateString(int index,const char *string);
	bool ResizeArray(int size);
	
	/////////////////////////////////////////////////
	public:
	MStringArray(bool create=false,bool nullstrings=false);
	~MStringArray(void);
	bool Create(bool nullstrings=false);
	bool Destroy(void);
	bool Clear(void);									// Clear all the strings
	int GetCount(void);									// Get Count in Array
	const char *GetString(int index);					// Get String
	bool SetString(int index,const char *string);		// Add string at position
	bool SetString(const char *string);					// Add string to end
	bool Remove(int index);								// Remove string at index
	bool IsMember(const char *string);					// Check if string is a member
	bool Print(void);									// Print out the Array List
	};

#endif // MStringArray_h

