@echo off
del SDL2.dll PiXL.exe
mkdir build
pushd build
del *.* /q
set LUA_SRC=..\lua\src\lapi.c ..\lua\src\lauxlib.c ..\lua\src\lbaselib.c ..\lua\src\lbitlib.c ..\lua\src\lcode.c ..\lua\src\lcorolib.c ..\lua\src\lctype.c ..\lua\src\ldblib.c ..\lua\src\ldebug.c ..\lua\src\ldo.c ..\lua\src\ldump.c ..\lua\src\lfunc.c ..\lua\src\lgc.c ..\lua\src\linit.c ..\lua\src\liolib.c ..\lua\src\llex.c ..\lua\src\lmathlib.c ..\lua\src\lmem.c ..\lua\src\loadlib.c ..\lua\src\lobject.c ..\lua\src\lopcodes.c ..\lua\src\loslib.c ..\lua\src\lparser.c ..\lua\src\lstate.c ..\lua\src\lstring.c ..\lua\src\lstrlib.c ..\lua\src\ltable.c ..\lua\src\ltablib.c ..\lua\src\ltm.c ..\lua\src\lundump.c ..\lua\src\lutf8lib.c ..\lua\src\lvm.c ..\lua\src\lzio.c
cl /O2 /MT /FePiXL.exe /I ..\lua\src /I ..\SDL2\include ..\pixl.c %LUA_SRC% /link /WX /SUBSYSTEM:WINDOWS /LIBPATH:..\SDL2\lib\x64
popd
copy SDL2\lib\x64\SDL2.dll .
copy build\PiXL.exe .
