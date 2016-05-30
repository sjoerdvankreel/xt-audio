set build=%CD%
cd ../dist/cpp-sample-win32-x64-static
xt-cpp-sample.exe
cd %build%
cd ../dist/cli-sample
xt-cli-sample.exe
cd %build%
cd ../dist/java-sample
java -jar com.xt-audio.xt.sample-1.0.2.jar
cd %build%