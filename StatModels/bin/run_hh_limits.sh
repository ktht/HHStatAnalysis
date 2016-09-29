#!/bin/bash

if [ $# -ne 3 ] ; then
    echo "Usage: stat_model shapes_path output_path"
    exit 1
fi

STAT_MODEL="$1"
SHAPES_PATH="$2"
OUTPUT_PATH="$3"
N_PARALLEL=4
JSON_NAME="limits"
JSON_FILE="${JSON_NAME}.json"
JSON_PATTERN="${JSON_NAME}_*.json"

create_hh_datacards --stat-model "$STAT_MODEL" --shapes-path "$SHAPES_PATH" --output-path "$OUTPUT_PATH"
RESULT=$?
if [ $RESULT -ne 0 ] ; then
    echo "Error occurred while creating hh datacards."
    exit 1
fi

cd "$OUTPUT_PATH"
combineTool.py -M T2W -i */* -o workspace.root --parallel $N_PARALLEL
combineTool.py -M Asymptotic -d */*/workspace.root --there -n .limit --parallel $N_PARALLEL
combineTool.py -M CollectLimits */*/*.limit.* --use-dirs -o "$JSON_FILE"


INPUT_FILES=( $(find . -maxdepth 1 -type f -name "$JSON_PATTERN" | sort) )

for input_file in "${INPUT_FILES[@]}" ; do
    plotLimits.py "$input_file" --auto-style --y-title "95% CL limit on #sigma (pb)" --logy
done
