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


//v2.0 copyright Comine.com 20150810M1103
#ifndef TAutoPtr_h
#define TAutoPtr_h

///////////////////////////////////////////
#include "MStdLib.h"

//******************************************************
//**  TAutoPtr Template
//******************************************************
template<class RefObject>
class TAutoPtr
	{
	////////////////////////////////////////
	RefObject *mRefObject;

	////////////////////////////////////////
	void ClearObject(void)
		{
		mRefObject=NULL;
		}

	////////////////////////////////////////
	TAutoPtr(const TAutoPtr &ref)
		{
		}

	////////////////////////////////////////
	public:
	TAutoPtr(bool create=false)
		{
		ClearObject();
		if(create==true && Create()==false)
			{
			return;
			}
		}

	///////////////////////////////////////
	~TAutoPtr(void)
		{
		Destroy();
		}

	///////////////////////////////////////
	bool Create(void)
		{
		// Allocate heap object
		RefObject *newobj=new RefObject;
		if(newobj==NULL)
			{
			Destroy();
			return false;
			}

		// Destroy exisiting reference
		Destroy();
		
		mRefObject=newobj;
		return true;
		}


	////////////////////////////////////////
	bool Destroy(void)
		{
		if(mRefObject!=NULL)
			{
			delete mRefObject;
			mRefObject=NULL;
			}

		return true;
		}


	/////////////////////////////////////////
	RefObject *operator ->(void)
		{
		return mRefObject;
		}

	/////////////////////////////////////////
	RefObject& operator *(void)
		{
		return *mRefObject;
		}


	/////////////////////////////////////////
	RefObject *Get(void)
		{
		// Return internal pointer
		return mRefObject;
		}

	/////////////////////////////////////////
	bool Swap(TAutoPtr<RefObject> &ref)
		{
		// Swap Referenced Memory
		RefObject *tmpobj=mRefObject;
		mRefObject = ref.mRefObject;
		ref.mRefObject=tmpobj;

		return true;
		}

	//////////////////////////////////////////
	bool operator=(TAutoPtr<RefObject> &ref)
		{
		if(ref.Get()==NULL)
			{
			Destroy();
			return true;
			}
		
		if(Create()==false)
			{
			return false;
			}

		// Assume Copy Sematics for type
		(*mRefObject) = (* ref.mRefObject);

		return true;
		}
	};

#endif // TAutoPtr_h

