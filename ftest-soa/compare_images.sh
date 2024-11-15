#!/bin/bash

# Check if the correct number of arguments are passed
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <image1.ppm> <image2.ppm>"
    exit 1
fi

IMAGE1="$1"
IMAGE2="$2"
CPP_FILE="tester.cpp"
EXECUTABLE="compare_images"

# Compile the C++ program
g++ -o "$EXECUTABLE" "$CPP_FILE" -std=c++11
if [ $? -ne 0 ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# Run the program with the provided images
./"$EXECUTABLE" "$IMAGE1" "$IMAGE2"

# Clean up the executable after running
rm -f "$EXECUTABLE"
