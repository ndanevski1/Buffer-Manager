#include <bit>
#include <cstring>
#include <cassert>
#include "Page.hpp"

Page make_page(int page_size, int slot_size) {
    Page page;
    page.page_size = page_size;
    page.slot_size = slot_size;
    page.data = new char[page_size];
    memset(page.data, 0, page_size);
    // set M
    int M = fixed_len_page_capacity(&page);
    char* end = (char*) page.data + page.page_size;
    int* mp = ((int*) end) - 1;
    *mp = M;
    return page;
}

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    *page = make_page(page_size, slot_size);
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    return (8*page->page_size - 8*4)/(page->slot_size*8+1);
}

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) {
    int M = fixed_len_page_capacity(page);
    char* bit_array_start = (char*) page -> data + page -> page_size - 4 - 1;
    char* bit_array_end = (char*) page -> data + page -> page_size - 4 - (M + 7) / 8;

    int takenSpots = 0;
    for(char* ptr = bit_array_start; ptr >= bit_array_end; ptr--) {
        takenSpots += __builtin_popcount(*ptr);
    }
    return M - takenSpots;
}

int find_first_free_slot(Page *page) {
    int M = fixed_len_page_capacity(page);
    char* bit_array_start = (char*) page -> data + page -> page_size - 4 - 1;
    char* bit_array_end = (char*) page -> data + page -> page_size - 4 - (M + 7) / 8;

    int bit_index = 0;
    for(char* ptr = bit_array_start; ptr >= bit_array_end; ptr--) {
        if(__builtin_popcount(*ptr) != 8){
            for(unsigned int i = 0, bit = 1; i < 8 and i + bit_index < M; i++, bit <<= 1){
                if(bit & (~(*ptr))){
                    bit_index += i;
                    return bit_index;
                }
            }
        }
        bit_index += 8;
    }
    return -1;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r) {
    int first_free_slot = find_first_free_slot(page);
    if(first_free_slot == -1)
        return -1;

    write_fixed_len_page(page, first_free_slot, r);

    return first_free_slot;
}

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r) {

    fixed_len_write(r, (char*) page -> data + slot * page -> slot_size);

    char* bit_array_start = (char*) page -> data + page -> page_size - 4 - 1;

    char* bit_array_word = bit_array_start - slot/8;
    *bit_array_word |= 1 << (slot % 8);

}

/**
 * Returns true if the given slot is filled.
 */
bool fixed_len_page_slot_full(Page *page, int slot){
    char* bit_array_start = (char*) page -> data + page -> page_size - 4 - 1;

    char* bit_array_word = bit_array_start - slot/8;
    return ((*bit_array_word) & (1 << (slot % 8))) != 0;
}

/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r) {
    fixed_len_read((char*) page -> data + slot * page -> slot_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE, r);
}

RecordIterator::RecordIterator(Page *page) : cur_page(page),
                                             cur_record(&dummy),
                                             slot_index(0) {}

Record RecordIterator::next() {
    return *cur_record;
}

int RecordIterator::nextSlot() {
    return slot_index - 1;
}

bool RecordIterator::hasNext() {
    if(cur_page == NULL){
        return false;
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
        return false;
    }

    assert(0);
}
