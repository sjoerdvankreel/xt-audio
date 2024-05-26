@echo off
cd ..\dist\java\xt\release\target
gpg -ab xt.audio-2.1.pom
gpg -ab xt.audio-2.1.jar
gpg -ab xt.audio-2.1-sources.jar
gpg -ab xt.audio-2.1-javadoc.jar
jar -cvf xt.audio-2.1-bundle.jar xt.audio-2.1.pom xt.audio-2.1.jar xt.audio-2.1-sources.jar xt.audio-2.1-javadoc.jar xt.audio-2.1.pom.asc xt.audio-2.1.jar.asc xt.audio-2.1-sources.jar.asc xt.audio-2.1-javadoc.jar.asc
cd ..\..\..\..\..\build