#!/bin/bash

magick_version=7.1.0
export CURR_DIR=$(pwd)
magick_install_dir="$CURR_DIR/vendor/image_magick"

git_url="https://github.com/ImageMagick/ImageMagick.git ImageMagick-$magick_version"

rm -rf $magick_install_dir/*
mkdir -p $magick_install_dir && cd $magick_install_dir
git clone $git_url
cd ImageMagick*

./configure \
    --prefix=$magick_install_dir
    --disable-static \
    --enable-shared \
    --with-jp2 \
    --with-jpeg \
    --with-png \
    --with-quantum-depth=8 \
    --with-rsvg \
    --with-webp \
    --without-bzlib \
    --without-djvu \
    --without-dps \
    --without-fftw \
    --without-fontconfig \
    --without-freetype \
    --without-gvc \
    --without-jbig \
    --without-lcms \
    --without-lcms2 \
    --without-lqr \
    --without-lzma \
    --without-magick-plus-plus \
    --without-openexr \
    --without-pango \
    --without-perl \
    --without-tiff \
    --without-wmf \
    --without-x \
    --without-xml \
    --without-zlib
make
make install