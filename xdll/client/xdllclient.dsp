# Microsoft Developer Studio Project File - Name="xdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xdll - Win32 DebugShow_Test
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xdllclient.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xdllclient.mak" CFG="xdll - Win32 DebugShow_Test"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xdll - Win32 Release_Log" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xdll - Win32 Release_ImmLog" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xdll - Win32 DebugShow" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xdll - Win32 DebugShow_Test" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xdll - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/Release"
# PROP Intermediate_Dir "../../../obj/xdll/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"../../../bin/Release/TipsClient.dll"
# Begin Special Build Tool
TargetPath=\XDLL\XDLL\bin\Release\TipsClient.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "C:\Program Files\Common Files\PPLiveNetwork\3.5.1.0098\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xdll - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/Debug"
# PROP Intermediate_Dir "../../../obj/xdll/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib shlwapi.lib Urlmon.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../bin/Debug/TipsClient.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=\XDLL\XDLL\bin\Debug\TipsClient.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "C:\Program Files\Common Files\PPLiveNetwork\3.5.1.0098\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xdll - Win32 Release_Log"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xdll___Win32_Release_Log"
# PROP BASE Intermediate_Dir "xdll___Win32_Release_Log"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xdll___Win32_Release_Log"
# PROP Intermediate_Dir "xdll___Win32_Release_Log"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "NEED_LOG" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib" /out:"../../../bin/Release/TipsClient.dll"
# ADD LINK32 winmm.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib" /out:"../../../bin/Release_Log/TipsClient.dll"
# Begin Special Build Tool
TargetPath=\XDLL\XDLL\bin\Release_Log\TipsClient.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "C:\Program Files\Common Files\PPLiveNetwork\3.5.1.0098\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xdll - Win32 Release_ImmLog"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xdll___Win32_Release_ImmLog0"
# PROP BASE Intermediate_Dir "xdll___Win32_Release_ImmLog0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xdll___Win32_Release_ImmLog0"
# PROP Intermediate_Dir "xdll___Win32_Release_ImmLog0"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "NEED_LOG" /D "NEED_IMM" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I "../../base" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "NEED_LOG" /D "NEED_IMM" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib" /out:"../../../bin/Release_ImmLog/TipsClient.dll"
# ADD LINK32 winmm.lib shlwapi.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"msvcrt.lib msvcrtd.lib" /out:"../../../bin/Release_ImmLog/TipsClient.dll"
# Begin Special Build Tool
TargetPath=\XDLL\XDLL\bin\Release_ImmLog\TipsClient.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "C:\Program Files\Common Files\PPLiveNetwork\3.5.1.0098\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xdll - Win32 DebugShow"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xdll___Win32_DebugShow"
# PROP BASE Intermediate_Dir "xdll___Win32_DebugShow"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xdll___Win32_DebugShow"
# PROP Intermediate_Dir "xdll___Win32_DebugShow"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /D "_SHOW_BROWSER" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib shlwapi.lib Urlmon.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../bin/Debug/TipsClient.dll" /pdbtype:sept
# ADD LINK32 winmm.lib shlwapi.lib Urlmon.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../bin/Debug/TipsClient.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=\XDLL\XDLL\bin\Debug\TipsClient.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "C:\Program Files\Common Files\PPLiveNetwork\3.5.1.0098\"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "xdll - Win32 DebugShow_Test"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xdll___Win32_DebugShow_Test"
# PROP BASE Intermediate_Dir "xdll___Win32_DebugShow_Test"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xdll___Win32_DebugShow_Test"
# PROP Intermediate_Dir "xdll___Win32_DebugShow_Test"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /D "_SHOW_BROWSER" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../base" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /D "_SHOW_BROWSER" /D "_TEST" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib shlwapi.lib Urlmon.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../bin/Debug/TipsClient.dll" /pdbtype:sept
# ADD LINK32 winmm.lib shlwapi.lib Urlmon.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../bin/Debug/TipsClient.dll" /pdbtype:sept
# Begin Special Build Tool
TargetPath=\XDLL\XDLL\bin\Debug\TipsClient.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(TargetPath) "C:\Program Files\Common Files\PPLiveNetwork\3.5.1.0098\"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "xdll - Win32 Release"
# Name "xdll - Win32 Debug"
# Name "xdll - Win32 Release_Log"
# Name "xdll - Win32 Release_ImmLog"
# Name "xdll - Win32 DebugShow"
# Name "xdll - Win32 DebugShow_Test"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "tinyxml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=.\tinystr.h
# End Source File
# Begin Source File

SOURCE=.\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=.\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=.\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AdConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\AdConfig.h
# End Source File
# Begin Source File

SOURCE=.\AdPolicy.cpp
# End Source File
# Begin Source File

SOURCE=.\AdPolicy.h
# End Source File
# Begin Source File

SOURCE=.\AsyncHttpDownloadListener.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncHttpDownloadListener.h
# End Source File
# Begin Source File

SOURCE=.\AutoLocks.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoLocks.h
# End Source File
# Begin Source File

SOURCE=.\CRegKeyEx.h
# End Source File
# Begin Source File

SOURCE=.\detours.h
# End Source File
# Begin Source File

SOURCE=.\Diskid32.cpp
# End Source File
# Begin Source File

SOURCE=.\DOMExtractor.cpp
# End Source File
# Begin Source File

SOURCE=.\DOMExtractor.h
# End Source File
# Begin Source File

SOURCE=.\ExternalObject.cpp
# End Source File
# Begin Source File

SOURCE=.\gstring.cpp
# End Source File
# Begin Source File

SOURCE=.\gstring.h
# End Source File
# Begin Source File

SOURCE=.\HookHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\HookHelper.h
# End Source File
# Begin Source File

SOURCE=.\HopInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\HopInfo.h
# End Source File
# Begin Source File

SOURCE=.\HopXmlConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\HopXmlConfig.h
# End Source File
# Begin Source File

SOURCE=.\httpimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpProtocol.cpp
# End Source File
# Begin Source File

SOURCE=.\iut.exe.h
# End Source File
# Begin Source File

SOURCE=.\IUTLauncher.cpp
# End Source File
# Begin Source File

SOURCE=.\IUTLauncher.h
# End Source File
# Begin Source File

SOURCE=.\LogVast.cpp
# End Source File
# Begin Source File

SOURCE=.\LogVast.h
# End Source File
# Begin Source File

SOURCE=.\MsgTask.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgTask.h
# End Source File
# Begin Source File

SOURCE=.\PlatformUA.cpp
# End Source File
# Begin Source File

SOURCE=.\PlatformUA.h
# End Source File
# Begin Source File

SOURCE=.\RemoveCookie.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RunningTask.cpp
# End Source File
# Begin Source File

SOURCE=.\RunningTask.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Task.cpp
# End Source File
# Begin Source File

SOURCE=.\Task.h
# End Source File
# Begin Source File

SOURCE=.\TipsClient.cpp
# End Source File
# Begin Source File

SOURCE=.\TipsClient.h
# End Source File
# Begin Source File

SOURCE=.\TipsClientCsOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\TipsClientCsOperator.h
# End Source File
# Begin Source File

SOURCE=.\TipsConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\TipsConvert.h
# End Source File
# Begin Source File

SOURCE=.\TipsUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\TipsUtil.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\WBWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\WBWnd.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\xdll.cpp
# End Source File
# Begin Source File

SOURCE=.\xdll.def
# End Source File
# Begin Source File

SOURCE=.\xdll.h
# End Source File
# Begin Source File

SOURCE=.\Xdll.idl
# ADD MTL /h "Xdll_i.h" /iid "Xdll_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\xdll.rc
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap7.bmp
# End Source File
# Begin Source File

SOURCE=.\TipsClientCsOperator.rgs
# End Source File
# Begin Source File

SOURCE=.\res\xdll.rc2
# End Source File
# Begin Source File

SOURCE=.\Xdll.rgs
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
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ExternalObject.h
# End Source File
# Begin Source File

SOURCE=.\httpimpl.h
# End Source File
# Begin Source File

SOURCE=.\HttpProtocol.h
# End Source File
# Begin Source File

SOURCE=.\urlconvert.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section xdll : {230A6C6C-6564-6966-6E65-2057494E4150}
# 	1:8:IDR_XDLL:3000
# End Section
