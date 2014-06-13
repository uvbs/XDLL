# Microsoft Developer Studio Project File - Name="monitor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=monitor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "monitor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "monitor.mak" CFG="monitor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "monitor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "monitor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "monitor - Win32 Release_Log" (based on "Win32 (x86) Application")
!MESSAGE "monitor - Win32 Release_ImmLog" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "monitor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release"
# PROP Intermediate_Dir "../../../obj/xdll/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=\RootSVN\XDLL\bin\Release\monitor.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "F:\RootSVN\XDLL\bin\Release\WCScheduler.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "monitor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/Debug"
# PROP Intermediate_Dir "../../../obj/xdll/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=\RootSVN\XDLL\bin\Debug\monitor.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "F:\RootSVN\XDLL\bin\Debug\WCScheduler.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "monitor - Win32 Release_Log"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "monitor___Win32_Release_Log"
# PROP BASE Intermediate_Dir "monitor___Win32_Release_Log"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release_Log"
# PROP Intermediate_Dir "../../../obj/xdll/Release_Log"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NEED_LOG" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=\RootSVN\XDLL\bin\Release_Log\monitor.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "F:\RootSVN\XDLL\bin\Release_Log\WCScheduler.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "monitor - Win32 Release_ImmLog"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "monitor___Win32_Release_ImmLog"
# PROP BASE Intermediate_Dir "monitor___Win32_Release_ImmLog"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release_ImmLog"
# PROP Intermediate_Dir "../../../obj/xdll/Release_ImmLog"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "NEED_LOG" /D "NEED_IMM" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=\RootSVN\XDLL\bin\Release_ImmLog\monitor.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "F:\RootSVN\XDLL\bin\Release_ImmLog\WCScheduler.exe"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "monitor - Win32 Release"
# Name "monitor - Win32 Debug"
# Name "monitor - Win32 Release_Log"
# Name "monitor - Win32 Release_ImmLog"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppDM.cpp
# End Source File
# Begin Source File

SOURCE=.\DefConst.cpp
# End Source File
# Begin Source File

SOURCE=.\monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\monitor.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Update.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppDM.h
# End Source File
# Begin Source File

SOURCE=.\DefConst.h
# End Source File
# Begin Source File

SOURCE=.\message.h
# End Source File
# Begin Source File

SOURCE=.\monitor.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Update.h
# End Source File
# Begin Source File

SOURCE=.\UpdateThread.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\monitor.ico
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "GeneralUtils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GeneralUtils\CrashHandler.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\MyLog.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\MyLog.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\NetUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\NetUtils.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\PathAndDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\PathAndDisk.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\PerfCounters.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\RegAndProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\RegAndProfile.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\StrUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\StrUtils.h
# End Source File
# Begin Source File

SOURCE=.\GeneralUtils\Win32Thread.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
