#include "CSVUtils.hpp"
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
    int num_of_heapfiles = 0;

    Heapfile h;
    init_heapfile(&h, page_size, heap_file);

    add_csv_to_heapfile(csv_file, h, page_size, num_of_records, num_of_pages, num_of_heapfiles);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "NUMBER OF RECORDS: " << num_of_records << std::endl;
    std::cout << "NUMBER OF PAGES: " << num_of_pages << std::endl;
    std::cout << "NUMBER OF HEAPFILES: " << num_of_heapfiles << std::endl;
    std::cout << "CREATE TIME: " << duration.count() << " microseconds." << std::endl;
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