#include "ColStore.hpp"
#include "utils.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iostream>

using namespace std::chrono;

int main(int argc, char** argv){
    if(argc != 6) {
        printf("Usage: select2 <colstore_name> <attribute_id> <start> <end> <page_size>\n");
        exit(1);
    }

    char* colstore_directory = argv[1];
    int attribute_id = atoi(argv[2]);
    char* start = argv[3];
    char* end = argv[4];
    int page_size = atoi(argv[5]);

    struct stat st = {0};

    assert(stat(colstore_directory, &st) != -1);

    auto start_time = high_resolution_clock::now();
    select2(colstore_directory, attribute_id, start, end, page_size);
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);

    std::cerr << "READ TIME: " << duration.count() << " microseconds." << std::endl;
}