# Microsoft Developer Studio Project File - Name="xdll_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=xdll_lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xdll_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xdll_lib.mak" CFG="xdll_lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xdll_lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "xdll_lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "xdll_lib - Win32 Release_Log" (based on "Win32 (x86) Static Library")
!MESSAGE "xdll_lib - Win32 Release_ImmLog" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xdll_lib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release"
# PROP Intermediate_Dir "../../../obj/xdll/Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "XDLL_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "xdll_lib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/Debug"
# PROP Intermediate_Dir "../../../obj/xdll/Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "xdll_lib - Win32 Release_Log"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xdll_lib___Win32_Release_Log"
# PROP BASE Intermediate_Dir "xdll_lib___Win32_Release_Log"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release_Log"
# PROP Intermediate_Dir "../../../obj/xdll/Release_Log"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "XDLL_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "XDLL_LIB" /D "NEED_LOG" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "xdll_lib - Win32 Release_ImmLog"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xdll_lib___Win32_Release_ImmLog"
# PROP BASE Intermediate_Dir "xdll_lib___Win32_Release_ImmLog"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release_ImmLog"
# PROP Intermediate_Dir "../../../obj/xdll/Release_ImmLog"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "XDLL_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "XDLL_LIB" /D "NEED_IMM" /D "NEED_LOG" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "xdll_lib - Win32 Release"
# Name "xdll_lib - Win32 Debug"
# Name "xdll_lib - Win32 Release_Log"
# Name "xdll_lib - Win32 Release_ImmLog"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Diskid32.cpp
# End Source File
# Begin Source File

SOURCE=.\DOMExtractor.cpp
# End Source File
# Begin Source File

SOURCE=.\gstring.cpp
# End Source File
# Begin Source File

SOURCE=.\HookHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\IUTLauncher.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TipsClient.cpp
# End Source File
# Begin Source File

SOURCE=.\TipsClientCsOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\WBWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\xdll.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DOMExtractor.h
# End Source File
# Begin Source File

SOURCE=.\gstring.h
# End Source File
# Begin Source File

SOURCE=.\HookHelper.h
# End Source File
# Begin Source File

SOURCE=.\HopInfo.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TipsClient.h
# End Source File
# Begin Source File

SOURCE=.\TipsClientCsOperator.h
# End Source File
# Begin Source File

SOURCE=.\WBWnd.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\xdll.h
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Group "protocol"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\base\synacast\xdll\PacketBuilder.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\base\ppl\mswin\mfc\net.cpp
# End Source File
# Begin Source File

SOURCE=..\..\base\ppl\mswin\mfc\net.h
# End Source File
# Begin Source File

SOURCE=..\..\base\ppl\mswin\mfc\string.h
# End Source File
# Begin Source File

SOURCE=..\..\base\ppl\mswin\mfc\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\base\ppl\mswin\mfc\TimerWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\base\ppl\mswin\mfc\TimerWindow.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
