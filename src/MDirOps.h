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


//v1.20 copyright Comine.com 20150813R1238
#ifndef MDirOps_h
#define MDirOps_h

////////////////////////////////////////////////////
#include "MBuffer.h"

//******************************************************
//**  MDirOps class
//******************************************************
class MDirOps
	{
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	void ClearObject(void);
	
	////////////////////////////////////////////////
	public:
	MDirOps(bool create=false);
	~MDirOps(void);
	bool Create(void);
	bool Destroy(void);
	bool MakeDir(const char *dirname,bool erroronfail=false);	// Make a directory

	// Make all subdirectories.  For instance c:\work\tmp\x\y will create
	// subdirectories c:\work, c:\work\tmp, c:\work\tmp\x, c:\work\tmp\x\y
	bool MakeDirNested(const char *dirname);

	bool RemoveDir(const char *dirname,bool erroronfail=false);
	bool Exists(const char *dirname);
	bool IsSystem(const char *dirname);
	bool IsReadOnly(const char *dirname);
	bool IsHidden(const char *dirname);
	bool IsEncrypted(const char *dirname);
	bool IsCompressed(const char *dirname);						// Is Directory Compressed
	bool Move(const char *srcdir,const char *dstdir				// Move a directory
			,bool erroronfail=false);
	bool MoveOnReboot(const char *srcdir,const char *dstdir);	// Move dir after reboot
	bool GetCurrentDir(MBuffer &dir);							// Return Current Directory
	bool GetCurrentDir(char drive,MBuffer &dir);				// Return Current Directory of drive
	bool GetTempDir(MBuffer &buf);								// Get Temp Directory with \ end
	bool SetDir(const char *dir);								// Change Directory
	bool IsReadable(const char *dir);							// Is Directory Readable
	};

#endif // MDirOps_h

