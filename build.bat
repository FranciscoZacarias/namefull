@echo off

set compiler_and_entry=cl ..\source\main.c

REM Linker errors for GLFW. Use /MD on release and /MDd on debug builds
set cl_default_flags=/Isource /nologo /FC /Zi /MDd

set external_include= /I"..\source\external\opengl" ^
											/I"..\source\external\glfw-3.3.9\include" ^
											/I"..\source\f_base" 

set linker_flags= user32.lib ^
									gdi32.lib ^
									Shell32.lib ^
									opengl32.lib ^
									winmm.lib ^
									"..\source\external\glfw-3.3.9\lib\glfw3.lib"

if not exist build mkdir build
pushd build
%compiler_and_entry% %cl_default_flags% %external_include% %linker_flags% /Fe"namefull.exe"
popd