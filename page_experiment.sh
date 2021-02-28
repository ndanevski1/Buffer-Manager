#!/bin/fish
set seq 1115 2215 3315 4415 6615 8815 13215 17615 26415 35215 52815 70415

for s in $seq
    make clean > /dev/null
    make write_fixed_len_pages read_fixed_len_page > /dev/null
    python3 mkcsv.py csv.csv 1000 > /dev/null
    echo Page size: $s
    ./write_fixed_len_pages csv.csv page.page $s
    ./read_fixed_len_page page.page $s > /dev/null
    echo
end

