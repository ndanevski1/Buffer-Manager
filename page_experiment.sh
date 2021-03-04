#!/bin/bash
seq=(1105 2205 3305 4405 6605 8805 13206 17606 26407 35208 52810 70412 105616 140820 211228 281636 422452 563268 844900 1126532 1689796 2253060 3379588 4506116 6759172 9012228 13518340)

echo $seq
make clean > /dev/null
python3 mkcsv.py csv.csv 20000 > /dev/null
make write_fixed_len_pages read_fixed_len_page > /dev/null
for s in ${seq[@]}; do
    echo Page size: $s
    ./write_fixed_len_pages csv.csv page.page $s
    ./read_fixed_len_page page.page $s > temp.o
    echo
done
