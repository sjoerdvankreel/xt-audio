@echo off
cd ..\dist\java\xt\target\release\
gpg -ab xt.audio-2.0-SNAPSHOT.pom
gpg -ab xt.audio-2.0-SNAPSHOT.jar
gpg -ab xt.audio-2.0-SNAPSHOT-sources.jar
gpg -ab xt.audio-2.0-SNAPSHOT-javadoc.jar
jar -cvf xt.audio-2.0-SNAPSHOT-bundle.jar xt.audio-2.0-SNAPSHOT.pom xt.audio-2.0-SNAPSHOT.jar xt.audio-2.0-SNAPSHOT-sources.jar xt.audio-2.0-SNAPSHOT-javadoc.jar xt.audio-2.0-SNAPSHOT.pom.asc xt.audio-2.0-SNAPSHOT.jar.asc xt.audio-2.0-SNAPSHOT-sources.jar.asc xt.audio-2.0-SNAPSHOT-javadoc.jar.asc
cd ..\..\..\..\..\build