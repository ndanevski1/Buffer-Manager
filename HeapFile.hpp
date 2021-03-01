#include "Page.hpp"
#include "utils.hpp"
#include <cstdio>

typedef struct {
	FILE *file_ptr;
	long file_offset;
	int page_size;
} Heapfile;

typedef int PageID;

typedef struct {
	PageID page_id;
	int slot;
} RecordID;

const int PAGES_IN_HEAPFILE = 100;

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

/**
 * afafs
 */
bool get_record_info(Heapfile *heapfile, RecordID record_id,
                     long &heapfile_offset, long &page_offset, long &record_offset);

RecordID select_record_with_attribute_between(
    Heapfile *heapfile, int attribute_id, char *start, char *end, Record *record);

class HeapfileIterator {
    private:
        Heapfile *cur_heapfile;
        Heapfile curr_result;
    public:
        HeapfileIterator(Heapfile *_cur_heapfile);
        Heapfile *next();
        bool hasNext();
};

class PageIterator {
    private:
        Heapfile *cur_heapfile;
        Page *cur_page;
        Page dummy;
        int page_index;
    public:
        PageIterator(Heapfile *_cur_heapfile);
        Page *next();
        bool hasNext();
};
