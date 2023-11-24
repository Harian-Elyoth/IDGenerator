#!/bin/bash

# Verify the existence of the 'build/' directory
if [ ! -d "build/" ]; then
    echo "Error: The 'build/' directory does not exist."
    exit 1
fi

# Remove the 'build/' directory
echo "Cleaning up..."
rm -rf build

echo "Clean-up complete."