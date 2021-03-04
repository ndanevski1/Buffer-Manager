#include "HeapFile.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iostream>

using namespace std::chrono;

void select(Heapfile h, int attribute_id, char* start, char* end, int page_size) {
    auto start_time = high_resolution_clock::now();
    
    HeapfileIterator heapfile_iterator(&h);
    while(heapfile_iterator.hasNext()) {
        Heapfile *h = heapfile_iterator.next();
        PageIterator page_iterator(h);
        while(page_iterator.hasNext()) {
            Page *p = page_iterator.next();
            RecordIterator record_iterator(p);
            while(record_iterator.hasNext()) {
                Record curr_record = record_iterator.next();
                const char *cmp_attribute = curr_record[attribute_id];
                if(strncmp(cmp_attribute, start, 5) >= 0 && strncmp(cmp_attribute, end, 5) <= 0){
                    printf("%s\n", curr_record[attribute_id]);
                }
            }
        }
    }

    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);

    std::cerr << "READ TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv){
    if(argc != 6) {
        printf("Usage: select <heapfile> <attribute_id> <start> <end> <page_size>\n");
        exit(1);
    }

    char* heap_file_name = argv[1];
    int attribute_id = atoi(argv[2]);
    char* start = argv[3];
    char* end = argv[4];
    int page_size = atoi(argv[5]);

    FILE *heap_file = fopen(heap_file_name, "r");
    Heapfile h;
    h.file_ptr = heap_file;
    h.file_offset = 0;
    h.page_size = page_size;

    select(h, attribute_id, start, end, page_size);
}