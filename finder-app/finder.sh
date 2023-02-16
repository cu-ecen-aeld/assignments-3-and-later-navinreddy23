#!/usr/bin/env bash

if [ "$#" -ne 2 ]; then
  echo "Expected arg count is 2"
  exit 1
fi

FILES_DIR=$1
SEARCH_STR=$2

if [ ! -d "${FILES_DIR}" ]; then
  echo "Not a directory: Arg 1 should be a directory"
  exit 1
fi

TOTAL_COUNT=$(ls  ${FILES_DIR} | wc -l)
MATCH_COUNT=$(cat  ${FILES_DIR}/* | grep ${SEARCH_STR} | wc -l)
echo "The number of files are ${TOTAL_COUNT} and the number of matching lines are ${MATCH_COUNT}"
exit 0