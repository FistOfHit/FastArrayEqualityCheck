#!/bin/bash

# Check if the .cu file is provided as an argument
if [ $# -eq 0 ]; then
    echo "Please provide the path to the .cu file as an argument."
    exit 1
fi

# Get the .cu file path from the argument
cu_file=$1

# Check if the .cu file exists
if [ ! -f "$cu_file" ]; then
    echo "File '$cu_file' does not exist."
    exit 1
fi

# Extract the base name of the .cu file
base_name=$(basename "$cu_file" .cu)

# Set the output executable name
executable="${base_name}.exe"

# Compile the .cu file using nvcc
nvcc -std=c++20 -o "$executable" "$cu_file"

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the executable..."
    
    # Run the compiled executable
    ./"$executable"
else
    echo "Compilation failed."
    exit 1
fi
