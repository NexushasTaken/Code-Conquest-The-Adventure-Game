#!/bin/bash

file="$PWD/.cache/build.zip"
mkdir -p $(dirname "$file")
if [[ ! -f "$file" ]]; then
  # https://archive.org/details/build_20250815
  curl -L -C - -o "$file" https://archive.org/download/build_20250815/build.zip
fi

dest="./app/src/main/cpp/deps/"
if [[ ! -d "$dest/curl" && ! -d "$dest/openssl" ]]; then
  unzip $file -d "$dest"
fi
