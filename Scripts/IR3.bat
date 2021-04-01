@ECHO OFF
PUSHD %~dp0
IR3.exe ../corpus/tokens.xml ../corpus/index.dat
POPD
EXIT /B
