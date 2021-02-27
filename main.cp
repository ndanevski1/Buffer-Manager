#include "HeapFile.hpp"
#include <string>

int main(){
    Heapfile h;
    int page_size = 1005;
    FILE *f = fopen("heapfile.test", "w+");
    init_heapfile(&h, page_size, f);

    alloc_page(&h);
    ///
    Record r(100);
    for(int i = 0; i < 100; i++){
        r[i] = string_to_cstring("prtljak");
    }

    printf("%d\n", fixed_len_sizeof(&r));

    Page p;
    init_fixed_len_page(&p, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE + 5, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);

    add_fixed_len_page(&p, &r);

    write_page(&p, &h, 0);

    RecordIterator record_iterator(&h);
    Record x;
    while(record_iterator.hasNext()){
        // printf("PERAM\n");
        x = record_iterator.next();
        for(const char* j : x){
            printf("%s\n", j);
        }
    }
}