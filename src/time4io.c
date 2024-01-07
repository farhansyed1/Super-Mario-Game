#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void){
    return (PORTD >> 8) & 0xF;
}
// btn 2,3,4 are bits 5-7
// btn 1 is bit
int getbtns(void){
    int bits = ((PORTD >> 5) & 0x7);
    //bits |= (PORTF >> 2) & 0x2;
    return bits;
}
int getbtn1(){
    return ((PORTF>>1) & 0x1)<<9;
}