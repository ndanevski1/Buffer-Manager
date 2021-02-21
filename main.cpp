#include "HeapFile.hpp"
#include <string>

// long fscanlong(FILE *f, long p){
//     fseek(f, p, SEEK_SET);
//     int long_size = sizeof(long);
//     char* b = new char[long_size];
//     fread(b, 1, long_size, f);
//     long partial = 0;
//     for(int i = long_size - 1; i >= 0; i--){
//         partial = (partial << 8) | b[i];
//     }
//     return partial;
// }
// int fscanint(FILE *f, int p){
//     fseek(f, p, SEEK_SET);
//     int int_size = sizeof(int);
//     char* b = new char[int_size];
//     fread(b, 1, int_size, f);
//     int partial = 0;
//     for(int i = int_size - 1; i >= 0; i--){
//         partial = (partial << 8) | b[i];
//     }
//     return partial;
// }

const char* string_to_cstring(std::string s){
    char *c = new char[s.size() + 1];
    for(int j = 0; j < s.size(); j++){
        c[j] = s[j];
    }
    c[s.size()] = '\0';
    return (const char*) c;
}

int main(){
    Heapfile h;
    int page_size = 1000;
    FILE *f = fopen("heapfile.test", "w+");
    init_heapfile(&h, page_size, f);

    alloc_page(&h);

    Record r(100);
    for(int i = 0; i < 100; i++){
        r[i] = string_to_cstring("abcdefghi");
    }

    Page p;
    init_fixed_len_page(&p, NUM_OF_ATTRIBUTES * ATTRIBUTE_SIZE, ATTRIBUTE_SIZE);

    add_fixed_len_page(&p, &r);

    write_page(&p, &h, 0);

    RecordIterator record_iterator(&h);
    Record x;
    while(record_iterator.hasNext()){
        x = record_iterator.next();
        for(const char* j : x){
            printf("%s", j);
        }
    }
}