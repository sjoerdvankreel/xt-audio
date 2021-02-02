@echo off
cd ..\dist\java\xt\target\
gpg -ab xt.audio-1.9.pom
gpg -ab xt.audio-1.9.jar
gpg -ab xt.audio-1.9-sources.jar
gpg -ab xt.audio-1.9-javadoc.jar
jar -cvf xt.audio-1.9-bundle.jar xt.audio-1.9.pom xt.audio-1.9.jar xt.audio-1.9-sources.jar xt.audio-1.9-javadoc.jar xt.audio-1.9.pom.asc xt.audio-1.9.jar.asc xt.audio-1.9-sources.jar.asc xt.audio-1.9-javadoc.jar.asc
cd ..\..\..\..\build