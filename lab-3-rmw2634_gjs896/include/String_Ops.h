#include "CustomDefinitions.h"

//-------------------getString--------------------//
// Constructs a string from Console input.
// Input: buffer to hold string, length of buffer
// Output: 0 if string not complete, 1 if enter pressed or length met
//------------------------------------------------//
int getString(char* buff, int buff_length);

int _atoi(const char *str);
#define atoi _atoi
