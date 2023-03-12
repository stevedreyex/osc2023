#include"mini_uart.h"

/**
 * Helper function to convert ASCII octal number into binary
 * s string
 * n number of digits
 */
int oct2bin(char *s, int n)
{
    int r=0;
    while(n-->0) {
        r<<=3;
        r+=*s++-'0';
    }
    return r;
}

int hex2bin(char *s, int n){
    int r=0;
    while(n-->0) {
        r<<=4;
        if (*s<='F' && *s>='A'){
            r+=*s++-0x37;
        }
        else r+=*s++-'0';
    }
    return r;
}

void uart_int(int i){
    int offset;
    if (i == 0) {
        uart_send('0');
        return;
    }
    while(i){
        offset = i%10;
        uart_send('0'+offset);
        i/=10;
    }
}

int atoi(char * c){
    int ret = 0;
    for(int i=sizeof(c)-1; i>=0; i--){
        ret+= (c[i]-0x48);
    }
    return ret;
}

// void uart_printf(char *s, )

void* simple_malloc(void **now, int size) {
    void *ret = *now;
    *now = *(char **)now + size;
    return ret;
}