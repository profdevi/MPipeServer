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


//v2.0 copyright Comine.com 20150810M0910
#include "MStdLib.h"
#include "MIReader.h"



/////////////////////////////////////////////////
bool MIReader::Read(char *string,int bufsize)
	{
	if(bufsize<1)
		{
		return false;
		}

	int i;
	for(i=0;i<bufsize-1;++i)
		{
		if(this->OnRead(string+i,1)==false)
			{
			string[i]=0;
			return true;
			}

		if(string[i]==0) { return true; }
		}

	// buffer full
	string[bufsize-1]=0;
	return true;	
	}


//////////////////////////////////////////////////
bool MIReader::Read(char &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		// May have reached end of file!
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MIReader::Read(unsigned char &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MIReader::Read(bool &value)
	{
	char bytevalue;
	if(this->OnRead(&bytevalue,sizeof(bytevalue))==false)
		{
		return false;
		}

	if(bytevalue==0)
		{
		value=false;
		}
	else
		{
		value=true;
		}

	return true;
	}


////////////////////////////////////////////////
bool MIReader::Read(short &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MIReader::Read(int &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MIReader::Read(unsigned long &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////
bool MIReader::Read(float &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////
bool MIReader::Read(double &value)
	{
	if(this->OnRead(&value,sizeof(value))==false)
		{
		return false;
		}

	return true;
	}


//////////////////////////////////////////////
bool MIReader::Read(void *block,int size)
	{
	if(this->OnRead(block,size)==false)
		{
		return false;
		}

	return true;
	}


///////////////////////////////////////////////
bool MIReader::ReadLine(char *string,int bufsize)	// Reads whole line till '\n'
	{
	if(bufsize<1)
		{
		return false;
		}

	// Read Line
	int i;
	for(i=0;i<bufsize-1;++i)
		{
		if(this->OnRead(string+i,1)==false)
			{
			//= We have reached EOF
			if(i==0)
				{
				//=We have not input at all
				string[0]=0;
				return false;
				}

			//=We Have some input

			// Place terminating zero
			string[i]=0;
			return true;
			}

		if(string[i]==0)
			{
			return false;
			}

		//Check for end of file
		if(string[i]=='\n')
			{
			string[i+1]=0;
			return true;
			}
		}

	// buffer full
	string[bufsize-1]=0;
	return true;
	}

