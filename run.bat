@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
SET INPUT_DIR=%1
SET OUTPUT_DIR=%2
FOR %%I IN (%INPUT_DIR%\*.txt) DO NLP1.exe %%I %OUTPUT_DIR%\%%~nI%%~xI
EXIT /B