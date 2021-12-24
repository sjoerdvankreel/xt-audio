@echo off
cd ..\dist\java\xt\release\target
gpg -ab xt.audio-2.0.pom
gpg -ab xt.audio-2.0.jar
gpg -ab xt.audio-2.0-sources.jar
gpg -ab xt.audio-2.0-javadoc.jar
jar -cvf xt.audio-2.0-bundle.jar xt.audio-2.0.pom xt.audio-2.0.jar xt.audio-2.0-sources.jar xt.audio-2.0-javadoc.jar xt.audio-2.0.pom.asc xt.audio-2.0.jar.asc xt.audio-2.0-sources.jar.asc xt.audio-2.0-javadoc.jar.asc
cd ..\..\..\..\..\build