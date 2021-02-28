#include "CSVUtils.hpp"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

void add_csv_to_heapfile(std::ifstream &csv_file, Heapfile h, int page_size,
                         int &num_of_records, int &num_of_pages, int &num_of_heapfiles) {
    num_of_records = 0;
    num_of_pages = 1;

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
    num_of_heapfiles = pid / PAGES_IN_HEAPFILE + (pid % PAGES_IN_HEAPFILE != 0);    
}