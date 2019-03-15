@ECHO OFF
FOR %%I IN (*.xml) DO IR1.exe %%I corpus.xml
EXIT /B
