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


//v3.0 copyright Comine.com 20150810M1144
#include "MStdLib.h"
#include "TVector.h"
#include "MStringBuffer.h"


//********************************************************************
//** Module Elements
//********************************************************************
static bool GIsSpace(char ch)
	{
	if(ch==' ') { return true; }
	else if(ch=='\t') { return true; }
	else if(ch=='\r') { return true; }
	else if(ch=='\n') { return true; }
	return false;
	}


//********************************************************************
//** MStringBuffer 
//********************************************************************
void MStringBuffer::ClearObject(void)
	{
	mEndPos=0;
	}


////////////////////////////////////////////////////////////////
MStringBuffer::MStringBuffer(void)
	{  ClearObject(); }


////////////////////////////////////////////////////////////////
MStringBuffer::MStringBuffer(int length)
	{
	ClearObject();
	if(Create(length)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////////////////////
MStringBuffer::~MStringBuffer(void)
	{  Destroy();  }


////////////////////////////////////////////////////////////////
bool MStringBuffer::Create(int length)
	{
	Destroy();

	if(mBuffer.Create(length+1)==false)
		{
		Destroy();
		return false;
		}

	mEndPos=0;
	mBuffer[mEndPos]=0;
	return true;
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::Create(const char *string)
	{
	Destroy();
	if(string==NULL)
		{
		return false;
		}

	// Check if space is enough for string
	int length=MStdStrLen(string);
	if(Create(length)==false) {  return false; }

	//=we have enough space for string

	//copy string into buffer
	MStdStrCpy(mBuffer.Get(),string);
	mEndPos=length;
	return true;
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::Destroy(void)
	{
	mBuffer.Destroy();
	ClearObject();
	return true;
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::Clear(void)
	{
	mEndPos=0;
	mBuffer[mEndPos]=0;
	return true;
	}


////////////////////////////////////////////////////////////////
const char *MStringBuffer::Get(void)
	{
	return mBuffer.Get();
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::Add(char ch)
	{
	if(mEndPos>=mBuffer.GetCount() )
		{
		return false;
		}

	mBuffer[mEndPos]=ch;
	mEndPos=mEndPos+1;
	mBuffer[mEndPos]=0;
	return true;
	}


/////////////////////////////////////////////////////////////////
bool MStringBuffer::Add(const char *st)		// Add str to buffer
	{
	for(int i=0;st[i]!=0;++i)
		{
		if(Add(st[i])==false) { return false; }
		}

	return true;
	}


/////////////////////////////////////////////////////////////////
bool MStringBuffer::Add(int index,char ch)			// Insert a char at index
	{
	if(mEndPos>=mBuffer.GetCount() )
		{
		return false;
		}

	MStdMemMove(mBuffer.Get()+index+1,mBuffer.Get()+index,mEndPos-index);
	mBuffer[index]=ch;
	mEndPos += 1;
	mBuffer[mEndPos]=0;
	return true;
	}


/////////////////////////////////////////////////////////////////
bool MStringBuffer::Set(int index,char ch)			// Set char at index
	{
	mBuffer[index]=ch;

	// if zero, index is the end position
	if(ch==0) { mEndPos=index; }

	return true;
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::Set(const char *string)		// Set a string in space
	{

	if(string==NULL)
		{
		return false;
		}

	// Check if space is enough for string
	int length=MStdStrLen(string);
	if(length>mBuffer.GetCount() )
		{
		if(Create(length)==false) {  return false; }
		//=Now We have extra space for string
		}

	//=we have enough space for string

	//copy string into buffer
	MStdStrCpy(mBuffer.Get(),string);
	mEndPos=length;
	return true;
	}


/////////////////////////////////////////////////////////////////
char MStringBuffer::Get(int index)					// returns char at index
	{
	return mBuffer[index];
	}


////////////////////////////////////////////////////////////////
int MStringBuffer::GetStringLength(void)
	{
	return mEndPos;
	}


////////////////////////////////////////////////////////////////
char MStringBuffer::operator[](int index)				// Efficient way of accessing  a character
	{
	return mBuffer[index];
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::TrimLeft(void)	// Trim extra spaces from left
	{
	int src,dst;  bool stopflag=false;
	for(src=dst=0;mBuffer[src]!=0;++src)
		{
		if(GIsSpace(mBuffer[src])==false)
			{ stopflag=true; }
		else if(stopflag==false)
			{ continue; }

		mBuffer[dst]=mBuffer[src];
		dst=dst+1;
		}

	// Update position
	mBuffer[dst]=0;
	mEndPos=dst;
	return true;
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::TrimRight(void)			// Trim Extra spaces from right
	{
	int dst;
	for(dst=mEndPos-1;dst>=0;--dst)
		{
		if(GIsSpace(mBuffer[dst])==false) { break; }

		mBuffer[dst]=0;
		mEndPos=dst;
		}

	return true;
	}


////////////////////////////////////////////////////////////////
bool MStringBuffer::Trim(void)			// Trim all unnecessary spaces
	{
	int src,dst;
	for(src=dst=0;mBuffer[src]!=0;++src)
		{
		if(GIsSpace(mBuffer[src])==true) { continue; }

		//=Processing a good character

		mBuffer[dst]=mBuffer[src];
		dst=dst+1;
		}

	// Update position
	mBuffer[dst]=0;
	mEndPos=dst;
	return true;
	}


///////////////////////////////////////////////////////////////
bool MStringBuffer::Compare(const char *string)		// Compare string with buffer content
	{
	if(string==NULL)
		{
		return false;
		}

	if(MStdStrCmp(mBuffer.Get(),string)==0) { return true; }

	return false;
	}


//////////////////////////////////////////////////////////////
bool MStringBuffer::ICompare(const char *string)		// Case Insensitive Compare
	{
	if(string==NULL)
		{
		return false;
		}

	if(MStdStrICmp(mBuffer.Get(),string)==0) { return true; }

	return false;
	}


///////////////////////////////////////////////////////////////
bool MStringBuffer::CutLeft(int length)
	{
	if(mEndPos<=length)
		{
		Clear();
		return true;
		}

	if(length<=0){ return true; }
	
	char *src=mBuffer.Get()+length;
	char *dst=mBuffer.Get();
	for(;;)
		{
		if((*dst++=*src++)==0) {  break; }
		}
	
	mEndPos = mEndPos - length;
	MStdAssert(mEndPos>=0 && mBuffer[mEndPos]==0 );
	return true;
	}


///////////////////////////////////////////////////////////////
bool MStringBuffer::CompareLeft(const char *string)
	{
	const char *q;
	const char *p=mBuffer.Get();
	for(q=string;*q!=0;++q)
		{
		if(*p!=*q) { return false; }
		p=p+1;
		}

	return true;
	}


/////////////////////////////////////////////////////////////
bool MStringBuffer::CapitilizeFirstLetter(void)
	{
	enum { INWORD=1,OUTWORD=2 } state=OUTWORD;

	char *pdata;
	for(pdata=mBuffer.Get();*pdata!=0;++pdata)
		{
		if(state==OUTWORD && MStdIsAlpha(*pdata)==true)
			{
			state=INWORD;
			*pdata = MStdToUpper(*pdata);
			continue;
			}

		if(state==OUTWORD && MStdIsSpace(*pdata)==false)
			{
			state=INWORD;
			continue;
			}

		if(state==INWORD && MStdIsAlpha(*pdata)==true)
			{
			state=INWORD;
			*pdata = MStdToLower(*pdata);
			continue;
			}

		if(state==INWORD && MStdIsSpace(*pdata)==true)
			{
			state=OUTWORD;
			continue;
			}
		}

	return true;
	}


//////////////////////////////////////////////////////////
bool MStringBuffer::ToUpperCase(void)
	{
	MStdStrToUpper(mBuffer.Get() );
	return true;
	}


/////////////////////////////////////////////////////////
bool MStringBuffer::ToLowerCase(void)
	{
	MStdStrToLower(mBuffer.Get() );
	return true;
	}


