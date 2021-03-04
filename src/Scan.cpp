#include "HeapFile.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <chrono>
#include <iostream>

using namespace std::chrono;

void scan(FILE *heap_file, int page_size) {
    auto start = high_resolution_clock::now();
    int num_of_records = 0;
    int num_of_pages = 0;
    int num_of_heapfiles = 0;

    Heapfile h;
    h.file_ptr = heap_file;
    h.file_offset = 0;
    h.page_size = page_size;

    HeapfileIterator heapfile_iterator(&h);
    while(heapfile_iterator.hasNext()) {
        Heapfile *i = heapfile_iterator.next();
        num_of_heapfiles++;
        PageIterator page_iterator(i);
        while(page_iterator.hasNext()) {
            Page *j = page_iterator.next();
            num_of_pages++;

            RecordIterator record_iterator(j);
            while(record_iterator.hasNext()) {
                Record record = record_iterator.next();
                num_of_records++;
                for(int j = 0; j < record.size(); j++) {
                    if(j != record.size() - 1){
                        printf("%s,", record[j]);
                    } else {
                        printf("%s\n", record[j]);
                    }
                }
            }
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cerr << "NUMBER OF RECORDS: " << num_of_records << std::endl;
    std::cerr << "NUMBER OF PAGES: " << num_of_pages << std::endl;
    std::cerr << "NUMBER OF HEAPFILES: " << num_of_heapfiles << std::endl;
    std::cerr << "READ TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("Usage: scan <heapfile> <page_size>\n");
        exit(1);
    }

    char* heap_file_name = argv[1];
    int page_size = atoi(argv[2]);

    FILE *heap_file = fopen(heap_file_name, "r");
    scan(heap_file, page_size);
}