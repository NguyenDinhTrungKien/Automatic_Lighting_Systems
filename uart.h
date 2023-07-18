#include <mega328p.h>
#define fosc 16000000


void uart_init(unsigned long baud) {
    /* === Cau hinh toc do BAUD === */
    long n = fosc /(16*baud) -1;
    UBRR0H = n >> 8;
    UBRR0L = n;
    
    /* === Cau hinh che do truyen , khung ban tin bang thanh ghi UCSRnC === */
    UCSR0C = 0b00000110; // Truyen khong dong bo, 8 data bit, 1 stop bit, no parity
    UCSR0B = 0b10011000; // Cho phep ca 2 chieu truyen va nhan
    
    #asm("sei")
}

void put_char(unsigned char data) {
    while (!(UCSR0A & 0b00100000)); //Cho thanh ghi du lieu Empty
    UDR0 = data;
}

void put_str(unsigned char *str){
    while(*str){
        put_char(*str);
        if(*str == '\n')
        put_char('\r');
        str++;
    }
}

void put_int(unsigned long value) {
    unsigned char buf[12];
    int index = 0, i;
    long j = value;
    do
      {
            buf[index] = j%10 + 48; // Chuyen gia tri sang ky tu
            j = j/10;
            index++;
      }
    while (j);      
    
    for(i = index; i > 0; i--) {
        put_char(buf[i-1]);
    }
} 
