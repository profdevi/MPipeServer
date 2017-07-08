# MPipeServer
This Windows console application MPipeServer and the MPipeClient application are 
used to test components for using windows named pipes for interprocess 
communication across user processes.

# Background   (Raison D'etre)
One of my friends has a dental practice that uses a poorly written client/server 
software.  The server software is running on a Microsoft Windows Server, and  
it handles requests from client software running on Windows workstations.  The 
server was written as a user application that has to be manually started by 
logging into the Windows Server and clicking on the server application.  When 
the dental practice starts up in the morning, someone logs into the Windows 
Server and starts up the server software. 

The problem is that the server application locks files that needs to be 
automatically backed up nightly.  The server application should have been
written as a Windows Service so that it automatically starts up without user
intervention, and can be shutdown by anyone for backups.  I am sure the design
was caused to some business requirements that forced this engineered solution.

It is very easy to forcefully shutdown the server process by killing it, but 
I am not sure if files would be corrupted in the process. So, I though it would 
be nice to have a software that runs in the context of the user that logs 
into the machine, and gracefully shuts down the server process by sending 
WM_QUIT messages within the context of that user.  

There are probably many other solutions that I had not though off that can just 
solve the problem directly.  But, I wanted to practice Interprocess Communication 
and the Security Model for Windows.  I decided to use Named Pipes for 
communicating requests between processes.  The code of this project uses 
reusable components for Interprocess communication using named pipes, as 
well manipulating the security elements found in the Windows Operating System.


# Download Links 

The following links is for the direct download of the 32 bit executable with
an MD5 digest of the executable.

Windows 32-bit Binary : [MPipeServer.exe](bin/MPipeServer.exe)

MD5 Digest for binary : [MPipeServer.exe.md5](bin/MPipeServer.exe.md5)


         

