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


//v2.1 copyright Comine.com 20150912S0722
#ifndef MBuffer_h
#define MBuffer_h

/////////////////////////////////////////////////////
#include "MStdLib.h"

/////////////////////////////////////////////////////
class MBuffer
	{
	char *mBuffer;								// Buffer start
	int mSize;									// Maximum Size of buffer

	/////////////////////////////////////////////////
	void ClearObject(void);

	/////////////////////////////////////////////////
	public:
	MBuffer(void);
	explicit MBuffer(int size);					// Auto allocate buffer in construction
	explicit MBuffer(const MBuffer &ref);		// Copy Constructor
	~MBuffer(void);								// Destructor
	bool Create(int n);							// Create a buffer of size n bytes
	bool Create(const MBuffer &ref);			// Create a copy of another buffer
	bool Destroy(void);							// Destroys the current buffer
	char *GetBuffer(void);						// Gets buffer
	wchar_t *GetWideBuffer(void);				// Returns the buffer as wide string
	char *operator()(void);						// Auto casting operator
	operator char *(void);						// String casting operator
	int GetSize(void);							// Get S
	bool SetString(const char *string);			// Set a known string
	bool SetString(const wchar_t *string);		// Set a known string
	bool SetData(void *data,int length);		// Copy Data into buffer
	char &operator [](int index);				// Array [] operator
	bool StringClear(void);						// Sets ""
	bool StringAppend(const char *string);		// Append a string
	bool CharAppend(char data);					// Append a char data
	bool StringPrepend(const char *string);		// Prepend String before string
	bool CharPrepend(char data);				// Prepend data before string
	bool operator=(const MBuffer &ref);			// Assignment operator
	bool ConvertWideToAscii(void);				// Convert the string from Wide to Ascii
	bool ConvertAsciiToWide(void);				// COnvert the string from ascii to wide
	bool Clear(int val=0);						// Clear buffer with val
	bool Compare(const char *string);			// =true if strings match
	bool Compare(const wchar_t *string);		// =true if strings match
	int GetStringLength(void);					// =length of ascii string
	int GetWideStringLength(void);				// =length of unicode string
	int Search(int start,const char *value,int len);	// =Return index of search value and len
	int ISearch(int start,const char *value,int len);	// =Return index of search value and len
	bool TrimRight(void);						// Remove Extra Spaces on right side
	bool GetSubString(int start,int length,MBuffer &out,int requestoutsize=0);	// Get Sub String and place into new MBuffer
	bool StrRemove(int length);					// Remove the first n characters
	};

#endif // MBuffer_h


