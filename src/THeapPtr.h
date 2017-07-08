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


//v1.1 copyright Comine.com 20170706R0717
#ifndef THeapPtr_h
#define THeapPtr_h

////////////////////////////////////////////
#include "MStdLib.h"

///////////////////////////////////////////
// THeapPtr Declaration
///////////////////////////////////////////
template <class DataType>
class THeapPtr
	{
	DataType *mObjectRef;

	/////////////////////////////////////////
	public:
	THeapPtr(void);
	THeapPtr(bool create);
	~THeapPtr(void);
	bool Create(void);
	bool Destroy(void);
	DataType *Get(void);
	DataType & operator*(void);
	DataType *operator->(void);
	int GetSize(void);
	};

///////////////////////////////////////////
// THeapPtr Implementation
///////////////////////////////////////////
template <class DataType>
THeapPtr<DataType>::THeapPtr(void)
	{
	mObjectRef=0;
	}


///////////////////////////////////////////
template <class DataType>
THeapPtr<DataType>::THeapPtr(bool create)
	{
	mObjectRef=0;
	if(create==true)
		{
		mObjectRef=new DataType;
		}
	}


//////////////////////////////////////////
template <class DataType>
THeapPtr<DataType>::~THeapPtr(void)
	{
	if(mObjectRef!=0)
		{
		delete mObjectRef;
		mObjectRef=0;
		}
	}


///////////////////////////////////////
template <class DataType>
bool THeapPtr<DataType>::Create(void)
	{
	if(mObjectRef!=0)
		{
		delete mObjectRef;
		mObjectRef=0;
		}

	mObjectRef=new DataType;
	if(mObjectRef!=0) { return true; }
	return false;
	}


/////////////////////////////////////////
template <class DataType>
bool THeapPtr<DataType>::Destroy(void)
	{
	if(mObjectRef!=0)
		{
		delete mObjectRef;
		mObjectRef=0;
		}

	return true;
	}


//////////////////////////////////////////
template <class DataType>
DataType *THeapPtr<DataType>::Get(void)
	{
	return mObjectRef;
	}


/////////////////////////////////////////
template <class DataType>
DataType & THeapPtr<DataType>::operator*(void)
	{
	MStdAssert(mObjectRef!=0);
	return *mObjectRef;
	}


//////////////////////////////////////////
template <class DataType>
DataType *THeapPtr<DataType>::operator->(void)
	{
	MStdAssert(mObjectRef!=0);
	return mObjectRef;
	}


//////////////////////////////////////////
template <class DataType>
int THeapPtr<DataType>::GetSize(void)
	{
	return sizeof(DataType);
	}


#endif //THeapPtr_h


