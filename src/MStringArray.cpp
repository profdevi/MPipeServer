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
#include "MStdLib.h"
#include "MString.h"
#include "TVector.h"
#include "TAutoPtr.h"
#include "MStringArray.h"


//******************************************************
//** MStringArray
//******************************************************
void MStringArray::ClearObject(void)
	{
	mNullStringsAllowed=false;
	mCount=0;
	}


/////////////////////////////////////////////////
bool MStringArray::UpdateString(int index,const char *string)
	{
	MStdAssert(index>=0 && index<mList.GetCount() );
	if(index<mCount)
		{
		if(mList[index].Create()==false || mList[index]->Create(string)==false)
			{
			return false;
			}

		return true;
		}

	// We need to resize to update the string
	if(ResizeArray(index+1)==false)
		{
		return false;
		}

	if(mList[index].Create()==false || mList[index]->Create(string)==false)
		{
		return false;
		}

	mCount=index+1;
	return true;
	}


/////////////////////////////////////////////////
bool MStringArray::ResizeArray(int size)
	{
	if(size<0)
		{
		return false;
		}
	else if(size<mCount)
		{
		// Keep original array
		for(int i=size;i<mCount;++i)
			{
			mList[i].Destroy();
			}

		mCount=size;
		return true;
		}
	else if(size<mList.GetCount() )
		{
		mCount=size;
		const int maxcap=mList.GetCount();
		for(int i=size;i<maxcap;++i)
			{
			mList[i].Destroy();
			}

		return true;
		}

	//=Space requires more capacity then mMaxCapacity

	// create a new list
	TVector<TAutoPtr<MString> > newlist;
	if(newlist.Create(size)==false)
		{
		return false;
		}

	// Copy array from original array
	for(int i=0;i<mCount;++i)
		{
		mList[i].Swap(newlist[i]);
		}
	
	mList.Swap(newlist);
	mCount=size;

	return true;
	}


/////////////////////////////////////////////////
MStringArray::MStringArray(bool create,bool nullstrings)
	{
	ClearObject();
	if(create==true && Create(nullstrings)==false)
		{
		return;
		}
	}


/////////////////////////////////////////////////
MStringArray::~MStringArray(void)
	{  Destroy();  }


/////////////////////////////////////////////////
bool MStringArray::Create(bool nullstrings)
	{
	Destroy();
	mNullStringsAllowed=nullstrings;
	return true;
	}


/////////////////////////////////////////////////
bool MStringArray::Destroy(void)
	{
	Clear();
	ClearObject();
	return true;
	}


/////////////////////////////////////////////////
bool MStringArray::Clear(void)
	{
	for(int i=0;i<mCount;++i)
		{
		mList[i].Destroy();
		}

	mCount=0;
	return true;
	}


/////////////////////////////////////////////////
int MStringArray::GetCount(void)
	{
	return mCount;
	}


/////////////////////////////////////////////////
const char *MStringArray::GetString(int index)
	{
	if(index<0 || index>=mCount)
		{
		if(mNullStringsAllowed==true) { return NULL; }
		return "";
		}

	if(mList[index].Get()==NULL)
		{
		if(mNullStringsAllowed==true) { return NULL; }
		return "";
		}

	return mList[index]->Get();
	}


/////////////////////////////////////////////////
bool MStringArray::SetString(int index,const char *string)
	{
	if(index>=mList.GetCount() )
		{
		if(ResizeArray(index+1)==false)
			{
			return false;
			}
		}

	return UpdateString(index,string);
	}


/////////////////////////////////////////////////
bool MStringArray::SetString(const char *string)
	{
	ResizeArray(mCount+1);
	return SetString(mCount-1,string);
	}


////////////////////////////////////////////////
bool MStringArray::Remove(int index)
	{
	if(index<0 || index>=mCount)
		{
		return false;
		}

	mList[index].Destroy();

	// Move All Strings down by one
	int i;
	for(i=index+1;i<mCount;++i)
		{  mList[i-1].Swap(mList[i]);  }

	mList[mCount-1].Destroy();
	mCount=mCount-1;

	return true;
	}


////////////////////////////////////////////////////
bool MStringArray::IsMember(const char *string)
	{
	int i;
	for(i=0;i<mCount;++i)
		{
		if(string==NULL)
			{
			if(mList[i].Get()==NULL) { return true; }
			continue;
			}
		
		if(MStdStrCmp(mList[i]->Get(),string)==0)
			{
			return true;
			}
		}

	return false;
	}


/////////////////////////////////////////////////////////////
bool MStringArray::Print(void)
	{
	int i;
	for(i=0;i<mCount;++i)
		{
		if(mList[i].Get()==NULL)
			{
			MStdPrintf("[%d:(null)]",i);
			}
		else
			{
			MStdPrintf("[%d:%s] ",i,mList[i]->Get() );
			}
		}

	MStdPrintf("\n");
	return true;
	}


