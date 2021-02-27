#include "Page.hpp"
#include "utils.hpp"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono> 

using namespace std::chrono; 

void append_page_to_page_file(Page &page, FILE *page_file, int page_size) {
    fseek(page_file, 0, SEEK_END);
    long file_end = ftell(page_file);
    fseekwrite_bytes(page_file, file_end, (char*) page.data, page_size);
}

void write_fixed_len_pages(std::ifstream &csv_file, FILE *page_file, int page_size) {
    auto start = high_resolution_clock::now();
    int num_of_records = 0;
    int num_of_pages = 1;

    std::string line;
    Page page;
    assert(page_size > NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    init_fixed_len_page(&page, page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    while (getline(csv_file, line)) {
        // std::cout << line << std::endl;
        Record row;
        std::stringstream s_stream(line); //create string stream from the string
        while(s_stream.good()) {
            std::string substr;
            getline(s_stream, substr, ','); //get first string delimited by comma
            row.push_back(string_to_cstring(substr, ATTRIBUTE_SIZE));
        }
        num_of_records++;

        if(add_fixed_len_page(&page, &row) == -1) {
            append_page_to_page_file(page, page_file, page_size);
            init_fixed_len_page(&page, page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
            add_fixed_len_page(&page, &row);
            num_of_pages++;
        }
    }
    append_page_to_page_file(page, page_file, page_size);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    std::cout << "NUMBER OF RECORDS: " << num_of_records << std::endl;
    std::cout << "NUMBER OF PAGES: " << num_of_pages << std::endl;
    std::cout << "WRITE TIME: " << duration.count() << " microseconds." << std::endl;
}

int main(int argc, char** argv) {
    if(argc != 4){
        printf("Usage: write_fixed_len_pages <csv_file> <page_file> <page_size>\n");
        exit(1);
    }
    char *csv_file_name = argv[1];
    char *page_file_name = argv[2];
    int page_size = atoi(argv[3]);
    std::ifstream csv_file (csv_file_name);
    FILE *page_file = fopen(page_file_name, "w+");

    write_fixed_len_pages(csv_file, page_file, page_size);

    csv_file.close();
    fclose(page_file);
}