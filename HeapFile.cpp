#include "HeapFile.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cassert>

char* fseekread(FILE *f, long offset, int length){
    fseek(f, offset, SEEK_SET);
    char* b = new char[length];
    fread(b, 1, length, f);
    return b;
}

void fseekwrite(FILE *f, long offset, char c){
    fseek(f, offset, SEEK_SET);
    char b[1];
    b[0] = c;
    fwrite(b, 1, 1, f);
}

void fseekwrite_bytes(FILE *f, long offset, char* c, int length) {
    fseek(f, offset, SEEK_SET);
    fwrite(c, 1, length, f);
}

long fscanlong(FILE *f, long offset){
    fseek(f, offset, SEEK_SET);
    int long_size = sizeof(long);
    char b[long_size];
    fread(b, 1, long_size, f);
    long partial = 0;
    for(int i = long_size - 1; i >= 0; i--){
        partial <<= 8;
        partial |= 0xff & b[i];
    }
    return partial;
}

int fscanint(FILE *f, long offset){
    fseek(f, offset, SEEK_SET);
    int int_size = sizeof(int);
    char b[int_size];
    fread(b, 1, int_size, f);
    int partial = 0;
    for(int i = int_size - 1; i >= 0; i--){
        partial <<= 8;
        partial |= 0xff & b[i];
    }
    return partial;
}

void fprintlong(FILE *f, long offset, long data){
    fseek(f, offset, SEEK_SET);
    int long_size = sizeof(long);
    char b[long_size];
    for(int i = 0; i < long_size; i++){
        b[i] = (data >> (8 * i)) & 0xff;
    }
    {
        fwrite(b, 1, long_size, f);
    }
}

void fprintint(FILE *f, long offset, int data){
    fseek(f, offset, SEEK_SET);
    int int_size = sizeof(int);
    char b[int_size];
    for(int i = 0; i < int_size; i++){
        b[i] = (data >> (8 * i)) & 0xff;
    }
    fwrite(b, 1, int_size, f);
}

void init_heapfile_with_offset(Heapfile *heapfile, int page_size, FILE *file, long offset) {
    int space_needed = sizeof(Heapfile *) + PAGES_IN_HEAPFILE * (8 + 4);

    heapfile->file_ptr = file;
    heapfile->file_offset = offset;
    heapfile->page_size = page_size;

    char heapfile_data[space_needed];
    memset(heapfile_data, 0, sizeof(heapfile_data));

    for(int i = 0; i < PAGES_IN_HEAPFILE; i++){
        char* n_position = (char*) &heapfile_data + 16 + i * (8 + 4);
        int* n_ptr_int = (int*) n_position;
        *n_ptr_int = -1;
    }

    fseek(file, offset, SEEK_SET);
    // 0 0 -1 0 -1 0 -1 0 -1 ...
    fwrite(heapfile_data, 1, space_needed, file);
}

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
    init_heapfile_with_offset(heapfile, page_size, file, 0L);
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile) {
    long next_heapfile_offset;
    fseek(heapfile->file_ptr, heapfile->file_offset, SEEK_SET);

    FILE *f = heapfile->file_ptr;

    next_heapfile_offset = fscanlong(f, heapfile->file_offset);
    if(next_heapfile_offset == 0){
        long page_data_location_in_heapfile = 0;
        int page_index = 0;
        bool found_page_location = false;
        long curr_page_position = 16;
        for(int i = 0; i < PAGES_IN_HEAPFILE; i++){
            long capacity_available = fscanint(f, curr_page_position);
            if(capacity_available == -1){
                page_data_location_in_heapfile = curr_page_position - 8;
                found_page_location = true;
                page_index = i;
                break;
            }
            curr_page_position += 8 + 4;
        }

        // 0 -1 -> ptr M
        if(found_page_location){
            // new page
            Page p;
            init_fixed_len_page(&p, heapfile->page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);

            // write page to disk
            fseek(f, 0L, SEEK_END);
            long page_data_location = 1208;

            fseekwrite_bytes(f, page_data_location, (char*) p.data, heapfile->page_size);

            // deallocate page
            delete[] (char*) p.data;

            fprintlong(f, page_data_location_in_heapfile, page_data_location);
            long M = fixed_len_page_capacity(&p);
            fprintint(f, page_data_location_in_heapfile + 8, M);
            return page_index;
        } else {
            Heapfile heap;

            fseek(f, 0L, SEEK_END);
            long heap_data_location = ftell(f);
            init_heapfile_with_offset(&heap, heapfile->page_size, f, heap_data_location);
            fprintlong(f, heapfile->file_offset, heap_data_location);

            Heapfile next_heapfile;
            next_heapfile.file_ptr = heapfile->file_ptr;
            next_heapfile.file_offset = heap_data_location;
            next_heapfile.page_size = heapfile->page_size;
            return PAGES_IN_HEAPFILE + alloc_page(&next_heapfile);
        }
    } else {
        Heapfile next_heapfile;
        next_heapfile.file_ptr = heapfile->file_ptr;
        next_heapfile.file_offset = next_heapfile_offset;
        next_heapfile.page_size = heapfile->page_size;
        return PAGES_IN_HEAPFILE + alloc_page(&next_heapfile);
    }
}

void read_page_at_offset(Heapfile *heapfile, long data_offset, Page *page){
    page->page_size = heapfile->page_size;
    page->slot_size = NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE;
    page->data = fseekread(heapfile->file_ptr, data_offset, heapfile->page_size);
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page){
    int heaps_to_jump = pid / PAGES_IN_HEAPFILE;
    int page_in_heap_loc = pid % PAGES_IN_HEAPFILE;

    Heapfile current_heapfile = *heapfile;
    for(int i = 0; i < heaps_to_jump; i++) {
        current_heapfile.file_offset = fscanlong(heapfile->file_ptr, current_heapfile.file_offset);
        assert(current_heapfile.file_offset != 0);
    }

    long page_offset = current_heapfile.file_offset + (8 + (8+4)*page_in_heap_loc);
    assert(fscanint(heapfile->file_ptr, page_offset + 8) != -1);
    long page_data_offset = fscanlong(heapfile->file_ptr, page_offset);

    read_page_at_offset(heapfile, page_data_offset, page);
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid){
    int heaps_to_jump = pid / PAGES_IN_HEAPFILE;
    int page_in_heap_loc = pid % PAGES_IN_HEAPFILE;

    Heapfile current_heapfile = *heapfile;
    for(int i = 0; i < heaps_to_jump; i++) {
        current_heapfile.file_offset = fscanlong(heapfile->file_ptr, current_heapfile.file_offset);
        assert(current_heapfile.file_offset != 0);
    }

    long page_offset = current_heapfile.file_offset + (8 + (8+4)*page_in_heap_loc);
    assert(fscanint(heapfile->file_ptr, page_offset + 8) != -1);
    long page_data_offset = fscanlong(heapfile->file_ptr, page_offset);

    fseekwrite_bytes(heapfile->file_ptr, page_data_offset, (char*) page->data, page->page_size);

    fprintint(heapfile->file_ptr, page_offset + 8, fixed_len_page_freeslots(page));
}

HeapfileIterator::HeapfileIterator(Heapfile *_cur_heapfile) : cur_heapfile(_cur_heapfile) {}

Heapfile *HeapfileIterator::next(){
    return cur_heapfile;
}

bool HeapfileIterator::hasNext(){
    if(cur_heapfile == NULL){
        return false;
    }

    long new_heapfile_offset = fscanlong(cur_heapfile->file_ptr, cur_heapfile->file_offset);

    if(new_heapfile_offset == 0){
        cur_heapfile = NULL;
    } else {
        cur_heapfile->file_offset = new_heapfile_offset;
    }

    return cur_heapfile != NULL;
}

PageIterator::PageIterator(Heapfile *_cur_heapfile) : heapfile_iterator(_cur_heapfile),
                                                      cur_heapfile(_cur_heapfile),
                                                      cur_page(&dummy), page_index(0) {}

Page *PageIterator::next(){
    return cur_page;
}

bool PageIterator::hasNext(){
    if(cur_heapfile == NULL and !heapfile_iterator.hasNext()){
        return false;
    }

    if(cur_heapfile == NULL){
        cur_heapfile = heapfile_iterator.next();
        page_index = 0;
    }

    while(page_index < PAGES_IN_HEAPFILE){
        long page_offset = cur_heapfile->file_offset + 8 + (8 + 4) * page_index;
        long page_data_offset = fscanlong(cur_heapfile->file_ptr, page_offset);
        int page_free_slots = fscanint(cur_heapfile->file_ptr, page_offset + 8);
        int M = (8*cur_heapfile->page_size - 8*4)/(NUM_OF_ATTRIBUTES*ATTRIBUTE_SIZE*8+1);
        if(page_free_slots != -1){
            read_page_at_offset(cur_heapfile, page_data_offset, cur_page);
            page_index++;
            return true;
        }
        page_index++;
    }

    if(page_index == PAGES_IN_HEAPFILE){
        cur_heapfile = NULL;
        return hasNext();
    }

    assert(0);
}

RecordIterator::RecordIterator(Heapfile *heapfile) : page_iterator(heapfile),
                                                     cur_page(NULL),
                                                     cur_record(&dummy) {}

Record RecordIterator::next() {
    return *cur_record;
}

bool RecordIterator::hasNext() {
    if(cur_page == NULL and !page_iterator.hasNext()){
        return false;
    }

    if(cur_page == NULL){
        cur_page = page_iterator.next();
        slot_index = 0;
    }

    while(slot_index < fixed_len_page_capacity(cur_page)){
        if(fixed_len_page_slot_full(cur_page, slot_index)){
            read_fixed_len_page(cur_page, slot_index, cur_record);
            slot_index++;
            return true;
        }
        slot_index++;
    }

    if(slot_index == fixed_len_page_capacity(cur_page)){
        cur_page = NULL;
        return hasNext();
    }

    assert(0);
}
