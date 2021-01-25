@echo off
cd ..\dist\java\xt\target\
gpg -ab xt.audio-1.8-SNAPSHOT.pom
gpg -ab xt.audio-1.8-SNAPSHOT.jar
gpg -ab xt.audio-1.8-SNAPSHOT-sources.jar
gpg -ab xt.audio-1.8-SNAPSHOT-javadoc.jar
jar -cvf xt.audio-1.8-SNAPSHOT-bundle.jar xt.audio-1.8-SNAPSHOT.pom xt.audio-1.8-SNAPSHOT.jar xt.audio-1.8-SNAPSHOT-sources.jar xt.audio-1.8-SNAPSHOT-javadoc.jar xt.audio-1.8-SNAPSHOT.pom.asc xt.audio-1.8-SNAPSHOT.jar.asc xt.audio-1.8-SNAPSHOT-sources.jar.asc xt.audio-1.8-SNAPSHOT-javadoc.jar.asc
cd ..\..\..\..\build