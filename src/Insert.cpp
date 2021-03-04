#include "CSVUtils.hpp"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

using namespace std::chrono;

void insert(std::ifstream &csv_file, FILE *heap_file, int page_size) {
    auto start = high_resolution_clock::now();
    int num_of_records = 0;
    int num_of_pages = 1;
    int num_of_heapfiles = 0;

    Heapfile h;
    h.file_ptr = heap_file;
    h.file_offset = 0;
    h.page_size = page_size;
    
    add_csv_to_heapfile(csv_file, h, page_size, num_of_records, num_of_pages, num_of_heapfiles);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "NUMBER OF RECORDS: " << num_of_records << std::endl;
    std::cout << "NUMBER OF PAGES: " << num_of_pages << std::endl;
    std::cout << "TOTAL NUMBER OF HEAPFILES: " << num_of_heapfiles << std::endl;
    std::cout << "WRITE TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv) {
    if(argc != 4){
        printf("Usage: insert <heapfile> <csv_file> <page_size>\n");
        exit(1);
    }
    char *heap_file_name = argv[1];
    char *csv_file_name = argv[2];
    int page_size = atoi(argv[3]);
    std::ifstream csv_file (csv_file_name);
    // r+ keeps the current content, and allows for updating.
    // w+ would've erased all file contents, which is bad.
    FILE *heap_file = fopen(heap_file_name, "r+");

    insert(csv_file, heap_file, page_size);

    csv_file.close();
    fclose(heap_file);
}