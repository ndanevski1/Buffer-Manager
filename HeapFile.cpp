#include "HeapFile.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>

struct PageHeader {
    void* page_ptr;
    int page_empty_slots;
};

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

long fscanlong(FILE *f, long offset){
    fseek(f, offset, SEEK_SET);
    int long_size = sizeof(long);
    char b[long_size];
    fread(b, 1, long_size, f);
    long partial = 0;
    for(int i = long_size - 1; i >= 0; i--){
        partial = (partial << 8) | b[i];
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
        partial = (partial << 8) | b[i];
    }
    return partial;
}

void fprintlong(FILE *f, long offset, long data){
    fseek(f, offset, SEEK_SET);
    int long_size = sizeof(long);
    char b[long_size];
    for(int i = 0; i < long_size; i++){
        b[i] = data & (0xff << (8 * i));
    }
    std::reverse(b, b + long_size);
    fwrite(b, 1, long_size, f);
}

void fprintint(FILE *f, long offset, int data){
    fseek(f, offset, SEEK_SET);
    int int_size = sizeof(int);
    char b[int_size];
    for(int i = 0; i < int_size; i++){
        b[i] = data & (0xff << (8 * i));
    }
    std::reverse(b, b + int_size);
    fwrite(b, 1, int_size, f);
}

void init_heapfile_with_offset(Heapfile *heapfile, int page_size, FILE *file, long offset) {
    int space_needed = sizeof(Heapfile *) + PAGES_IN_HEAPFILE * sizeof(PageHeader);

    heapfile->file_ptr = file;
    heapfile->file_offset = offset;
    heapfile->page_size = page_size;
    int M = (8*page_size - 8*4)/(ATTRIBUTE_SIZE*8+1);

    char heapfile_data[space_needed];
    memset(heapfile_data, 0, sizeof(heapfile_data));

    int ptr_size = sizeof(void*);

    for(int i = 0; i < PAGES_IN_HEAPFILE; i++){
        char* n_position = (char*) &heapfile_data + 2 * ptr_size + 2 * i * ptr_size;
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
            Page *p = new Page;
            init_fixed_len_page(p, heapfile->page_size, ATTRIBUTE_SIZE);

            // write page to disk
            fseek(f, 0L, SEEK_END);
            long page_data_location = ftell(f);

            for(int i = 0; i < heapfile->page_size; i++){
                fseekwrite(f, page_data_location + i, *(((char*) p->data) + i));
            }

            // deallocate page
            delete[] (char*) p->data;
            delete p;

            fprintlong(f, page_data_location_in_heapfile, page_data_location);
            int M = (8*heapfile->page_size - 8*4)/(ATTRIBUTE_SIZE*8+1);
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

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page){
    int heaps_to_jump = pid / PAGES_IN_HEAPFILE;
    int page_in_heap_loc = pid % PAGES_IN_HEAPFILE;

    // fseek

    // fread
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid){

}

// class RecordIterator {
// 	private:
// 		Heapfile *heapfile;
// 		Page *curPage;
// 	public:
// 		RecordIterator(Heapfile *heapfile);
// 		Record next();
// 		bool hasNext();
// }