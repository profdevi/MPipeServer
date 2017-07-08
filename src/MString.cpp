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


//v3.2 copyright Comine.com 20170409U0957
#include "MStdLib.h"
#include "TVector.h"
#include "MIReader.h"
#include "MIWriter.h"
#include "MString.h"


//************************************************************************
//  MString Definition
//************************************************************************
void MString::ClearObject(void)
	{
	mLength=0;
	}


////////////////////////////////////////////////
MString::MString(void)
	{  ClearObject();  }


///////////////////////////////////////////////
MString::MString(const char *str)
	{
	ClearObject();
	if(Create(str)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MString::MString(const wchar_t *str)
	{
	ClearObject();
	if(Create(str)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MString::MString(const MString &str)
	{
	ClearObject();
	if(Create(str.mString.Get() )==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MString::MString(MStringWide &str)
	{
	ClearObject();
	if(Create(str.Get() )==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MString::~MString(void)
	{  Destroy();  }


///////////////////////////////////////////////
bool MString::Create(const char *str)
	{
	Destroy();
	if(str==0)
		{
		Destroy();
		return false; 
		}

	mLength=MStdStrLen(str);
	if(mString.Create(mLength+1)==false)
		{
		Destroy();
		return false;
		}

	MString::Copy(mString.Get(),str);
	return true;
	}


///////////////////////////////////////////////
bool MString::Create(const wchar_t *str)
	{
	Destroy();

	if(str==NULL)
		{
		Destroy();
		return false;
		}


	mLength=MString::Length(str);
	if(mString.Create(mLength+1)==false)
		{
		Destroy();
		return false;
		}

	MString::Copy(mString.Get(),str);
	return true;
	}


///////////////////////////////////////////////
bool MString::Create(const MString &str)
	{
	return Create(str.mString.Get());
	}


///////////////////////////////////////////////
bool MString::Create(MStringWide &str)
	{
	return Create(str.Get());
	}


///////////////////////////////////////////////
bool MString::Create(char ch)
	{
	Destroy();

	if(mString.Create(2)==false)
		{
		Destroy();
		return false;
		}

	mString[0]=ch;
	mString[1]=0;

	if(ch!=0)
		{
		mLength=1;
		}
	else
		{
		mLength=0;
		}

	return true;	
	}


///////////////////////////////////////////////
bool MString::Create(wchar_t ch)
	{
	Destroy();

	if(mString.Create(2)==false)
		{
		Destroy();  return false;
		}

	mString[0]=(char)ch;
	mString[1]=0;

	if(ch!=0)
		{
		mLength=1;
		}
	else
		{
		mLength=0;
		}

	return true;		
	}


///////////////////////////////////////////////
bool MString::Create(MIReader &reader)
	{
	Destroy();

	int length;
	if(reader.Read(length)==false)
		{
		Destroy();
		return false;
		}

	MStdAssert(mLength>=0);

	if(mString.Create(length+1)==false)
		{
		Destroy();
		return false;
		}

	if(reader.Read((void *)mString.Get(),length+1)==false)
		{
		Destroy();
		return false;
		}

	mLength=length;
	return true;
	}


///////////////////////////////////////////////
bool MString::Destroy(void)
	{
	mString.Destroy();
	ClearObject();
	return true;
	}


///////////////////////////////////////////////
bool MString::operator=(const char *str)
	{
	return Create(str);
	}


///////////////////////////////////////////////
bool MString::operator=(const wchar_t *str)
	{
	return Create(str);
	}


///////////////////////////////////////////////
bool MString::operator=(MString &src)
	{
	return Create(src);
	}


///////////////////////////////////////////////
MString MString::operator +(const char *str) // return an temp object
	{
	MString retobj;
	if(str==NULL)
		{
		return retobj;
		}

	if(retobj.Create(mString.Get())==false)
		{
		return retobj;
		}

	if(retobj.Append(str)==false)
		{
		return retobj;
		}

	return retobj;
	}


///////////////////////////////////////////////
MString MString::operator +(MString &ref)	// return a temp object
	{
	MString retobj;
	if(retobj.Create(mString.Get())==false)
		{
		return retobj;
		}

	if(retobj.Append(ref.Get())==false)
		{
		return retobj;
		}

	return retobj;
	}


///////////////////////////////////////////////
MString MString::operator +(MStringWide &ref)	// return a temp object
	{
	MString retobj;
	if(retobj.Create(mString.Get())==false)
		{
		return retobj;
		}

	if(retobj.Append(ref.Get())==false)
		{
		return retobj;
		}

	return retobj;
	}


///////////////////////////////////////////////
char &MString::operator[](int index)
	{
	MStdAssert(index>=0 && index<=mLength);
	return mString[index];
	}


///////////////////////////////////////////////
const char *MString::Get(void)
	{
	return mString.Get();
	}


////////////////////////////////////////////////
const char *MString::GetEnd(int count)
	{
	if(count<0) { return "";  }
	if(count>mLength) { return "";  }
	return mString.Get()+mLength-count;
	}


///////////////////////////////////////////////
bool MString::ToUpper(void)
	{
	return MString::ToUpper(mString.Get());
	}


///////////////////////////////////////////////
bool MString::ToLower(void)
	{
	return MString::ToLower(mString.Get());
	}


///////////////////////////////////////////////
bool MString::Trim(void)
	{
	if(MString::Trim(mString.Get() )==false)
		{
		return false;
		}

	mLength=MString::Length(mString.Get() );
	return true;
	}


///////////////////////////////////////////////
bool MString::TrimLeft(void)
	{
	if(MString::TrimLeft(mString.Get() )==false)
		{
		return false;
		}

	mLength=MString::Length(mString.Get() );
	return true;
	}


///////////////////////////////////////////////
bool MString::TrimRight(void)
	{
	if(MString::TrimRight(mString.Get() )==false)
		{
		return false;
		}

	mLength=MString::Length(mString.Get() );
	return true;
	}


//////////////////////////////////////////////
bool MString::TrimRight(int length)
	{
	if(length<0)
		{
		return false;
		}

	if(length>mLength)
		{
		return false;
		}

	//= We Will Keep the original buffer
	mLength-=length;
	mString[mLength]=0;

	return true;
	}


///////////////////////////////////////////////
bool MString::Append(const char *str)
	{
	if(str==NULL)
		{
		return false; 
		}

	int newlength=MString::Length(str)+MString::Length(mString.Get() );
	TVector<char> newspace;
	if(newspace.Create(newlength+1)==false)
		{
		return false;
		}

	MString::Copy(newspace.Get(),mString.Get());
	MString::Append(newspace.Get(),str);

	if(mString.Swap(newspace)==false)
		{
		return false;
		}

	mLength=newlength;

	return true;
	}


///////////////////////////////////////////////
bool MString::Append(const wchar_t *str)
	{
	if(str==0)
		{
		return false; 
		}

	int length=MString::Length(str)+MString::Length(mString.Get() );
	TVector<char> data;
	if(data.Create(length+1)==false)
		{
		return false;
		}

	MString::Copy(data.Get(),mString.Get() );
	MString::Append(data.Get(),str);

	mString.Swap(data);
	mLength=length+1;

	return true;
	}


///////////////////////////////////////////////
bool MString::Append(char ch)
	{
	char buf[2]={ch,0};
	return Append(buf);
	}


///////////////////////////////////////////////
bool MString::Append(wchar_t ch)
	{
	wchar_t buf[2]={ch,0};
	return Append(buf);
	}


///////////////////////////////////////////////
bool MString::Append(int val)
	{
	char buf[20];
	MStdSPrintf(buf,sizeof(buf)-1,"%d",val);
	return Append(buf);
	}


///////////////////////////////////////////////
bool MString::Append(float val)
	{
	char buf[20];
	MStdSPrintf(buf,sizeof(buf)-1,"%f",val);
	return Append(buf);
	}


///////////////////////////////////////////////
bool MString::Append(double val)
	{
	char buf[20];
	MStdSPrintf(buf,sizeof(buf)-1,"%lf",val);
	return Append(buf);
	}


///////////////////////////////////////////////
bool MString::Append(bool val)
	{
	if(val==true)
		{  return Append("true");  }
	else
		{  return Append("false");  }
	}


///////////////////////////////////////////////
int MString::Length(void)
	{
	return mLength;
	}


///////////////////////////////////////////////
int MString::Compare(const char *string)
	{
	return MString::Compare(mString.Get(),string);
	}


///////////////////////////////////////////////
int MString::Compare(const wchar_t *string)
	{
	return MString::Compare(mString.Get() ,string);
	}


///////////////////////////////////////////////
int MString::Compare(MString &string)
	{
	return MString::Compare(mString.Get() ,string.Get() );
	}


///////////////////////////////////////////////
int MString::Compare(MStringWide &string)
	{
	return MString::Compare(mString.Get(),string.Get());
	}

///////////////////////////////////////////////
bool MString::Write(MIWriter &writer) const
	{
	if(writer.Write(mLength)==false)
		{
		return false;
		}

	// Now Save the string length
	if(writer.Write((void *)mString.Get(),mLength+1)==false)
		{
		return false;
		}

	return true;
	}

/////////////////////////////////////////////////////////////
bool MString::Zero(void)
	{
	const int count=mString.GetCount();
	for(int i=0;i<count;++i)
		{
		mString[i]=0;
		}

	mLength=0;
	return true;
	}


//************************************************************************
//  MString Static Methods
//************************************************************************
bool MString::Copy(char *dest,const char *src)
	{	
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	char *p; const char *q;
	p=dest; q=src;
	while((*p++=*q++)!=0) { }

	return true;
	}


////////////////////////////////////////////////
bool MString::Copy(char *dest,const wchar_t *src)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	char *p;  const wchar_t *q;
	p=dest; q=src;
	while((*p++=(char)*q++)!=0) { }

	return true;
	}


////////////////////////////////////////////////
bool MString::Copy(wchar_t *dest,const wchar_t *src)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	wchar_t *p;  const wchar_t *q;
	p=dest; q=src;
	while((*p++=*q++)!=0) { }

	return true;
	}


///////////////////////////////////////////////
bool MString::Copy(wchar_t *dest,const char *src)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	wchar_t *p; const char *q;
	p=dest; q=src;
	while((*p++=*q++)!=0) { }

	return true;
	}


////////////////////////////////////////////////
bool MString::Copy(char *dest,const char *src,int maxcopy)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	char *p; const char *q;
	p=dest; q=src;

	int i;
	for(i=0;(*p++=*q++)!=0;++i)
		{
		if(i+1==maxcopy)
			{
			*p=0;
			break;
			}
		}

	return true;
	}


////////////////////////////////////////////////
bool MString::Copy(char *dest,const wchar_t *src,int maxcopy)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	char *p;  const wchar_t *q;
	p=dest; q=src;

	int i;
	for(i=0;(*p++=(char)*q++)!=0;++i)
		{
		if(i+1==maxcopy)
			{
			*p=0;
			break;
			}
		}

	return true;
	}


////////////////////////////////////////////////
bool MString::Copy(wchar_t *dest,const char *src,int maxcopy)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	wchar_t *p;  const char *q;
	p=dest; q=src;

	int i;
	for(i=0;(*p++=*q++)!=0;++i)
		{
		if(i+1==maxcopy)
			{
			*p=0;
			break;
			}
		}

	return true;	
	}


////////////////////////////////////////////////
bool MString::Copy(wchar_t *dest,const wchar_t *src,int maxcopy)
	{
	if(dest==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	wchar_t *p;  const wchar_t *q;
	p=dest; q=src;

	int i;
	for(i=0;(*p++=*q++)!=0;++i)
		{
		if(i+1==maxcopy)
			{
			*p=0;
			break;
			}
		}

	return true;
	}


///////////////////////////////////////////////
int MString::Length(const char *str)
	{
	int length;
	const char *p; 
	length=0;

	for(p=str;*p!=0;++p)
		{ length+=1; }

	return length;
	}


//////////////////////////////////////////////
int MString::Length(const wchar_t *str)
	{
	int length;
	const wchar_t *p; 
	length=0;

	for(p=str;*p!=0;++p)
		{ length+=1; }

	return length;
	}


//////////////////////////////////////////////
int MString::Compare(const char *src,const char *dst)
	{
	const char *p;
	const char *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(*p!=*q)
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


////////////////////////////////////////////////	
int MString::Compare(const wchar_t *src,const char *dst)
	{
	const wchar_t *p;
	const char *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(*p!=*q)
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


/////////////////////////////////////////////////
int MString::Compare(const char *src,const wchar_t *dst)
	{
	const char *p;
	const wchar_t *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(*p!=*q)
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


//////////////////////////////////////////////////
int MString::Compare(const wchar_t *src,const wchar_t *dst)
	{
	const wchar_t *p;
	const wchar_t *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(*p!=*q)
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


//////////////////////////////////////////////
int MString::ICompare(const char *src,const char *dst)
	{
	const char *p;
	const char *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(MString::ToUpper(*p)!=MString::ToUpper(*q))
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


////////////////////////////////////////////////	
int MString::ICompare(const wchar_t *src,const char *dst)
	{
	const wchar_t *p;
	const char *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(MString::ToUpper(*p)!=MString::ToUpper(*q))
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


/////////////////////////////////////////////////
int MString::ICompare(const char *src,const wchar_t *dst)
	{
	const char *p;
	const wchar_t *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(MString::ToUpper(*p)!=MString::ToUpper(*q))
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


//////////////////////////////////////////////////
int MString::ICompare(const wchar_t *src,const wchar_t *dst)
	{
	const wchar_t *p;
	const wchar_t *q;
	for(p=src,q=dst; ;++p,++q )
		{
		if(MString::ToUpper(*p)!=MString::ToUpper(*q))
			{ return *p-*q; }
		else if(*p==0)
			{ return 0; }
		}
	}


///////////////////////////////////////////////////
bool MString::IsSpace(char ch)  // for ' ',/r/n/t
	{
	if(ch==' ' || ch=='\r' || ch=='\t'  || ch=='\n')
		{ return true; }

	return false;
	}


//////////////////////////////////////////////////
bool MString::IsSpace(wchar_t ch)
	{
	if(ch==' ' || ch=='\r' || ch=='\t'  || ch=='\n')
		{ return true; }

	return false;
	}


//////////////////////////////////////////////////
char MString::ToUpper(char ch)
	{
	if(ch>='a' && ch<='z')
		{ ch+='A'-'a'; }

	return ch;
	}


//////////////////////////////////////////////////
char MString::ToLower(char ch)
	{
	if(ch>='A' && ch<='Z')
		{ ch+='a'-'Z'; }

	return ch;
	}


/////////////////////////////////////////////////
wchar_t MString::ToUpper(wchar_t ch)
	{
	if(ch>='a' && ch<='z')
		{ ch+=(wchar_t)('A'-'a'); }

	return ch;
	}


////////////////////////////////////////////////
wchar_t MString::ToLower(wchar_t ch)
	{
	if(ch>='A' && ch<='Z')
		{ ch+=(wchar_t)('a'-'Z'); }

	return ch;
	}


//////////////////////////////////////////////////
bool MString::ToLower(char *str)
	{
	if(str==NULL)
		{
		return false;
		}

	char *p;
	for(p=str;*p!=0;++p)
		{
		if(*p>='A' && *p<='Z')
			{ *p += 'a'-'A'; }
		}

	return true;
	}


//////////////////////////////////////////////////
bool MString::ToLower(wchar_t *str)
	{
	if(str==NULL)
		{
		return false;
		}

	wchar_t *p;
	for(p=str;*p!=0;++p)
		{
		if(*p>='A' && *p<='Z')
			{ *p += 'a'-'A'; }
		}

	return true;
	}

//////////////////////////////////////////////////
bool MString::ToUpper(char *str)
	{
	if(str==NULL)
		{
		return false;
		}

	char *p;
	for(p=str;*p!=0;++p)
		{
		if(*p>='a' && *p<='z')
			{ *p += 'A'-'a'; }
		}

	return true;
	}


//////////////////////////////////////////////////
bool MString::ToUpper(wchar_t *str)
	{
	if(str==NULL)
		{
		return false;
		}

	wchar_t *p;
	for(p=str;*p!=0;++p)
		{
		if(*p>='a' && *p<='z')
			{ *p +=(wchar_t)('A'-'a'); }
		}

	return true;
	}

///////////////////////////////////////////////
bool MString::Trim(char *str)  // Remove All spaces
	{
	char *src;
	char *dst;
	if(str==NULL)
		{
		return false;
		}

	for(src=dst=str;*src!=0;++src)
		{
		if(MString::IsSpace(*src)==true)
			{ continue; }

		*dst++=*src;
		}

	*dst=0;
	return true;
	}


///////////////////////////////////////////////
bool MString::Trim(wchar_t *str)  // Remove All spaces
	{
	wchar_t *src;
	wchar_t *dst;
	if(str==NULL)
		{
		return false;
		}

	for(src=dst=str;*src!=0;++src)
		{
		if(MString::IsSpace(*src)==true)
			{ continue; }

		*dst++=*src;
		}
	*dst=0;
	return true;
	}


/////////////////////////////////////////////////
bool MString::TrimLeft(char *string)
	{
	char *src;
	char *dst;
	for(src=string;*src!=0;++src)
		{
		if(MString::IsSpace(*src)==false)
			{ break; }
		}

	// Copy String
	for(dst=string; ;++dst)
		{
		if((*dst=*src)==0)
			{ break; }

		src=src+1;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MString::TrimRight(char *string)
	{
	char *src;
	for(src=string;*src!=0;++src) { } // Find End

	for(src=src-1; src>=string ;--src)
		{
		if(MString::IsSpace(*src)==false)
			{ break; }

		*src=0;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MString::TrimLeft(wchar_t *string)
	{
	wchar_t *src;
	wchar_t *dst;
	for(src=string;*src!=0;++src)
		{
		if(MString::IsSpace(*src)==false)
			{ break; }
		}

	// Copy String
	for(dst=string; ;++dst)
		{
		if((*dst=*src)==0)
			{ break; }

		src=src+1;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MString::TrimRight(wchar_t *string)
	{
	wchar_t *src;
	for(src=string;*src!=0;++src) { } // Find End

	for(src=src-1; src>=string ;--src)
		{
		if(MString::IsSpace(*src)==false)
			{ break; }

		*src=0;
		}

	return true;
	}


///////////////////////////////////////////////////
bool MString::Append(char *str,const char *src)
	{
	if(str==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	while(*str!=0)
		{  str=str+1; }

	return MString::Copy(str,src);
	}


///////////////////////////////////////////////////
bool MString::Append(char *str,const wchar_t *src)
	{
	if(str==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	while(*str!=0)
		{  str=str+1; }

	return MString::Copy(str,src);
	}


////////////////////////////////////////////////////
bool MString::Append(wchar_t *str,const char *src)
	{
	if(str==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	while(*str!=0)
		{  str=str+1; }

	return MString::Copy(str,src);
	}


/////////////////////////////////////////////////////
bool MString::Append(wchar_t *str,const wchar_t *src)
	{
	if(str==NULL)
		{
		return false;
		}

	if(src==NULL)
		{
		return false;
		}

	while(*str!=0)
		{  str=str+1; }

	return MString::Copy(str,src);
	}


/////////////////////////////////////////////////
char *MString::Alloc(const char *st,const char *file,int line)
	{
	if(st==NULL)
		{
		return NULL;
		}

	int length=MString::Length(st);
	char *newstr;

	newstr=new(std::nothrow) char[length+1];
	if(newstr==NULL)
		{
		return NULL;
		}

	MString::Copy(newstr,st);
	return newstr;
	}


////////////////////////////////////////////////
char *MString::Alloc(const char *st,const char *st2,const char *file,int line)
	{
	if(st==NULL)
		{
		return NULL;
		}

	if(st2==NULL)
		{
		return NULL;
		}

	int length=MString::Length(st)+MString::Length(st2);
	char *newstr;
	newstr=new(std::nothrow) char[length+1];
	if(newstr==NULL)
		{
		return NULL;
		}

	MString::Copy(newstr,st);
	MString::Append(newstr,st2);
	return newstr;
	}



//************************************************
//  MStringWide Definition
//************************************************
void MStringWide::ClearObject(void)
	{
	mLength=0;
	}


///////////////////////////////////////////////
MStringWide::MStringWide(void)
	{  ClearObject();  }


///////////////////////////////////////////////
MStringWide::MStringWide(const char *str)
	{
	ClearObject();
	if(Create(str)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MStringWide::MStringWide(const wchar_t *str)
	{
	ClearObject();
	if(Create(str)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MStringWide::MStringWide(MStringWide &str)
	{
	ClearObject();
	if(Create(str)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MStringWide::MStringWide(MString &str)
	{
	ClearObject();
	if(Create(str.Get() )==false)
		{
		return;
		}
	}


///////////////////////////////////////////////
MStringWide::~MStringWide(void)
	{  Destroy();  }


///////////////////////////////////////////////
bool MStringWide::Create(const char *str)
	{
	Destroy();
	if(str==0)
		{
		return false;
		}

	int length=MString::Length(str);
	if(mString.Create(length+1)==false)
		{
		return false;
		}

	MString::Copy(mString.Get(),str);
	mLength=length;

	return true;
	}


///////////////////////////////////////////////
bool MStringWide::Create(const wchar_t *str)
	{
	Destroy();
	if(str==NULL)
		{
		return false;
		}

	int length=MString::Length(str);	
	if(mString.Create(length+1)==false)
		{
		Destroy();
		return false;
		}

	MString::Copy(mString.Get(),str);
	mLength=length;

	return true;
	}


///////////////////////////////////////////////
bool MStringWide::Create(MString &str)
	{  return Create(str.Get());  }


///////////////////////////////////////////////
bool MStringWide::Create(MStringWide &str)
	{  return Create(str.Get() );  }


////////////////////////////////////////////////
bool MStringWide::Create(char ch)
	{
	Destroy();
	if(mString.Create(2)==false)
		{
		Destroy();
		return false;
		}

	mString[0]=ch;
	mString[1]=0;
	mLength=1;

	return true;
	}


////////////////////////////////////////////////
bool MStringWide::Create(wchar_t ch)
	{
	Destroy();

	if(mString.Create(2)==false)
		{
		Destroy();
		return false;
		}

	mString[0]=ch;
	mString[1]=0;
	mLength=1;

	return true;
	}


////////////////////////////////////////////////
bool MStringWide::Create(MIReader &reader)
	{
	Destroy();

	int length;
	if(reader.Read(length)==false)
		{
		Destroy();
		return false;
		}

	MStdAssert(mLength>=0);

	if(mString.Create(length+1)==false)
		{
		Destroy();
		return false;
		}

	if(reader.Read((void *)mString.Get(),sizeof(wchar_t)*(length+1) )==false)
		{
		Destroy();
		return false;
		}

	mLength=length;
	return true;
	}



///////////////////////////////////////////////
bool MStringWide::Destroy(void)
	{
	mString.Destroy();
	ClearObject();
	return true;
	}


///////////////////////////////////////////////
bool MStringWide::operator=(const char *str)
	{  return Create(str);  }


///////////////////////////////////////////////
bool MStringWide::operator=(const wchar_t *str)
	{  return Create(str);  }


///////////////////////////////////////////////
bool MStringWide::operator=(MStringWide &src)
	{  return Create(src.mString.Get());  }


///////////////////////////////////////////////
wchar_t &MStringWide::operator[](int index)
	{
	MStdAssert(index>=0 && index<=mLength);
	return mString[index];
	}


///////////////////////////////////////////////
const wchar_t *MStringWide::Get(void)
	{  return mString.Get();  }


///////////////////////////////////////////////
const wchar_t *MStringWide::GetEnd(int count)
	{
	if(count<0) { return L"";  }
	if(count>mLength) { return L"";  }
	return mString.Get()+mLength-count;	
	}


///////////////////////////////////////////////
bool MStringWide::ToUpper(void)
	{
	return MString::ToUpper(mString.Get());
	}


///////////////////////////////////////////////
bool MStringWide::ToLower(void)
	{
	return MString::ToLower(mString.Get() );
	}


///////////////////////////////////////////////
bool MStringWide::Trim(void)
	{
	if(MString::Trim(mString.Get() )==false)
		{
		return false;
		}

	mLength=MString::Length(mString.Get() );
	return true;
	}


///////////////////////////////////////////////
bool MStringWide::TrimLeft(void)
	{
	if(MString::TrimLeft(mString.Get() )==false)
		{
		return false;
		}

	mLength=MString::Length(mString.Get() );
	return true;
	}


///////////////////////////////////////////////
bool MStringWide::TrimRight(void)
	{
	if(MString::TrimRight(mString.Get() )==false)
		{
		return false;
		}

	mLength=MString::Length(mString.Get() );
	return true;
	}


///////////////////////////////////////////////
bool MStringWide::TrimRight(int length)
	{
	if(length<0)
		{
		return false;
		}

	if(length>mLength)
		{
		return false;
		}

	//= We Will Keep the original buffer
	mLength-=length;
	mString[mLength]=0;

	return true;	
	}


///////////////////////////////////////////////
bool MStringWide::Append(const wchar_t *str)
	{
	if(str==0)
		{
		return false; 
		}

	int newlength=MString::Length(str)+MString::Length(mString.Get() );
	TVector<wchar_t> newstr;
	if(newstr.Create(newlength+1)==false)
		{
		return false;
		}

	MString::Copy(newstr.Get(),mString.Get());
	MString::Append(newstr.Get(),str);

	mString.Swap(newstr);
	mLength=newlength;

	return true;
	}


///////////////////////////////////////////////
bool MStringWide::Append(const char *str)
	{
	if(str==0)
		{
		return false; 
		}

	int newlength=MString::Length(str)+MString::Length(mString.Get());
	TVector<wchar_t> newstr;
	if(newstr.Create(newlength+1)==false)
		{
		return false;
		}

	MString::Copy(newstr.Get(),mString.Get() );
	MString::Append(newstr.Get(),str);

	mString.Swap(newstr);
	return true;
	}


///////////////////////////////////////////////
bool MStringWide::Append(int val)
	{
	char buf[20];
	MStdSPrintf(buf,sizeof(buf)-1,"%d",val);
	return Append(buf);
	}


///////////////////////////////////////////////
bool MStringWide::Append(float val)
	{
	char buf[20];
	MStdSPrintf(buf,sizeof(buf)-1,"%f",val);
	return Append(buf);
	}


///////////////////////////////////////////////
bool MStringWide::Append(double val)
	{
	char buf[20];
	MStdSPrintf(buf,sizeof(buf)-1,"%lf",val);
	return Append(buf);
	}


///////////////////////////////////////////////
bool MStringWide::Append(bool val)
	{
	if(val==true)
		{  return Append("true");  }
	else
		{  return Append("false");  }
	}


///////////////////////////////////////////////
int MStringWide::Length(void)
	{
	return mLength;
	}


///////////////////////////////////////////////
int MStringWide::Compare(const char *string)
	{
	return MString::Compare(mString.Get(),string);
	}


///////////////////////////////////////////////
int MStringWide::Compare(const wchar_t *string)
	{
	return MString::Compare(mString.Get(),string);
	}


///////////////////////////////////////////////
int MStringWide::Compare(MString &string)
	{
	return MString::Compare(mString.Get(),string.Get() );
	}


///////////////////////////////////////////////
int MStringWide::Compare(MStringWide &string)
	{
	return MString::Compare(mString.Get(),string.Get());
	}


////////////////////////////////////////////////
bool MStringWide::Write(MIWriter &writer) const
	{
	if(writer.Write(mLength)==false)
		{
		return false;
		}

	// Now Save the string length
	if(writer.Write((void *)mString.Get(),sizeof(wchar_t)*(mLength+1) )==false)
		{
		return false;
		}

	return true;
	}

