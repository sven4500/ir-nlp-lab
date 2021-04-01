@ECHO OFF
PUSHD %~dp0
NLP1.exe ../corpus/corpus.xml ../corpus/tokens.xml
POPD
EXIT /B
