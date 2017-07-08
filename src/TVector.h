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


//v2.2 copyright Comine.com 20151212S0010
#ifndef TVector_h
#define TVector_h

////////////////////////////////////////////
#include "MStdLib.h"
#include "MIReader.h"
#include "MIWriter.h"

////////////////////////////////////////////
template <typename Data>
class TVector
	{
	Data *mArray;
	int mCount;

	/////////////////////////////////////////
	void ClearObject(void)
		{
		mArray=0;
		mCount=0;
		}	

	public:
	/////////////////////////////////////////
	TVector(void)
		{  ClearObject();  }

	/////////////////////////////////////////
	explicit TVector(int count)
		{
		ClearObject();
		if(Create(count)==false)
			{
			return;
			}
		}

	/////////////////////////////////////////
	explicit TVector(TVector<Data> &other)
		{
		ClearObject();
		if(Create(other)==false)
			{
			return;
			}
		}

	/////////////////////////////////////////
	explicit TVector(MIReader &reader)
		{
		ClearObject();
		if(Create(reader)==false)
			{
			return;
			}
		}

	/////////////////////////////////////////
	TVector(const Data *data,int len)
		{
		ClearObject();
		if(Create(data,len)==false)
			{
			}
		}

	/////////////////////////////////////////
	~TVector(void)
		{  Destroy();  }

	///////////////////////////////////////
	bool Create(int count)
		{
		Destroy();
		MStdAssert(count>0);
		mCount=count;
		mArray = new(std::nothrow) Data[mCount];
		if(mArray==NULL)
			{
			Destroy();
			return false;
			}

		return true;
		}

	///////////////////////////////////////
	bool Create(const TVector<Data> &other)
		{
		Destroy();
		if(Create(other.GetCount())==false)
			{
			return false;
			}

		// Now Copy the data elements
		const Data *src=other.Get();
		Data *target=mArray;
		int count=other.GetCount();

		int i;
		for(i=0;i<count;++i)
			{
			*target++ = *src++;
			}

		return true;
		}

	
	///////////////////////////////////////
	bool Create(MIReader &reader)
		{
		int count;
		if(reader.Read(count)==false)
			{
			Destroy();
			return false;
			}

		if(Create(count)==false)
			{  return false;  }

		// Now read the array
		int i;
		for(i=0;i<mCount;++i)
			{
			if(MIReaderRead(reader,mArray[i])==false)
				{
				Destroy();
				return false;
				}
			}
		
		return true;
		}

	////////////////////////////////////////
	bool Create(const Data *arr,int count)
		{
		if(Create(count)==false)
			{
			return false;
			}

		int i;
		for(i=0;i<count;++i)
			{
			mArray[i] = arr[i];
			}

		return true;
		}


	////////////////////////////////////////
	bool Destroy(void)
		{
		if(mArray!=0)
			{
			delete[] mArray;
			mArray=0;
			}

		ClearObject();
		return true;
		}


	////////////////////////////////////////
	inline Data *Get(void) const
		{
		return mArray;
		}

	///////////////////////////////////////
	inline int GetCount(void) const
		{
		return mCount;
		}

	////////////////////////////////////////
	// Fast Inline version for speed
	#ifdef NDEBUG
	/////////////////////////////////////////
	inline operator Data *(void)
		{
		return mArray;
		}
	#else
	////////////////////////////////////////
	inline Data &operator[](int index)
		{
		return mArray[index];
		}
	#endif // NDEBUG

	////////////////////////////////////////
	inline Data Get(int index) const
		{
		return mArray[index];
		}

	////////////////////////////////////////
	inline bool Set(int index,const Data &ref)
		{
		mArray[index]=ref;
		return true;
		}

	////////////////////////////////////////
	bool Set(const Data &ref)
		{
		int i;
		for(i=0;i<mCount;++i)
			{
			mArray[i]=ref;
			}

		return true;
		}

	////////////////////////////////////////
	bool Swap(TVector<Data> &ref)
		{
		Data *tmp=mArray;
		mArray=ref.mArray;
		ref.mArray=tmp;

		int tmplength=mCount;
		mCount = ref.mCount;
		ref.mCount = tmplength;
		return true;
		}


	/////////////////////////////////////////
	bool Swap(int index1,int index2)
		{
		Data tmp=mArray[index1];
		mArray[index1]=mArray[index2];
		mArray[index2]=tmp;
		return true;
		}


	/////////////////////////////////////////
	bool Write(MIWriter &writer) const
		{
		if(writer.Write(mCount)==false)
			{
			return false;
			}

		int i;
		for(i=0;i<mCount;++i)
			{
			if(MIWriterWrite(writer,mArray[i])==false)
				{
				return false;
				}
			}

		return true;
		}

	////////////////////////////////////////
	bool Copy(Data *target,int len)
		{
		MStdAssert(mCount<=len && target!=NULL );
		int i;
		for(i=0;i<mCount;++i)
			{
			target[i] = mArray[i];
			}

		return true;
		}

	///////////////////////////////////////
	// Reverse the array contents
	bool Reverse(void)
		{
		int begin,end;
		begin=0;
		end=mCount-1;
		while(begin<end)
			{
			if(Swap(begin,end)==false)
				{
				return false;
				}

			begin = begin + 1;
			end = end - 1;
			}

		return true;
		}


	///////////////////////////////////////
	bool Resize(int newsize)
		{
		MStdAssert(newsize>0);
		Data *newarray;
		newarray = new(std::nothrow) Data[newsize];
		if(newarray==NULL)
			{
			return false;
			}
		
		// Copy Old Data to new Memory
		const int mincopylength=MStdGetMin(newsize,mCount);
		int i;
		for(i=0;i<mincopylength;++i)
			{
			// Copy Constructor
			newarray[i] = mArray[i];
			}

		// Release old objects and memory
		delete[] mArray;

		// Point to new array
		mArray=newarray;
		mCount=newsize;

		return true;
		}
	};

#endif //TVector_h

