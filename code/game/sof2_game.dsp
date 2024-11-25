# Microsoft Developer Studio Project File - Name="SoF2game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SoF2game - Win32 SH Debug SoF2
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sof2_game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sof2_game.mak" CFG="SoF2game - Win32 SH Debug SoF2"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SoF2game - Win32 Release SoF2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SoF2game - Win32 Debug SoF2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SoF2game - Win32 SH Debug SoF2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SoF2game___Win32_Release_TA"
# PROP BASE Intermediate_Dir "SoF2game___Win32_Release_TA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release"
# PROP Intermediate_Dir "../Release/SoF2game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W4 /GX /O2 /D "WIN32" /D "NDebug SoF2" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W4 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MISSIONPACK" /D "QAGAME" /D "_SOF2" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDebug SoF2" /mktyplib203 /win32
# ADD MTL /nologo /D "NDebug SoF2" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDebug SoF2"
# ADD RSC /l 0x409 /d "NDebug SoF2"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release/qaSoF2gamex86.dll"
# SUBTRACT BASE LINK32 /incremental:yes /debug
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map:"../Release/sof2mp_gamex86.map" /debug /machine:I386 /def:".\SoF2_game.def" /out:"../Release/sof2mp_gamex86.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SoF2game___Win32_Debug_TA"
# PROP BASE Intermediate_Dir "SoF2game___Win32_Debug_TA"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "..\Debug\SoF2game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_Debug SoF2" /D "_WINDOWS" /D "BUILDING_REF_GL" /D "Debug SoF2" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "BUILDING_REF_GL" /D "Debug SoF2" /D "WIN32" /D "_WINDOWS" /D "MISSIONPACK" /D "QAGAME" /D "_SOF2" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_Debug SoF2" /mktyplib203 /win32
# ADD MTL /nologo /D "_Debug SoF2" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_Debug SoF2"
# ADD RSC /l 0x409 /d "_Debug SoF2"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /debug /machine:I386 /out:"..\Debug/qaSoF2gamex86.dll"
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map:"..\Debug\sof2mp_gamex86.map" /debug /machine:I386 /def:".\SoF2_game.def" /out:"..\Debug\sof2mp_gamex86.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SoF2game___Win32_SH_Debug_SoF2"
# PROP BASE Intermediate_Dir "SoF2game___Win32_SH_Debug_SoF2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SoF2game___Win32_SH_Debug_SoF2"
# PROP Intermediate_Dir "SoF2game___Win32_SH_Debug_SoF2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "BUILDING_REF_GL" /D "Debug SoF2" /D "WIN32" /D "_WINDOWS" /D "MISSIONPACK" /D "QAGAME" /D "_SOF2" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "BUILDING_REF_GL" /D "Debug SoF2" /D "WIN32" /D "_WINDOWS" /D "MISSIONPACK" /D "QAGAME" /D "_SOF2" /D "MEM_DEBUG" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_Debug SoF2" /mktyplib203 /win32
# ADD MTL /nologo /D "_Debug SoF2" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_Debug SoF2"
# ADD RSC /l 0x409 /d "_Debug SoF2"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map:"..\Debug\sof2mp_gamex86.map" /debug /machine:I386 /def:".\SoF2_game.def" /out:"..\Debug\sof2mp_gamex86.dll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /pdb:"..\SHDebug/sof2mp_gamex86.pdb" /map:"..\SHDebug\sof2mp_gamex86.map" /debug /machine:I386 /def:".\SoF2_game.def" /out:"..\SHDebug\sof2mp_gamex86.dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "SoF2game - Win32 Release SoF2"
# Name "SoF2game - Win32 Debug SoF2"
# Name "SoF2game - Win32 SH Debug SoF2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\ai_main.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ai_util.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ai_wpnav.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_gametype.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_lib.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP BASE Exclude_From_Build 1
# PROP Intermediate_Dir "..\SHDebug\SoF2game"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_misc.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_player.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_pmove.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_slidemove.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_weapons.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_active.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_antilag.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_bot.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_client.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_cmds.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_combat.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_gametype.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_items.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_main.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_misc.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_missile.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_mover.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_session.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_spawn.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_svcmds.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_syscalls.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_target.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_team.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_trigger.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_utils.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_weapon.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\q_math.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\q_shared.c

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SoF2_game.def

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP BASE Exclude_From_Build 1
# PROP Intermediate_Dir "..\SHDebug\SoF2game"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ai_main.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\anims.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_aas.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ai_char.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ai_chat.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ai_gen.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ai_goal.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ai_move.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ai_weap.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\be_ea.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_local.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_public.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bg_weapons.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\botlib.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cgame\cg_local.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cgame\cg_public.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chars.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\qcommon\disablewarnings.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\ghoul2\G2.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_local.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_public.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\g_team.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inv.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\match.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\ui\menudef.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\q_shared.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\surfaceflags.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\syn.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\qcommon\tags.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cgame\tr_types.h

!IF  "$(CFG)" == "SoF2game - Win32 Release SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 Debug SoF2"

!ELSEIF  "$(CFG)" == "SoF2game - Win32 SH Debug SoF2"

# PROP Intermediate_Dir "..\SHDebug\SoF2game"

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\game.bat
# End Source File
# Begin Source File

SOURCE=.\game.q3asm
# End Source File
# End Target
# End Project
