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


//v2.0 copyright Comine.com 20150810M0909
#ifndef MIWRITER_H
#define MIWRITER_H

////////////////////////////////////////////
#include "MStdLib.h"

////////////////////////////////////////////
struct MIWriter
	{
	/////////////////////////////////////////////////////////
	virtual bool OnWrite(const void *buffer,int length)=0;

	// Support Functions
	bool Write(const void *buffer,int length);
	bool Write(const char *string);			// Writes a string with 0 at end
	bool Write(short value);				// Write 16 bit short
	bool Write(int value);
	bool Write(unsigned long value);
	bool Write(char value);
	bool Write(unsigned char value);
	bool Write(bool value);					// Write a boolean
	bool Write(float value);
	bool Write(double value);
	bool WriteChars(const char *string);	// Write all chars of except zero
	bool WriteChars(const char *str1,const char *str2,const char *str3=NULL
			,const char *str4=NULL,const char *str5=NULL,const char *str6=NULL);
	};


///////////////////////////////////////////////////////////////
// Support Functions
inline bool MIWriterWrite(MIWriter &writer,const void *buffer,int length)
	{
	return writer.Write(buffer,length);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,const char *string)
	{
	return writer.Write(string);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,short value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,int value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,unsigned long value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,char value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,unsigned char value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,bool value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,float value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWrite(MIWriter &writer,double value)
	{
	return writer.Write(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWriteChars(MIWriter &writer,const char *value)
	{
	return writer.WriteChars(value);
	}


///////////////////////////////////////////////////////////////
inline bool MIWriterWriteChars(MIWriter &writer,const char *str1,const char *str2,const char *str3=NULL
		,const char *str4=NULL,const char *str5=NULL,const char *str6=NULL)
	{
	return writer.WriteChars(str1,str2,str3,str4,str5,str6);
	}

#endif // MIWRITER_H

