#include "ColStore.hpp"
#include <cstring>

void select2(char *colstore_directory, int attribute_id, char* start, char* end, int page_size){
    return select3(colstore_directory, attribute_id, attribute_id, start, end, page_size);
}

void select3(char *colstore_directory, int filter_attribute_id, int return_attribute_id,
             char* start, char* end, int page_size){
    std::string filter_file_name = colstore_directory, select_file_name = colstore_directory;
    filter_file_name += "/"; select_file_name += "/";
    if(filter_attribute_id < 10){
        filter_file_name += "0";
    }
    if(return_attribute_id < 10){
        select_file_name += "0";
    }
    filter_file_name += std::to_string(filter_attribute_id) + ".heap";
    select_file_name += std::to_string(return_attribute_id) + ".heap";
    FILE *filter_file = fopen(filter_file_name.c_str(), "r");
    FILE *select_file = fopen(select_file_name.c_str(), "r");

    Heapfile filter_heapfile, select_heapfile;
    filter_heapfile.file_ptr = filter_file, select_heapfile.file_ptr = select_file;
    filter_heapfile.file_offset = select_heapfile.file_offset = 0;
    filter_heapfile.page_size = select_heapfile.page_size = page_size;

    HeapfileIterator hi_filter(&filter_heapfile), hi_select(&select_heapfile);
    while(hi_filter.hasNext() and hi_select.hasNext()) {
        Heapfile *h_filter = hi_filter.next(), *h_select = hi_select.next();

        PageIterator pi_filter(h_filter), pi_select(h_select);
        while(pi_filter.hasNext() and pi_select.hasNext()) {
            Page *p_filter = pi_filter.next(), *p_select = pi_select.next();

            RecordIterator ri_filter(p_filter), ri_select(p_select);
            while(ri_filter.hasNext() and ri_select.hasNext()) {
                Record filter_record = ri_filter.next(), select_record = ri_select.next();

                const char *cmp_attribute = filter_record[filter_attribute_id];
                if(strncmp(cmp_attribute, start, 5) >= 0 && strncmp(cmp_attribute, end, 5) <= 0){
                    printf("%s\n", select_record[return_attribute_id]);
                }
            }
        }
    }
}