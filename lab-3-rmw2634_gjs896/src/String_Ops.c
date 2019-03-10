#include <stdbool.h>
#include "CustomDefinitions.h"

//-------------------getString--------------------//
// Constructs a string from Console input.
// Input: buffer to hold string, length of buffer
// Output: 0 if string not complete, 1 if enter pressed or length met
//------------------------------------------------//
int getString(char* buff, int buff_length) {
  static int index = 0;
  int ch = fgetc(uart);
  if (ch != (char)EOF) {
    fputc(ch, uart);
    if (ch == '\n' || ch == '\r' || index == buff_length) {
      buff[index] = 0;
      int result = index;
      index = 0;
      return (result);
    } else if (ch == 8) {
      index--;
    } else {
      buff[index] = ch;
      index++;
    }
  }
  return (-1);
}

int _atoi(const char *str) {
    bool isNegative;
    int val;
    if ((isNegative = (*str == '-'))) { ++str; }
    for (val = 0; *str >= '0' && *str <= '9'; ++str) {
        val *= 10;
        val += *str - '0';
    }
    return isNegative ? -val : val;
}
