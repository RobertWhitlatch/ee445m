#include "CustomDefinitions.h"

//-------------------getString--------------------//
// Constructs a string from Console input.
// Input: buffer to hold string, length of buffer
// Output: 0 if string not complete, 1 if enter pressed or length met
//------------------------------------------------//
int getString(char* buff, int buff_length){
    static int index = 0;
    char ch = fgetc(uart_cmd);
    fputc(ch, uart_cmd);
    if(ch > -1){
        if(ch == '\n' || ch == '\r' || index == buff_length){
            buff[index] = 0;
            int result = index;
            index = 0;
            return (result);
        }else if(ch == 8){
            index--;
        } else {
            buff[index] = ch;
            index++;
        }
    }
    return(-1);
}
