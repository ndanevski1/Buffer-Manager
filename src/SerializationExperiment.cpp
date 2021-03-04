#include "Record.hpp"
#include <cstdio>

int main(){
    Record r;
    printf("%d bytes are needed to serialize records.\n", fixed_len_sizeof(&r));
}