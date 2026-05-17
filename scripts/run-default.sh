#!/bin/bash

# Ensure we are executing from the scripts folder
cd "$(dirname "$0")"

# Create output directory if it doesn't exist
mkdir -p ../data/output

echo "=== RUNNING BATCH MODE ==="

for ranges_file in ../data/ranges/*.txt; do
    filename=$(basename "$ranges_file")
    
    # Map the ranges files to their corresponding registers files using a case statement
    case "$filename" in
        "ranges1.txt" | "ranges2.txt" | "ranges3.txt")
            reg_filename="registers2.txt"
            ;;
        "ranges4.txt" | "ranges5.txt")
            reg_filename="registers1.txt"
            ;;
        "ranges6.txt")
            reg_filename="registers3.txt"
            ;;
        *)
            reg_filename=""
            ;;
    esac

    if [ -n "$reg_filename" ]; then
        # E.g., turns ranges1.txt into allocation1.txt
        out_filename="allocation${filename#ranges}" 

        echo "Processing: $filename + $reg_filename -> $out_filename"
        ../build/DA-GRAPH-COLORING -b "$filename" "$reg_filename" "$out_filename"
    else
        echo "⚠️ Warning: No register mapping found for $filename"
    fi
done

echo "=== BATCH COMPLETE ==="