#include "HeapFile.hpp"
#include "utils.hpp"
#include <string>

int main(){
    Heapfile h;
    int page_size = 2205;
    FILE *f = fopen("heapfile.test", "w+");
    init_heapfile(&h, page_size, f);

    alloc_page(&h);
    alloc_page(&h);
    ///
    Record r(100);
    for(int i = 0; i < 100; i++){
        r[i] = string_to_cstring(std::to_string(i), ATTRIBUTE_SIZE);
        // printf("%s ", r[i]);
    }

    printf("%d\n", fixed_len_sizeof(&r));

    Page p;
    init_fixed_len_page(&p, page_size, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE);

    add_fixed_len_page(&p, &r);

    write_page(&p, &h, 0);

    for(int i = 0; i < 100; i++){
        r[i] = string_to_cstring(std::to_string(i * 100), ATTRIBUTE_SIZE);
    }

    add_fixed_len_page(&p, &r);
    write_page(&p, &h, 1);

    HeapfileIterator heapfile_iterator(&h);
    while(heapfile_iterator.hasNext()) {
        Heapfile *i = heapfile_iterator.next();

        printf("Najde heapfile\n");
        PageIterator page_iterator(i);
        while(page_iterator.hasNext()) {
            Page *j = page_iterator.next();
            printf("Najde page\n");

            RecordIterator record_iterator(j);
            while(record_iterator.hasNext()) {
                Record k = record_iterator.next();
                printf("Najde record\n");

                for(const char* l : k){
                    printf("%s\n", l);
                }
            }
        }
    }
}