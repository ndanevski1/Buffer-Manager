#include <cstdio>
#include <string>

char* fseekread(FILE *f, long offset, int length);

void fseekwrite(FILE *f, long offset, char c);

void fseekwrite_bytes(FILE *f, long offset, char* c, int length);

long fscanlong(FILE *f, long offset);

int fscanint(FILE *f, long offset);

void fprintlong(FILE *f, long offset, long data);

void fprintint(FILE *f, long offset, int data);

const char* string_to_cstring(std::string s, int allocate_bytes);
