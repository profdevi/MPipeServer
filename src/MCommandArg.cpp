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


//v2.3 copyright Comine.com 20170412W1700
#include "MStdLib.h"
#include "MString.h"
#include "MCommandParser.h"
#include "MCommandArg.h"


//*****************************************************************
//** Module Elements
//*****************************************************************
static const int MAXARGUMENTS=1024;
static const char SEPERATOR[]=" \r\n\t";

/////////////////////////////////////////////////////
static char GToUpper(char ch)
	{
	if(ch>='a' && ch<='z') { ch += 'A'-'a'; }
	return ch;
	}



/////////////////////////////////////////////////////////
static int GMemICompare(const char *srcone,const char *srctwo,int len)
	{	int i;
	for(i=0;i<len;++i)
		{
		char ch1=srcone[i];  
		char ch2=srctwo[i];
		
		if(ch1>='a' && ch1<='z') { ch1 += 'A' - 'a';  }
		if(ch2>='a' && ch2<='z') { ch2 += 'A' - 'a'; }
		if(ch1!=ch2) { return ch1-ch2; }
		}
	return 0;
	}

/////////////////////////////////////////////////////////
static int GMemCompare(const char *srcone,const char *srctwo,int len)
	{	int i;
	for(i=0;i<len;++i)
		{
		if(srcone[i]!=srctwo[i]) { return srcone[i]-srctwo[i]; }
		}
	return 0;
	}


//*****************************************************************
//** MCommandArg
//*****************************************************************
void MCommandArg::ClearObject(void)
	{
	mArg=0;
	mArgv=0;
	mBlock=0;
	mStartIndex=0;
	mUnitTestOutput=false;
	}


///////////////////////////////////////
MCommandArg::MCommandArg(void)
	{  ClearObject();  }


///////////////////////////////////////
MCommandArg::MCommandArg(int arg,const char *argv[])
	{ 
	ClearObject();
	if(Create(arg,argv)==false) {  return;  }
	}


///////////////////////////////////////
MCommandArg::MCommandArg(int arg,char **argv)
	{
	ClearObject();
	if(Create(arg,argv)==false) {  return; }
	}


///////////////////////////////////////
MCommandArg::MCommandArg(const char *string)
	{
	ClearObject();
	if(Create(string)==false){  return;  }
	}


//////////////////////////////////////
MCommandArg::~MCommandArg(void)
	{  Destroy(); }


/////////////////////////////////////
bool MCommandArg::Create(int arg,const char *argv[])
	{
	int i;
	Destroy();

	// Mark output as visible
	mUnitTestOutput=true;

	mArg=arg;
	mStartIndex=1;

	if(mArg==0)
		{
		return true;
		}

	int totalspace=0;
	for(i=0;i<mArg;++i)
		{
		if(argv[i]==0) { continue; }
		totalspace += MStdStrLen(argv[i])+1;
		}

	//////////////////////////////////////////
	mBlock=new(std::nothrow) char[totalspace];
	if(mBlock==0)
		{
		Destroy();
		return false;
		}

	/////////////////////////////////////////
	mArgv=new(std::nothrow) char *[mArg];
	if(mArgv==0)
		{
		Destroy();
		return false;
		}

	/////////////////////////////////////////
	int pos=0;
	for(i=0;i<mArg;++i)
		{
		if(argv[i]==0) { continue; }
		MString::Copy(mBlock+pos,argv[i]);
		mArgv[i]=mBlock+pos;
		pos+=MStdStrLen(argv[i])+1;
		}

	return true;
	}


///////////////////////////////////////
bool MCommandArg::Create(int arg,char **argv)
	{
	int i;
	Destroy();

	// Mark output as visible
	mUnitTestOutput=true;

	mArg=arg; mStartIndex=1;

	if(mArg==0)
		{
		return true;
		}

	int totalspace=0;
	for(i=0;i<mArg;++i)
		{
		if(argv[i]==0) { continue; }
		totalspace += MStdStrLen(argv[i])+1;
		}

	//////////////////////////////////////////
	mBlock=new(std::nothrow) char[totalspace];
	if(mBlock==0)
		{
		Destroy();
		return false;
		}

	/////////////////////////////////////////
	mArgv=new(std::nothrow) char *[mArg];
	if(mArgv==0)
		{
		Destroy();
		return false;
		}

	/////////////////////////////////////////
	int pos=0;
	for(i=0;i<mArg;++i)
		{
		if(argv[i]==0) { continue; }
		MString::Copy(mBlock+pos,argv[i]);
		mArgv[i]=mBlock+pos;
		pos+=MStdStrLen(argv[i])+1;
		}

	return true;
	}


//////////////////////////////////////
bool MCommandArg::Create(const char *arguments)
	{
	Destroy();
	
	// Mark output as visible
	mUnitTestOutput=true;

	MCommandParser parser;
	if(parser.Create(SEPERATOR)==false)
		{
		Destroy();
		return false;
		}

	if(parser.ParseString(arguments)==false)
		{
		return false;
		}

	char **tmpargs;
	const int parsergetcount=parser.GetCount();
	tmpargs=new(std::nothrow) char *[parsergetcount+1];
	if(tmpargs==NULL)
		{
		return false;
		}

	int i;
	for(i=0;i<parser.GetCount();++i)
		{
		const int arglength=MStdStrLen(parser.GetString(i))+1;
		tmpargs[i]=new(std::nothrow) char[arglength];
		if(tmpargs[i]==NULL)
			{
			while(--i>=0)
				{
				delete[] tmpargs[i];
				}

			delete[] tmpargs;
			return false;
			}

		// Copy the string into newly allocated memory block
		MStdStrCpy(tmpargs[i],parser.GetString(i) );
		}

	bool ret=Create(parser.GetCount() ,(const char **)tmpargs);
	mStartIndex=0; 
	for(i=0;i<parser.GetCount();++i)
		{
		delete[] tmpargs[i];
		tmpargs[i]=NULL;
		}

	delete[] tmpargs;
	return ret;
	}


/////////////////////////////////////
bool MCommandArg::Destroy(void)
	{
	if(mBlock!=0)
		{
		delete[] mBlock;
		mBlock=0;
		}

	if(mArgv!=0)
		{
		delete[] mArgv;
		mArgv=0;
		}

	ClearObject();
	return true;
	}


///////////////////////////////////////
bool MCommandArg::CheckArgHelp(void)
	{
	/////////////////////////////////
	if(mArg<0)
		{
		return false;
		}

	bool ret=false;
	for(int i=mArg-1;i>=mStartIndex;--i)
		{
		if(mArgv[i]==0)
			{
			// Do Nothing
			}
		else if( MString::ICompare(mArgv[i],"-arghelp")==0 )
			{
			RemoveArg(i);
			ret=true;
			}
		}

	return ret;
	}


///////////////////////////////////////
bool MCommandArg::CheckRemoveHelp(void)
	{
	/////////////////////////////////
	if(mArg<0)
		{
		return false;
		}

	bool ret=false;
	for(int i=mArg-1;i>=mStartIndex;--i)
		{
		if(mArgv[i]==0)
			{
			// Do Nothing
			}
		else if(MString::ICompare(mArgv[i],"/?")==0
				|| MString::ICompare(mArgv[i],"-?")==0
				|| MString::ICompare(mArgv[i],"-h")==0
				|| MString::ICompare(mArgv[i],"/h")==0
				|| MString::ICompare(mArgv[i],"-help")==0
				|| MString::ICompare(mArgv[i],"/help")==0
				|| MString::ICompare(mArgv[i],"?")==0
				|| MString::ICompare(mArgv[i],"--help")==0 )
			{
			RemoveArg(i);
			ret=true;
			}
		}

	return ret;
	}


///////////////////////////////////////
bool MCommandArg::RemoveArg(int index)
	{
	if(index<0 || index>=mArg)
		{
		return false;
		}

	for(int p=index;p<mArg-1;++p)
		{ mArgv[p]=mArgv[p+1]; }

	if(mArg>0) { --mArg; }
	return true;
	}


///////////////////////////////////////
int MCommandArg::GetArgCount(void)
	{
	return mArg;
	}


//////////////////////////////////////
const char *MCommandArg::GetArg(int arg)
	{
	if(arg<0 || arg>=mArg)
		{
		return "";
		}

	return mArgv[arg];
	}


//////////////////////////////////////
bool MCommandArg::IsArgument(const char *argvalue,bool casesense)
	{
	for(int i=0;i<GetArgCount();++i)
		{
		if(casesense==true)
			{
			if(MString::Compare(GetArg(i),argvalue)==0) { return true; }
			}
		else
			{
			if(MString::ICompare(GetArg(i),argvalue)==0) { return true; }
			}
		}

	return false;
	}


//////////////////////////////////////
bool MCommandArg::RemoveArg(const char *argvalue,bool casesense)
	{
	for(int i=GetArgCount()-1;i>=0;--i)
		{
		if(casesense==true)
			{
			if(MString::Compare(GetArg(i),argvalue)==0)  { RemoveArg(i);  }
			}
		else
			{
			if(MString::ICompare(GetArg(i),argvalue)==0) { RemoveArg(i);  }
			}
		}
	return false;
	}


//////////////////////////////////////////
bool MCommandArg::CheckRemoveArg(const char *argvalue,bool casesense)
	{
	if(IsArgument(argvalue,casesense)==true)
		{  RemoveArg(argvalue,casesense);  return true; }

	return false;
	}


//////////////////////////////////////////
bool MCommandArg::GetNameValue(const char *name
		,const char *&value,int &index,bool casesense)
	{
	int len=MStdStrLen(name);
	for(int i=0;i<GetArgCount();++i)
		{
		if(casesense==true)
			{
			if(GMemCompare(GetArg(i),name,len)==0)
				{  value=GetArg(i)+len; index=i; return true;  }
			}
		else
			{
			if(GMemICompare(GetArg(i),name,len)==0)
				{  value=GetArg(i)+len;  index=i; return true; }
			}
		}

	value=0;  index=-1;
	return false;
	}


////////////////////////////////////////////////////////////
bool MCommandArg::Show(void)
	{
	const int count=GetArgCount();
	for(int i=0;i<count;++i)
		{
		MStdPrintf(" Argument %2d : %s\n",i,GetArg(i) );
		}

	return true;
	}


