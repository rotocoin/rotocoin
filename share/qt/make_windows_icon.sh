#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/rotocoin.png
ICON_DST=../../src/qt/res/icons/rotocoin.ico
convert ${ICON_SRC} -resize 16x16 rotocoin-16.png
convert ${ICON_SRC} -resize 32x32 rotocoin-32.png
convert ${ICON_SRC} -resize 48x48 rotocoin-48.png
convert rotocoin-16.png rotocoin-32.png rotocoin-48.png ${ICON_DST}

