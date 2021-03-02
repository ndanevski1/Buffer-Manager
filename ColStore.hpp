#include "HeapFile.hpp"
#include "utils.hpp"

void select2(char *colstore_directory, int attribute_id, char* start, char* end, int page_size);
void select3(char *colstore_directory, int filter_attribute_id, int return_attribute_id,
             char* start, char* end, int page_size);
