#include "CSVUtils.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>

using namespace std::chrono;

Record create_temp_record(int attribute_index, const char *attribute){
    Record temp_record(NUM_OF_ATTRIBUTES);
    for(int i = 0; i < NUM_OF_ATTRIBUTES; i++){
        temp_record[i] = "\0\0\0\0\0\0\0\0\0\0";
    }
    temp_record[attribute_index] = attribute;
    return temp_record;
}
void csv2colstore(std::ifstream &csv_file, char *colstore_directory, int page_size) {
    auto start = high_resolution_clock::now();

    Heapfile attribute_heapfiles[NUM_OF_ATTRIBUTES];
    for(int i = 0; i < NUM_OF_ATTRIBUTES; i++){
        std::string s = colstore_directory;
        s += "/";
        if(i < 10){
            s += "0";
        }
        s += std::to_string(i) + ".heap";
        FILE *current_attribute_file = fopen(s.c_str(), "w+");
        init_heapfile(&attribute_heapfiles[i], page_size, current_attribute_file);
    }

    int num_of_records = 0;
    int num_of_pages;
    int num_of_heapfiles;

    std::string line;
    Page pages[NUM_OF_ATTRIBUTES];
    assert(page_size > NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    for(int i = 0; i < NUM_OF_ATTRIBUTES; i++){
        init_fixed_len_page(&pages[i], page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    }
    while (getline(csv_file, line)) {
        Record row;
        std::stringstream s_stream(line); //create string stream from the string
        while(s_stream.good()) {
            std::string substr;
            getline(s_stream, substr, ','); //get first string delimited by comma
            row.push_back(string_to_cstring(substr, ATTRIBUTE_SIZE));
        }

        int pages_done = 0;
        for(int i = 0; i < NUM_OF_ATTRIBUTES; i++){
            Record temp_record = create_temp_record(i, row[i]);
            if(add_fixed_len_page(&pages[i], &temp_record) == -1)
                break;
            pages_done++;
        }
        num_of_records++;

        assert(pages_done == 0 or pages_done == NUM_OF_ATTRIBUTES);

        if(pages_done == 0){
            for(int i = 0; i < NUM_OF_ATTRIBUTES; i++){
                // send page to heapfile
                PageID pid = alloc_page(&attribute_heapfiles[i]);
                write_page(&pages[i], &attribute_heapfiles[i], pid);
                // create new page, add record to new page
                init_fixed_len_page(&pages[i], page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
                Record temp_record = create_temp_record(i, row[i]);
                add_fixed_len_page(&pages[i], &temp_record);
            }
        }
    }

    // send page to heapfile
    for(int i = 0; i < NUM_OF_ATTRIBUTES; i++){
        // send page to heapfile
        PageID pid = alloc_page(&attribute_heapfiles[i]);
        write_page(&pages[i], &attribute_heapfiles[i], pid);

        pid++;
        num_of_pages = NUM_OF_ATTRIBUTES * pid;
        num_of_heapfiles = NUM_OF_ATTRIBUTES * (pid / PAGES_IN_HEAPFILE + (pid % PAGES_IN_HEAPFILE != 0));
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "NUMBER OF RECORDS: " << num_of_records << std::endl;
    std::cout << "NUMBER OF PAGES: " << num_of_pages << std::endl;
    std::cout << "NUMBER OF HEAPFILES: " << num_of_heapfiles << std::endl;
    std::cout << "CREATE TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv) {
    if(argc != 4){
        printf("Usage: csv2colstore <csv_file> <colstore_name> <page_size>\n");
        exit(1);
    }

    char *csv_file_name = argv[1];
    char *colstore_directory = argv[2];
    int page_size = atoi(argv[3]);
    std::ifstream csv_file (csv_file_name);

    struct stat st = {0};

    assert(stat(colstore_directory, &st) == -1);
    mkdir(colstore_directory, 0700);

    csv2colstore(csv_file, colstore_directory, page_size);

    csv_file.close();
}