@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
SET INPUT_FILE=input.csv
SET OUTPUT_FILE=output
SET OUTPUT_EXT=txt
REM /A говорим что это арифметическая операция
SET /A INDEX=0
SET /A COUNT=0
SET /A BATCH_SIZE=10000
FOR /F "TOKENS=2 DELIMS=," %%A IN (%INPUT_FILE%) DO (
	SET A=%%A
	@ECHO.!A:"=!
	SET /A COUNT+=1
	IF !COUNT! GTR !BATCH_SIZE! (
		SET /A COUNT=0
		SET /A INDEX+=1
	)
) >> !OUTPUT_FILE!_!INDEX!.!OUTPUT_EXT!
ENDLOCAL
EXIT /B
