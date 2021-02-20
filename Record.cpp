#include <vector>
#include <cstring>
#include <cassert>
#include "Record.hpp"


/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    int numOfBytes = 0;
    for(auto attribute : *record)
        numOfBytes += ATTRIBUTE_SIZE;
    return numOfBytes;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {
    char *tempBuf = (char*) buf;
    for(auto attribute : *record) {
        memcpy(tempBuf, attribute, ATTRIBUTE_SIZE);
        tempBuf += ATTRIBUTE_SIZE;
    }
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
    assert(size == NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);
    record->resize(size / ATTRIBUTE_SIZE);
    char *tempBuf = (char*) buf;
    for(int i = 0; i < NUM_OF_ATTRIBUTES; i++) {
        (*record)[i] = new char[ATTRIBUTE_SIZE];
        memcpy(tempBuf, (*record)[i], ATTRIBUTE_SIZE);
        tempBuf += ATTRIBUTE_SIZE;
    }
}