#include "utils.hpp"
#include "HeapFile.hpp"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

using namespace std::chrono;

void csv2heapfile(std::ifstream &csv_file, FILE *heap_file, int page_size) {
    auto start = high_resolution_clock::now();
    int num_of_records = 0;
    int num_of_pages = 1;

    Heapfile h;
    init_heapfile(&h, page_size, heap_file);

    std::string line;
    Page page;
    assert(page_size > NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    init_fixed_len_page(&page, page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    while (getline(csv_file, line)) {
        Record row;
        std::stringstream s_stream(line); //create string stream from the string
        while(s_stream.good()) {
            std::string substr;
            getline(s_stream, substr, ','); //get first string delimited by comma
            row.push_back(string_to_cstring(substr, ATTRIBUTE_SIZE));
        }
        num_of_records++;

        if(add_fixed_len_page(&page, &row) == -1) {
            PageID pid = alloc_page(&h);
            write_page(&page, &h, pid);
            init_fixed_len_page(&page, page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
            add_fixed_len_page(&page, &row);
            num_of_pages++;
        }
    }
    PageID pid = alloc_page(&h);
    write_page(&page, &h, pid);
    int num_of_heapfiles = pid / PAGES_IN_HEAPFILE + (pid % PAGES_IN_HEAPFILE != 0);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "NUMBER OF RECORDS: " << num_of_records << std::endl;
    std::cout << "NUMBER OF PAGES: " << num_of_pages << std::endl;
    std::cout << "NUMBER OF HEAPFILES: " << num_of_heapfiles << std::endl;
    std::cout << "WRITE TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv) {
    if(argc != 4){
        printf("Usage: csv2heapfile <csv_file> <heapfile> <page_size>\n");
        exit(1);
    }
    char *csv_file_name = argv[1];
    char *heap_file_name = argv[2];
    int page_size = atoi(argv[3]);
    std::ifstream csv_file (csv_file_name);
    FILE *heap_file = fopen(heap_file_name, "w+");

    csv2heapfile(csv_file, heap_file, page_size);

    csv_file.close();
    fclose(heap_file);
}