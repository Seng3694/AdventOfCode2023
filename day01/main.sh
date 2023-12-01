#!/bin/bash

paste -d '' \
    <(grep -Po '^[^\d]*\d' day01/input.txt | grep -Po '\d') \
    <(grep -Po '\d[^\d]*$' day01/input.txt | grep -Po '\d') \
    | paste -sd '+' \
    | bc
