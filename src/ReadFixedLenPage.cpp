#include "Page.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <chrono>
#include <iostream>

using namespace std::chrono;

void read_fixed_len_page(FILE *page_file, int page_size) {
    auto start = high_resolution_clock::now();

    fseek(page_file, 0L, SEEK_END);
    int file_size = ftell(page_file);
    int record_count = 0;

    assert(file_size % page_size == 0);

    int page_count = file_size / page_size;
    for(int i = 0; i < page_count; i++){
        Page page;
        init_fixed_len_page(&page, page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);

        long page_offset = page_size * i;
        char* page_data = fseekread(page_file, page_offset, page_size);

        memcpy(page.data, page_data, page_size);
        RecordIterator record_iterator(&page);
        while(record_iterator.hasNext()) {
            Record record = record_iterator.next();
            record_count++;

            for(int j = 0; j < record.size(); j++){
                if(j != record.size() - 1){
                    printf("%s,", record[j]);
                } else {
                    printf("%s\n", record[j]);
                }
            }
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cerr << "NUMBER OF RECORDS: " << record_count << std::endl;
    std::cerr << "NUMBER OF PAGES: " << page_count << std::endl;
    std::cerr << "READ TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("Usage: read_fixed_len_page <page_file> <page_size>\n");
        exit(1);
    }

    char* page_file_name = argv[1];
    int page_size = atoi(argv[2]);

    FILE *page_file = fopen(page_file_name, "r");
    read_fixed_len_page(page_file, page_size);
}