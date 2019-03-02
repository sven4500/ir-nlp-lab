@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION
REM /A говорим что это арифметическая операция.
SET /A INDEX=0
SET /A COUNT=0
SET /A BATCH_SIZE=10000
REM Забираем только вторую подстроку.
FOR /F "TOKENS=2 DELIMS=," %%A IN (%1) DO (
	SET A=%%A
	@ECHO.!A:"=!
	SET /A COUNT+=1
	IF !COUNT! GEQ %BATCH_SIZE% (
		SET /A COUNT=0
		SET /A INDEX+=1
	)
) >> out_!INDEX!.txt
ENDLOCAL
EXIT /B
