#include "HeapFile.hpp"

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

int main(){
    Heapfile h;
    int page_size = 1000;
    FILE *f = fopen("heapfile.test", "w+");
    init_heapfile(&h, page_size, f);

    alloc_page(&h);

    // printf("%ld %ld %d\n", fscanlong(f, 0), fscanlong(f, 8), fscanint(f, 16));
}