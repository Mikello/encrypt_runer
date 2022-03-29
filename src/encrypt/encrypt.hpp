

// #ifndef _ENCODE_H_
// #define _ENCODE_H_

// #ifdef  __cplusplus
extern "C" {
// #endif

// typedef struct test_st_s test_st_t;

// struct test_st_s {
//     int val1;
//     double val2;
//     char val3;
// };

char * encode(char* name, char key, size_t size);

char * decode(char* name, char key, size_t size);

// #ifdef  __cplusplus
}
// #endif

// #endif  /* _ENCODE_H_ */