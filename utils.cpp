#include "utils.hpp"

char* fseekread(FILE *f, long offset, int length){
    fseek(f, offset, SEEK_SET);
    char* b = new char[length];
    fread(b, 1, length, f);
    return b;
}

void fseekwrite(FILE *f, long offset, char c){
    fseek(f, offset, SEEK_SET);
    char b[1];
    b[0] = c;
    fwrite(b, 1, 1, f);
}

void fseekwrite_bytes(FILE *f, long offset, char* c, int length) {
    fseek(f, offset, SEEK_SET);
    fwrite(c, 1, length, f);
}

long fscanlong(FILE *f, long offset){
    fseek(f, offset, SEEK_SET);
    int long_size = sizeof(long);
    char b[long_size];
    fread(b, 1, long_size, f);
    long partial = 0;
    for(int i = long_size - 1; i >= 0; i--){
        partial <<= 8;
        partial |= 0xff & b[i];
    }
    return partial;
}

int fscanint(FILE *f, long offset){
    fseek(f, offset, SEEK_SET);
    int int_size = sizeof(int);
    char b[int_size];
    fread(b, 1, int_size, f);
    int partial = 0;
    for(int i = int_size - 1; i >= 0; i--){
        partial <<= 8;
        partial |= 0xff & b[i];
    }
    return partial;
}

void fprintlong(FILE *f, long offset, long data){
    fseek(f, offset, SEEK_SET);
    int long_size = sizeof(long);
    char b[long_size];
    for(int i = 0; i < long_size; i++){
        b[i] = (data >> (8 * i)) & 0xff;
    }
    {
        fwrite(b, 1, long_size, f);
    }
}

void fprintint(FILE *f, long offset, int data){
    fseek(f, offset, SEEK_SET);
    int int_size = sizeof(int);
    char b[int_size];
    for(int i = 0; i < int_size; i++){
        b[i] = (data >> (8 * i)) & 0xff;
    }
    fwrite(b, 1, int_size, f);
}

const char* string_to_cstring(std::string s){
    char *c = new char[s.size() + 1];
    for(int j = 0; j < s.size(); j++){
        c[j] = s[j];
    }
    c[s.size()] = '\0';
    return (const char*) c;
}