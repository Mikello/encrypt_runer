#include <stdint.h>
#include <cstring>
#include <stdlib.h>
#include <fstream>

#include "encrypt.hpp"


using namespace std;

char * encode(char* name, char key, size_t size) {
    // Кодирование исключающим ИЛИ
    char * ret = (char *)malloc( (size+1) * sizeof( char ) );

    for (size_t i = 0; i < size; i++){
        ret[i] = name[i] ^ key;
    }
    ret[size] = 0;
    return ret;
}

char * decode(char* name, char key, size_t size) {
    // Декодирование исключающим ИЛИ
    char * ret = (char *)malloc( (size+1) * sizeof( char ) );

    for (size_t i = 0; i < size; i++){
        ret[i] = name[i] ^ key;
    }
    ret[size] = 0;
    return ret;
}

// компилировать
// gcc -fPIC -shared -o libencrypt.so encrypt.cpp