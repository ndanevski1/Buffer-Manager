#include "utils.hpp"
#include "HeapFile.hpp"

void add_csv_to_heapfile(std::ifstream &csv_file, Heapfile h, int page_size,
                         int &num_of_records, int &num_of_pages, int &num_of_heapfiles);