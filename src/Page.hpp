#include "Record.hpp"

typedef struct {
    void *data;
    int page_size;
    int slot_size;
} Page;

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);

/**
 * Returns true if the given slot is filled.
 */
bool fixed_len_page_slot_full(Page *page, int slot);

/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r);

class RecordIterator {
    private:
        Page *cur_page;
        Record *cur_record;
        Record dummy;
        int slot_index;
    public:
        RecordIterator(Page *page);
        Record next();
        int nextSlot();
        bool hasNext();
};
