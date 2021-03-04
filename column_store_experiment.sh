#!/bin/bash
seq=(1105 2205 3305 4405 6605 8805 13206 17606 26407 35208 52810 70412 105616 140820 211228 281636 422452 563268 844900 1126532 1689796 2253060 3379588 4506116 6759172 9012228 13518340)

echo $seq
make clean > /dev/null
python3 mkcsv.py csv.csv 1000 > /dev/null
make csv2colstore select2 select3 > /dev/null
for s in ${seq[@]}; do
    echo Page size: $s
    rm -r -f col.col
    ./csv2colstore csv.csv col.col $s
    ./select2 col.col 5 NIK NIK $s > /dev/null
    ./select3 col.col 5 10 NIK NIK $s > /dev/null
    ./select2 col.col 5 N V $s > /dev/null
    ./select3 col.col 5 10 N V $s > /dev/null
    ./select2 col.col 5 A ZZZZZ $s > /dev/null
    ./select3 col.col 5 10 A ZZZZZ $s > /dev/null
    echo
done
