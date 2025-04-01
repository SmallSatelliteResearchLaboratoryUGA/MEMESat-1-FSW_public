#!/bin/bash

# Check if the correct number of arguments is provided
if [ $# -ne 3 ]; then
  echo "Usage: $0 <input_file> <segment_size> <output_prefix>"
  exit 1
fi

input_file="$1"
segment_size="$2" # Size in bytes
output_prefix="$3"

# Check if the input file exists
if [ ! -f "$input_file" ]; then
  echo "Error: Input file '$input_file' not found."
  exit 1
fi

# Compute and display the MD5 checksum
md5sum "$input_file"

# Split the file with a numeric suffix
split -b "$segment_size" -d "$input_file" "$output_prefix"

# Check if the split command was successful
if [ $? -ne 0 ]; then
  echo "Error: Failed to split file '$input_file'."
  exit 1
fi

# Create output directory
mkdir -p out_files

# Rename the output files to add .seg suffix and move them to out_files
for file in "${output_prefix}"*; do
  if [ -f "$file" ]; then # Make sure it is a file.
    mv "$file" "out_files/${file}.seg"
  fi
done

echo "Renamed and moved output files to out_files/ with '.seg' suffix."