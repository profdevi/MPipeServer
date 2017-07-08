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


//v2.0 copyright Comine.com 20150810M1158
#include "MStdLib.h"
#include "MString.h"
#include "MStringArray.h"
#include "MStringBuffer.h"
#include "MCommandParser.h"


//*********************************************************
//** Module Elements
//*********************************************************
static const int GMAXTOKENSIZE=1000;


//*********************************************************
//** MCommandParser
//*********************************************************
void MCommandParser::ClearObject(void)
	{
	}

/////////////////////////////////////////////////
bool MCommandParser::IsSeperator(char ch)
	{
	const char *seplist=mSeperatorList.Get();
	if(mSeperatorList.Get()==NULL)
		{  
		return false;
		}

	for(int i=0;seplist[i]!=0;++i)
		{
		if(seplist[i]==ch) { return true; }
		}

	return false;
	}


/////////////////////////////////////////////////
MCommandParser::MCommandParser(void)
	{ 
	ClearObject();
	}


/////////////////////////////////////////////////
MCommandParser::~MCommandParser(void)
	{
	Destroy();
	}


/////////////////////////////////////////////////
bool MCommandParser::Create(const char *seperators)
	{
	Destroy();
	if(seperators==NULL)
		{
		Destroy();
		return false;
		}

	if(mSeperatorList.Create(seperators)==false)
		{
		Destroy();
		return false;
		}

	if(mArray.Create()==false)
		{
		Destroy();
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MCommandParser::Destroy(void)
	{
	mArray.Destroy();
	mSeperatorList.Destroy();
	ClearObject();
	return true;
	}


/////////////////////////////////////////////////
bool MCommandParser::Clear(void)
	{
	return mArray.Clear();
	}


/////////////////////////////////////////////////
bool MCommandParser::ParseString(const char *string)
	{
	Clear();

	// Parser Routine
	enum GState { START,CMDSIMPLE,DQUOTE,DQUOTE_SLASH}; 
	GState state=START;

	MStringBuffer buffer;
	if(buffer.Create(GMAXTOKENSIZE)==false)
		{
		return false;
		}

	buffer.Clear();
	mArray.Clear();

	for(int i=0;string[i]!=0;++i)
		{
		char nextchar=string[i];
		
		//FSM Routine
		if(state==START)
			{
			if(nextchar=='\"'){  state=DQUOTE;  continue;  }
			else if(IsSeperator(nextchar)==true) { continue; }
			else {  buffer.Add(nextchar);  state=CMDSIMPLE;  continue;  }
			}
		else if(state==CMDSIMPLE)
			{
			if(IsSeperator(nextchar)==true)
				{
				state=START;  mArray.SetString(buffer.Get());
				buffer.Clear();  continue;
				}
			else if(nextchar=='"')
				{
				state=DQUOTE;  mArray.SetString(buffer.Get());
				buffer.Clear();  continue;
				}
			buffer.Add(nextchar); continue;
			}
		else if(state==DQUOTE)
			{
			if(nextchar=='\"')
				{
				state=START;  mArray.SetString(buffer.Get());
				buffer.Clear(); continue;
				}
			else if(nextchar=='\\')
				{  state=DQUOTE_SLASH; continue;  }
			else if(nextchar=='\n' || nextchar=='\r')
				{
				state=START;  mArray.SetString(buffer.Get());
				buffer.Clear();  continue;
				}
			else
				{  buffer.Add(nextchar);  continue; }
			}
		else if(state==DQUOTE_SLASH)
			{
			if(nextchar=='\\')
				{  state=DQUOTE;  buffer.Add('\\'); continue;  }
			else if(nextchar=='\"')
				{  state=DQUOTE;  buffer.Add('\"');  continue;  }
			else if(nextchar=='r')
				{  state=DQUOTE;  buffer.Add('\r');  continue;  }
			else if(nextchar=='n')
				{  state=DQUOTE;  buffer.Add('\n');  continue;  }
			else if(nextchar=='t')
				{  state=DQUOTE;  buffer.Add('\t');  continue;  }
			else if(nextchar=='a')
				{  state=DQUOTE;  buffer.Add('\a');  continue;  }
			else	// Unknown Escape character
				{  state=DQUOTE;  continue; }
			}
		else
			{
			return false;
			}
		}

	// Make sure that processed everything
	if(state!=START && buffer.GetStringLength()>0)
		{  mArray.SetString(buffer.Get());  }

	buffer.Destroy();
	return true;
	}


/////////////////////////////////////////////////
int MCommandParser::GetCount(void)
	{
	return mArray.GetCount();
	}


/////////////////////////////////////////////////
const char *MCommandParser::GetString(int index)
	{
	return mArray.GetString(index);
	}



