#!/usr/bin/env bash

FILE_NAME="NULL"

#https://stackoverflow.com/questions/17986615/one-command-to-create-a-directory-and-file-inside-it-linux-command
atouch() {
  mkdir -p $(sed 's/\(.*\)\/.*/\1/' <<< ${FILE_NAME}) && touch ${FILE_NAME}
}

if [ "$#" -ne 2 ]; then
  echo "Expected arg count is 2"
  exit 1
fi

FILE_NAME=$1
WRITE_STR=$2

atouch
echo "${WRITE_STR}" >"${FILE_NAME}"

if [ $? -ne 0 ]; then
  exit 1
fi
