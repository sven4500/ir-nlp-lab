@ECHO OFF
FOR %%I IN (%1\*.txt) DO NLP1.exe %%I %2
EXIT /B
