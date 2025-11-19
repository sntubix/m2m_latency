#!/bin/bash

# Define your pattern (customize as needed)
PATTERN="GPIO_"$1"_IRQ:"

# Define output CSV file
OUTPUT="test_results/""$2"_measuremest_""$1".csv"

echo "Line,Seconds,Nanoseconds" > "$OUTPUT"
LINE=0

sudo dmesg | grep "$PATTERN" | while read -r line; do
    # Extract the raw timestamp in ns
    RAW_NS=$(echo "$line" | sed -n "s/.*$PATTERN\([0-9]*\).*/\1/p")

    # Calculate seconds and remaining nanoseconds
    SECONDS=$((RAW_NS / 1000000000))
    REMAINDER_NS=$((RAW_NS % 1000000000))

    # Write to CSV
    echo "$LINE,$SECONDS,$REMAINDER_NS" >> "$OUTPUT"
    ((LINE++))
done

echo "Saved $LINE entries to $OUTPUT"

