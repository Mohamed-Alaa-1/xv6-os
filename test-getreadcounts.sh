#!/bin/bash

# Rebuild the xv6 kernel
cd src/
make clean
make > /dev/null

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Run the test
make qemu-nox <<EOF
./test_getreadcount
EOF
