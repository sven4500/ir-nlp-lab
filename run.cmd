@ECHO OFF
SET OUT_DIR=out
MKDIR %OUT_DIR%
FOR %%I IN (*.xml) DO IR1.exe %%I %OUT_DIR%
EXIT /B
