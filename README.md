
## Introduction

This is just 'hello world' android jni application that is build from scratch
using only command line utilities. It does not use gradle, just a simple script.

I do not advocate to build app this way but just show how the whole build
process involved in the building of the an android jni application. You could
actually build 'regular' android apps with this method by just removing a few
lines related to c++ compilation.


## How to build

Clone the project and 'cd' into the directory. If you have a keystore file just
copy your file in this folder and name it keystore.jks otherwise just run this
command.

```
keytool -genkeypair -keystore keystore.jks -alias androidkey \
      -dname "CN=mqttserver.ibm.com, OU=ID, O=IBM, L=Hursley, S=Hants, C=GB" \
      -validity 10000 \
      -keyalg RSA \
      -keysize 2048 \
      -storepass android \
      -keypass android
```

After that you can just run 
```
./build.sh
```

It will create **handmade_native_android.apk** file which you can upload to your
phone via **adb** or any other way and install it.


## What you get

At the end you will have just a simple app with random number in the middle.
This number is loaded from shared library written in C++. Tappning on the screen
will requst and draw a different random number.
