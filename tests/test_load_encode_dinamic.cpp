#include <Python.h>
#include <fstream>
#include <iostream>
// #include "../src/encrypt/encrypt.hpp"


#include <stdio.h>
#include <dlfcn.h>

#include <cstring>
#include <memory>
using namespace std;

char * load_source (const char* name) {
  /*
  Загрузить текстовый файл
  */
  streampos size;
  char * memblock;

  ifstream file (name, ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    size = file.tellg();
    char * memblock = (char *)malloc( size * sizeof( char ) );
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    memblock[size] = 0;
    return memblock;
  }
  return nullptr;
}

// extern "C" {
int main(int argc, char **argv) {
    /*
    Тест коррекности шифровки расшифровки с доступом к либе из С++
    */

    // загрузка библиотеки шифровки/расшифровки
    std::string lib_name = "../src/encrypt/libencrypt.so";

    void *lib = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!lib) {
        printf("cannot open library '%s'\n", "libencrypt.so");
        return 0;
    }

    typedef char * (*encode_decode_t)(char *, char, size_t); 
    encode_decode_t encode = (encode_decode_t)dlsym(lib, "encode");
    encode_decode_t decode = (encode_decode_t)dlsym(lib, "decode");

    // Загрузим текстовый файл
    char* origin_str = load_source("res/test_main.py");
    std::cout << "len origin_str " << strlen(origin_str) << "\n";
    char key = '\x01';

    // Зашифруем
    size_t size = strlen(origin_str);
    char* enc = encode(origin_str, key, size);
    std::cout << "len encode_str " << strlen(enc) << "\n";

    // Расшифруем
    size = strlen(enc);
    char* dec = decode(enc, key, size);   
    std::cout << "len decode_str " << strlen(dec) << "\n";

    // Сравним
    assert(!strcmp(origin_str, dec));

    std::cout << "Test passed" << "\n\n";
    return 0;
}
// }