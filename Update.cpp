#include "HeapFile.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>

void update(Heapfile h, RecordID record_id, int attribute_id, char* new_value, int  page_size) {
    long heapfile_offset = 0, page_offset = 0, record_offset = 0;
    bool found_info = get_record_info(&h, record_id, heapfile_offset, page_offset, record_offset);

    assert(found_info);
    long attribute_offset = record_offset + attribute_id * ATTRIBUTE_SIZE;
    fseekwrite_bytes(h.file_ptr, attribute_offset, new_value, strlen(new_value) + 1);
}

int main(int argc, char** argv){
    if(argc != 6) {
        printf("Usage: update <heapfile> <record_id> <attribute_id> <new_value> <page_size>\n");
        exit(1);
    }

    char* heap_file_name = argv[1];
    char* record_id_concat = argv[2];
    RecordID record_id; 
    assert(sscanf (record_id_concat,"%d_%d",&record_id.page_id, &record_id.slot) == 2);
    int attribute_id = atoi(argv[3]);
    char* new_value = argv[4];
    int page_size = atoi(argv[5]);

    FILE *heap_file = fopen(heap_file_name, "r+");
    Heapfile h;
    h.file_ptr = heap_file;
    h.file_offset = 0;
    h.page_size = page_size;
    
    update(h, record_id, attribute_id, new_value, page_size);
}