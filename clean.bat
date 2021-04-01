@ECHO OFF
PUSHD %~dp0
REM RD /Q /S Debug
REM RD /Q /S Release
RD /Q /S ipch
DEL /Q /F *.sdf
DEL /Q /F /A:H *.suo
FOR /D %%X IN (*) DO (
	RD /Q /S %%X\Debug
	RD /Q /S %%X\Release
	REM RD /Q /S %%X\*.user
)
POPD
EXIT /B
