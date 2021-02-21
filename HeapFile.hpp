#include "Page.hpp"
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

class HeapfileIterator {
    private:
        Heapfile *cur_heapfile;
    public:
        HeapfileIterator(Heapfile *_cur_heapfile);
        Heapfile *next();
        bool hasNext();
};

class PageIterator {
    private:
        HeapfileIterator heapfile_iterator;
        Heapfile *cur_heapfile;
        Page *cur_page;
        Page dummy;
        int page_index;
    public:
        PageIterator(Heapfile *_cur_heapfile);
        Page *next();
        bool hasNext();
};

class RecordIterator {
	private:
		PageIterator page_iterator;
		Page *cur_page;
		Record *cur_record;
		Record dummy;
		int slot_index;
	public:
		RecordIterator(Heapfile *heapfile);
		Record next();
		bool hasNext();
};
