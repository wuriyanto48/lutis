# Lutis

[![lutis CI](https://github.com/wuriyanto48/lutis/actions/workflows/ci.yml/badge.svg)](https://github.com/wuriyanto48/lutis/actions/workflows/ci.yml)

An Experimental C/C++ Graphics Lib binding with `Nodejs`

### Requirements
- C and C++ compiler, gcc https://gcc.gnu.org/install/binaries.html or other compilers
- Nodejs https://nodejs.org/en/download/
- cmake https://cmake.org/download/
- cmake-js https://github.com/cmake-js/cmake-js

### Current C/C++ Graphics and other dependencies
- OpenCV https://github.com/opencv/opencv
- OpenCV Contrib https://github.com/opencv/opencv_contrib
- ImageMagick https://github.com/ImageMagick/ImageMagick.git
- LibJpeg https://github.com/stohrendorf/libjpeg-cmake.git
- LibPng https://github.com/glennrp/libpng.git
- LibWebp https://github.com/webmproject/libwebp.git
- LibZ https://github.com/madler/zlib.git

### Building

Install `cmake-js`
```shell
$ npm install --location=global cmake-js
```

Install Nodejs dependencies
```shell
$ npm install
```

Build and Install Vendor
```shell
$ make build-vendor
```

Build Project
```shell
$ make build
```

Running on Linux
```shell
$ ./runlinux
```