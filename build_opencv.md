### Requirements
- gcc https://gcc.gnu.org/install/binaries.html
- cmake https://cmake.org/download/


### Building

#### Build OpenCV

Clone OpenCV and OpenCV Contrib from Github. Create `vendor` folder to put `opencv` and all our external library.

```shell
$ mkdir vendor
$ cd vendor
$ mkdir opencv
$ cd opencv
$ git clone https://github.com/opencv/opencv.git
$ cd opencv
$ git checkout tags/4.2.0
$ cd ..
$ git clone https://github.com/opencv/opencv_contrib.git
$ cd opencv_contrib 
$ git checkout tags/4.2.0
$ cd ..
```

Next I create two more directories 
- (i) an `install` directory which will be the target for my installation of OpenCV and, 
- (ii) another named `build_opencv` for building the sources in.

```shell
$ mkdir install build_opencv
$ ls -l
drwxr-xr-x  44 wuriyanto  staff  1408 Dec 20 21:04 build_opencv
drwxr-xr-x   6 wuriyanto  staff   192 Dec 20 21:04 install
drwxr-xr-x  22 wuriyanto  staff   704 Dec 20 20:41 opencv
drwxr-xr-x  13 wuriyanto  staff   416 Dec 20 20:40 opencv_contrib
```

Next I change directories into the `build_opencv` directory and configure `CMake` as shown below.

```shell
$ cd build_opencv
$ cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=../install \
      -D INSTALL_C_EXAMPLES=ON \
      -D OPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules \
      -D BUILD_EXAMPLES=ON ../opencv
```

The last steps to install is to kick off the multithreaded build to compile the project followed by installing it into the install directory as seen below.

```shell
$ export CPUS=$(sysctl -n hw.physicalcpu)
$ make -j $CPUS
$ make install
```