@echo off

pushd %~dp0\dst

cl /W3 /Zi /Od /nologo ..\example.c /link /INCREMENTAL:NO
cl /W3 /Zi /Od /nologo ..\test.c /link /INCREMENTAL:NO

del *.obj
popd
