#!/bin/bash
cd "$(dirname "$0")"

# adapted from: https://askubuntu.com/a/179420
crop_pdfs () {
  for FILE in ./*.pdf; do
    pdfcrop "${FILE}" "${FILE}"
  done
}

# adapted from: https://unix.stackexchange.com/a/418992
cd gen/
COMMAND='crop_pdfs'
find `pwd` -iname "*.pdf" -printf "%h\n" | sort -u | while read i; do
    cd "$i" && pwd && $COMMAND
done