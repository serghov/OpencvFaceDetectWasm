# Opencv face detection using emscripten

## Introduction

With this project I wanted to see how much of opencv can be ported to work 
on emscripten with .wasm.

Live demo [here](https://serghov.github.io/OpencvFaceDetectWasm/cmake-build-debug)

## About

There are two main parts to in the codebase `/web` and `/cpp`.

`index.html` in `/web` loads a video in a `<video>` tag and runs a small script
that copies each frame of that video onto a canvas, runs `getImageData` on that canvas
and passes the resulted pixel array to c++.

`main.cpp` in `/cpp` grabs the pixel array passed by js, wraps it in an opencv `Mat`
and runs [haar face detection](http://docs.opencv.org/2.4/doc/tutorials/objdetect/cascade_classifier/cascade_classifier.html) on it.
Afterwards the image is displayed using emscripten's port of SDL.

## Building


To build this project you will need emscripten which can be found [here](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).

You will also need a version of Opencv built with emscripten, you can either download the version I build it [here](https://drive.google.com/file/d/0B2jxqOyAJmV6Ul9uUzFGNjV2SlE/view?usp=sharing),
or build it yourself (instructions below).

After getting the dependencies you will need to set 2 environment variables
* OPENCV_INSTALL_DIR=directory where you installed or unziped opencv
* EMSCRIPTEN=emscripten directory, if you run source ./emsdk_env.sh this will be set automatically
 
 After this just go ahead and 
 ```
 cd cmake-debug-build # or another directory if you want
 camke ..
 make
 ```
 This will build the project, and make an `index.html`, it needs a webserver to work properly.
 You could use python
 ```
 python -m SimpleHTTPServer
 ```
 Open `localhost:8000` in your browser and thats it!
 
#### Building opencv

Coming soon...

use prebuilt .zip for now

## TODO

* webcam
* decode video in cpp with ffmpeg of opencv
* other stuff?
