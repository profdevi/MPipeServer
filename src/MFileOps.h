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


//v1.49 copyright Comine.com 20170406R1314
#ifndef MFileOps_h
#define MFileOps_h

////////////////////////////////////////////////
#include "MStdLib.h"
#include "MString.h"
#include "MBuffer.h"

//******************************************************
//**  MFileOps class
//******************************************************
class MFileOps
	{
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MFileOps(void);
	explicit MFileOps(bool autocreate);
	~MFileOps(void);
	bool Create(void);
	bool Destroy(void);
	bool Move(const char *srcfile,const char *dstfile			// Move a file
			,bool erroronfail=false);			
	bool MoveOnReboot(const char *srcfile,const char *dstfile);	// Move File after reboot
	bool Delete(const char *filename							// Delete a file
			,bool erroronfaile=false);
	bool DeleteOnReboot(const char *filename);					// Delete file on reboot
	bool OverWriteDelete(const char *filename);					// Overwrite and delete file
	bool Copy(const char *srcfile,const char *dstfile			// Copy a file
			,bool stopifexists=false,bool erroronfail=false);					
	bool Exists(const char *filename);							// Check if file exists
	bool IsBinary(const char *filename);						// Check if file is binary
	bool IsOpenable(const char *filename);						// Check if file is openable
	bool AreSame(const char *file1,const char *file2);			// Compare two files
	bool ConvertTextUnixToDos(const char *filename);			// Unix To Dos Text
	bool ConvertTextDosToUnix(const char *filename);			// Dos To Unix Text
	bool CleanToDosText(const char *filename);					// Cleans to Dos Text
	bool IsUnixTextFile(const char *filename);					// Check if unix text file
	bool IsDosTextFile(const char *filename);					// Check of dos text file
	bool Append(const char *filename,const char *extrafile);	// Append contents of file with extra
	bool MakeNewFile(const char *filename);						// Create fresh blank file
	bool IsDirectory(const char *filename);						// = true if filename is dir
	bool IsReadOnly(const char *filename);						// =true if filename
	bool SetReadOnly(const char *filename,bool val);			// Set File name
	bool IsSystemFile(const char *filename);					// =true if filename
	bool SetSystemFile(const char *filename,bool val);			// Set File as system file
	bool IsHiddenFile(const char *filename);					// =true if filename
	bool SetHiddenFile(const char *filename,bool val);			// Set File as hidden file
	bool WriteTextFile(const char *filename,const char *data);	// Create fresh file
	bool WriteBinaryFile(const char *filename,const char *data,int buffer);
	bool AppendTextFile(const char *filename,const char *extra);// Append text to file
	bool ReadTextFile(const char *filename,char *buffer
			,int &size);										// Read from file into buffer
	bool ReadTextFile(const char *filename,MBuffer &buffer);	// Read from file into buffer
	bool ReadBinaryFile(const char *filename,MBuffer &buffer);	// Read from file into buffer
	bool GetAbsolutePath(const char *filename
			,MString &absolutepath);							// Get Path for file
	bool GetTemperoryFileName(MString &filename);				// Get Full path Temp File
	bool WriteToExistingFile(const char *filename
			,const char *data,int length,int offset=0);			// Write to existing binary file
	bool GetTempFileNameInDir(const char *directory,MString &filename);
	bool GetCanonicalName(const char *path,MString &canonname);	// convert all paths to / based
	bool PrintFile(const char *filename);						// Print contents of file
	int GetSize(const char *filename);							// Get File Size
	bool SetSize(const char *filename,int size);				// Resize file
	int GetLineCount(const char *filename);						// Get Line Count of file
	bool GetAppPath(MString &apppathname);						// Get Application path name
	bool GetAppConfigFile(MString &configfile);					// Get Application Configuration File
	};

#endif // MFileOps_h

