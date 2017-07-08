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
#ifndef MIREADER_H
#define MIREADER_H

////////////////////////////////////////////
struct MIReader
	{
	////////////////////////////////////////////
	virtual bool OnRead(void *buffer,int length)=0;

	////////////////////////////////////////////
	bool Read(char *string,int bufsize);
	bool Read(char &value);
	bool Read(unsigned char &value);
	bool Read(bool &value);
	bool Read(short &value);
	bool Read(int &value);
	bool Read(unsigned long &value);
	bool Read(float &value);
	bool Read(double &value);
	bool Read(void *block,int size);
	bool ReadLine(char *string,int bufsize);			// Reads whole line till '\n'
	};


////////////////////////////////////////////
// Support Functions
inline bool MIReaderRead(MIReader &reader,char *string,int bufsize)
	{
	return reader.Read(string,bufsize);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,char &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,unsigned char &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,bool &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,short &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,int &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,unsigned long &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,float &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,double &value)
	{
	return reader.Read(value);
	}


////////////////////////////////////////////
inline bool MIReaderRead(MIReader &reader,void *block,int size)
	{
	return reader.Read(block,size);
	}


////////////////////////////////////////////
inline bool MIReaderReadLine(MIReader &reader,char *string,int bufsize)
	{
	return reader.ReadLine(string,bufsize);
	}


#endif // MIREADER_H


