#!/bin/bash
seq=(1105 2205 3305 4405 6605 8805 13206 17606 26407 35208 52810 70412 105616 140820 211228 281636 422452 563268 844900 1126532 1689796 2253060 3379588 4506116 6759172 9012228 13518340)

echo $seq
make clean > /dev/null
python3 mkcsv.py csv.csv 1000 > /dev/null
make csv2heapfile "select" > /dev/null
for s in ${seq[@]}; do
    echo Page size: $s
    ./csv2heapfile csv.csv savehere.heap $s
    ./select savehere.heap 5 NIK NIK $s > /dev/null
    ./select savehere.heap 5 N V $s > /dev/null
    ./select savehere.heap 5 A ZZZZZ $s > /dev/null
    echo
done