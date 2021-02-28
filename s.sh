#!/bin/bash
make clean
python3 mkcsv.py csv.csv 2000
make csv2heapfile
./csv2heapfile csv.csv savehere.heap 2215
make scan
./scan savehere.heap 2215 > out.rec