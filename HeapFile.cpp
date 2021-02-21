#include "HeapFile.hpp"
#include <cstdio>
#include <cstring>

struct PageHeader {
    void* page_ptr;
    int page_empty_slots;
};

char* fseekread(FILE *f, long p, int length){
    fseek(f, p, SEEK_SET);
    char* b = new char[length];
    fread(b, 1, length, f);
    return b;
}

void fseekwrite(FILE *f, long p, char c){
    fseek(f, p, SEEK_SET);
    char b[1];
    b[0] = c;
    fwrite(b, 1, 1, f);
}

void init_heapfile_with_offset(Heapfile *heapfile, int page_size, FILE *file, long offset) {
    int space_needed = sizeof(Heapfile *) + PAGES_IN_HEAPFILE * sizeof(PageHeader);

    Heapfile h;
    h.file_ptr = file;
    h.page_size = page_size;
    int M = (8*page_size - 8*4)/(ATTRIBUTE_SIZE*8+1);

    char heapfile_data[space_needed];
    memset(heapfile_data, 0, sizeof(heapfile_data));

    int ptr_size = sizeof(void*);

    for(int i = 0; i < PAGES_IN_HEAPFILE; i++){
        char* n_position = (char*) &heapfile_data + 2 * ptr_size + 2 * i * ptr_size;
        int* n_ptr_int = (int*) n_position;
        *n_ptr_int = M;
    }
    // 0 0 M 0 M 0 M 0 M ...
    fwrite(heapfile_data, 1, space_needed, file);
    rewind(file);
}


/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file) {
    int space_needed = sizeof(Heapfile *) + PAGES_IN_HEAPFILE * sizeof(PageHeader);

    Heapfile h;
    h.file_ptr = file;
    h.page_size = page_size;
    int M = (8*page_size - 8*4)/(ATTRIBUTE_SIZE*8+1);

    char heapfile_data[space_needed];
    memset(heapfile_data, 0, sizeof(heapfile_data));

    int ptr_size = sizeof(void*);

    for(int i = 0; i < PAGES_IN_HEAPFILE; i++){
        char* n_position = (char*) &heapfile_data + 2 * ptr_size + 2 * i * ptr_size;
        int* n_ptr_int = (int*) n_position;
        *n_ptr_int = M;
    }
    // 0 0 M 0 M 0 M 0 M ...
    fwrite(heapfile_data, 1, space_needed, file);
    rewind(file);
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile) {
    Heapfile* next_heapfile_ptr;
    fscanf(heapfile->file_ptr, "%p", &next_heapfile_ptr);
    if(next_heapfile_ptr == NULL){
        long page_offset = 0;
        bool found_page_location = false;
        long curr_page_position = 8;
        for(int i = 0; i < PAGES_IN_HEAPFILE; i++){
            char* c = fseekread(f, curr_page_position, 8);
            bool allZeroes = true;
            for(int j = 0; j < 8; j++){
                allZeroes &= (c[j] != 0);
            }
            if(allZeroes){
                page_offset = curr_page_position;
                found_page_location = true;
                break;
            }
            curr_page_position += 8 + 4;
        }

        // 0 M -> ptr M
        if(found_page_location){
            Page *p = new Page;
            init_fixed_len_page(p, heapfile->page_size, ATTRIBUTE_SIZE);
            
            fseek(f, 0L, SEEK_END);
            long page_data_location = ftell(f);

            for(int i = 0; i < heapfile->page_size; i++){
                fseekwrite(f, page_data_location + i, *(((char*) p->data) + i));
            }
            for(int i = 0; i < 8; i++){
                fseekwrite(f, page_offset, page_data_location & (0xff << i));
            }
        } else {
            init_heapfile
            //napraj nov heapfile ako ne
        }

    } else {
        return PAGES_IN_HEAPFILE + alloc_page(next_heapfile_ptr);
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