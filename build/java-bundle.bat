@echo off
cd ..\dist\java\xt\target\
gpg -ab xt.audio-1.8.pom
gpg -ab xt.audio-1.8.jar
gpg -ab xt.audio-1.8-sources.jar
gpg -ab xt.audio-1.8-javadoc.jar
jar -cvf xt.audio-1.8-bundle.jar xt.audio-1.8.pom xt.audio-1.8.jar xt.audio-1.8-sources.jar xt.audio-1.8-javadoc.jar xt.audio-1.8.pom.asc xt.audio-1.8.jar.asc xt.audio-1.8-sources.jar.asc xt.audio-1.8-javadoc.jar.asc
cd ..\..\..\..\build