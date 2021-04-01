@ECHO OFF
PUSHD %~dp0
FOR %%I IN (..\corpus\*.xml) DO IR1.exe %%I ..\corpus\corpus.xml
POPD
EXIT /B
