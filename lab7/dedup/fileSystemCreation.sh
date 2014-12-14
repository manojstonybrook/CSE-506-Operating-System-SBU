#!/bin/bash

clear

echo "Creating File System of size 10000 bytes"

echo "Your new file system is FS4"

dd if=/dev/zero of=FS4 bs=1 count=100000
