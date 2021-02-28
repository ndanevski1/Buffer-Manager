#include "HeapFile.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>

void delete_record(Heapfile h, RecordID record_id, int page_size) {
    long heapfile_offset = 0, page_offset = 0, record_offset = 0;
    int original_record_slot = record_id.slot;
    record_id.slot = 0;
    // HACK: since slot ids change, we fake the slot id
    // because record iterator only gives us existing records.
    bool found_info = get_record_info(&h, record_id, heapfile_offset, page_offset, record_offset);
    record_id.slot = original_record_slot;

    int page_index_in_heapfile = record_id.page_id % PAGES_IN_HEAPFILE;
    long capacity_location = heapfile_offset + 8 + (8 + 4) * page_index_in_heapfile + 8;
    int capacity = fscanint(h.file_ptr, capacity_location);
    fprintint(h.file_ptr, capacity_location, capacity+1);

    long bit_array_start = page_offset + page_size - 4 - 1;
    long bit_array_word = bit_array_start - original_record_slot/8;

    char* bit_array_byte = fseekread(h.file_ptr, bit_array_word, 1);
    // disables one bit from the bit array (the slot-th one).
    *bit_array_byte &= 255 ^ (1 << (record_id.slot % 8));
    fseekwrite(h.file_ptr, bit_array_word, *bit_array_byte);
}

int main(int argc, char** argv){
    if(argc != 4) {
        printf("Usage: delete <heapfile> <record_id> <page_size>\n");
        exit(1);
    }

    char* heap_file_name = argv[1];
    char* record_id_concat = argv[2];
    RecordID record_id; 
    assert(sscanf (record_id_concat,"%d_%d",&record_id.page_id, &record_id.slot) == 2);
    int page_size = atoi(argv[3]);

    FILE *heap_file = fopen(heap_file_name, "r+");
    Heapfile h;
    h.file_ptr = heap_file;
    h.file_offset = 0;
    h.page_size = page_size;
    
    delete_record(h, record_id, page_size);
}