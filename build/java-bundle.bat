@echo off
cd ..\dist\java\xt\release\target
gpg -ab xt.audio-2.2.pom
gpg -ab xt.audio-2.2.jar
gpg -ab xt.audio-2.2-sources.jar
gpg -ab xt.audio-2.2-javadoc.jar
jar -cvf xt.audio-2.2-bundle.jar xt.audio-2.2.pom xt.audio-2.2.jar xt.audio-2.2-sources.jar xt.audio-2.2-javadoc.jar xt.audio-2.2.pom.asc xt.audio-2.2.jar.asc xt.audio-2.2-sources.jar.asc xt.audio-2.2-javadoc.jar.asc
cd ..\..\..\..\..\build