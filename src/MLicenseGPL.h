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


//v1.0 copyright Comine.com 20170214T1045
#ifndef MLicenseGPL_h
#define MLicenseGPL_h

/////////////////////////////////////////////////
#include "MStdLib.h"

//******************************************************
//**  MLicenseGPL class
//******************************************************
class MLicenseGPL
	{
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////
	public:
	MLicenseGPL(bool construct=false);
	~MLicenseGPL(void);
	bool Create(void);
	bool Destroy(void);
	const char *Get(void);				// Get License String
	bool Print(void);					// Print out the license
	};

#endif // MLicenseGPL_h

