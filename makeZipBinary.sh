#!/bin/sh

VERSION=471

if [ -e /tmp/eXtrema-$VERSION.zip ]; then
  echo "  makeZipBinary: /tmp/eXtrema-$VERSION.zip already exists, remove or rename to save it, aborting"
  exit 0
fi

mkdir -p /tmp/eXtrema-$VERSION/Docs
strip src/extrema.exe
cp src/extrema.exe /tmp/eXtrema-$VERSION
cp ../bin/*.dll /tmp/eXtrema-$VERSION
cp -r ../Help ../Images ../Scripts ../Fonts /tmp/eXtrema-$VERSION
cp ../doc/* /tmp/eXtrema-$VERSION/Docs

cd /tmp
zip -r eXtrema-$VERSION.zip eXtrema-$VERSION

echo "  makeZipBinary: /tmp/eXtrema-$VERSION.zip is ready."

