@echo off
cd ..\dist\java\xt\target\
gpg -ab xt.audio-1.8.pom
gpg -ab xt.audio-1.8.jar
cd ..\..\..\..\build