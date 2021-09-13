#!/bin/sh

if [ -e /tmp/eXtrema-462.zip ]; then
  echo "  makeZipBinary: /tmp/eXtrema-462.zip already exists, remove or rename to save it, aborting"
  exit 0
fi

mkdir -p /tmp/eXtrema-462/Docs
strip src/extrema.exe
cp src/extrema.exe /tmp/eXtrema-462
cp ../bin/*.dll /tmp/eXtrema-462
cp -r ../Help ../Images ../Scripts /tmp/eXtrema-462
cp ../doc/* /tmp/eXtrema-462/Docs

cd /tmp
zip -r eXtrema-462.zip eXtrema-462

echo "  makeZipBinary: /tmp/eXtrema-462.zip is ready."

