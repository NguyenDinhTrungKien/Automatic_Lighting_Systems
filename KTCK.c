#include <mega328p.h>
#include <delay.h>
#include <glcd.h>
#include <i2c.h>
#include <stdio.h>
#include <font5x7.h>
#include "uart.h"
#define PIR_PIN 2      // Chan cho cam bien PIR
#define LDR_PIN 3      // Chan cho Cam bien LDR (Light Dependent Resistor)
#define RELAY_PIN 6    // Chan dieu khien relay
#define LIGHT_PIN 7    // Chan dieu khien den



void displayNumber(int number, int x, int y)
{
    char numberString[10];
    sprintf(numberString, "%d", number);
    glcd_outtextxy(x, y, numberString);
}

void main(void)
{
    GLCDINIT_t init;    
    int motionDetected = 0; // Bien theo doi viec phat hien chuyen dong
    int lightLevel = 0; // Bien theo doi muc anh sang (1 neu toi, 0 neu sang)
    int relayStatus = 0; // Bien theo doi trang thai relay (1 neu BAT, 0 neu TAT)
    int lightStatus = 0; // Bien theo doi trang thai den (1 neu BAT, 0 neu TAT)
    int relayStatusDisplayed = 0; // Co de theo doi viec hien thi trang thai relay
    int lightStatusDisplayed = 0; // Co de theo doi viec hien thi trang thai den
    int peopleCount = 0; // Bien theo doi so nguoi
    int uartDisplayed = 0; // Bien cheack cho UART
    int i;
    /*Khoi tao uart */
    uart_init(9600);
    
    
    /* Khoi tao giao dien I2C */
    i2c_init();

    /* Su dung font */
    init.font = font5x7;
    /* Khong can doc du lieu tu bo nho ngoai vi */
    init.readxmem = NULL;
    /* Khong can ghi du lieu vao bo nho ngoai vi */
    init.writexmem = NULL;
    /* Dat huong quet ngang cho man hinh Univision UG-2864HSWEG01: dao nguoc */
    init.reverse_x = SSD1306_REVX_REV;
    /* Dat huong quet doc cho man hinh Univision UG-2864HSWEG01: dao nguoc */
    init.reverse_y = SSD1306_REVY_REV;
    /* Dat gia tri cho cau hinh dau ra COM
       cho man hinh Univision UG-2864HSWEG01: xen ke */
    init.interlaced = SSD1306_INTERLACED;
    /* Dien ap Vcc duoc tao ra boi bo chuyen doi DC/DC noi bo */
    init.external_vcc = SSD1306_USE_INTERNAL_DCDC;
    /* Dat do tuong phan */
    init.contrast = SSD1306_DEFAULT_CONTRAST;
    /* Khoi tao bo dieu khien LCD va do hoa */
    glcd_init(&init);

    DDRD &= ~(1 << PIR_PIN); // Dat chan PIR_PIN la INPUT
    DDRD &= ~(1 << LDR_PIN); // Dat chan LDR_PIN la INPUT
    DDRD |= (1 << RELAY_PIN); // Dat chan RELAY_PIN la OUTPUT
    DDRD |= (1 << LIGHT_PIN); // Dat chan LIGHT_PIN la OUTPUT

    while (1)
    {
        int motion = PIND & (1 << PIR_PIN); // Doc gia tri cam bien PIR
        int light = PIND & (1 << LDR_PIN); // Doc gia tri cam bien LDR

        if (light)
        {
            lightLevel = 0; // Dat muc anh sang la 0 (sang)
        }
        else
        {
            lightLevel = 1; // Dat muc anh sang la 1 (toi)
        }

        if (motion && !lightLevel && !motionDetected )
        {
            motionDetected = 1; // Dat co phat hien chuyen dong thanh true
            
            /* Ve tren man hinh OLED */
            glcd_clear();
            glcd_outtextxyf(0, 0, "Co nguoi o day");
            put_str("Co nguoi o day");
            relayStatus = 1; // Dat trang thai relay la BAT
            lightStatus = 1; // Dat trang thai den la BAT
            PORTD &= ~(1 << RELAY_PIN); // Bat relay
            PORTD |= (1 << LIGHT_PIN); // Bat den

            // Tang so luong nguoi va hien thi tren GLCD
            peopleCount++;
            
            glcd_outtextxyf(0, 10, "So nguoi: ");
            displayNumber(peopleCount, 55, 10);
            
            
            put_str("so nguoi: ");
            put_int(peopleCount);
            // Dat lai co hien thi
            relayStatusDisplayed = 0;
            lightStatusDisplayed = 0;
            
        }
        else if ((!motion || lightLevel) && motionDetected)
        {
            motionDetected = 0; // Dat co phat hien chuyen dong thanh false
            
            /* Ve tren man hinh OLED */
            glcd_clear();
            glcd_outtextxyf(0, 0, "Khong co ai ca");
            
            put_str("Khong co ai ca");
            relayStatus = 0; // Dat trang thai relay la TAT
            lightStatus = 0; // Dat trang thai den la TAT
            PORTD |= (1 << RELAY_PIN); // Tat relay
            PORTD &= ~(1 << LIGHT_PIN); // Tat den

            // Dat lai co hien thi
            relayStatusDisplayed = 0;
            lightStatusDisplayed = 0;
        }

        if (!relayStatusDisplayed)
        {
            /* Hien thi trang thai relay tren man hinh OLED */
            glcd_outtextxyf(0, 40, "Trang thai relay: ");
            put_str("Trang thai relay: ");
            if (relayStatus)
            {
                glcd_outtext("Bat");
                put_str("Bat");
            }
            else
            {
                glcd_outtext("Tat");
                put_str("Tat");
            }
            relayStatusDisplayed = 1; // Dat co hien thi trang thai relay thanh true
        }

        if (!lightStatusDisplayed)
        {
            /* Hien thi trang thai den tren man hinh OLED */
            glcd_outtextxyf(0, 50, "Trang thai den: ");
            put_str("Trang thai den: ");
            if (lightStatus)
            {
                glcd_outtext("Bat");
                put_str("Bat");
            }
            else
            {
                glcd_outtext("Tat");
                put_str("Tat");
            }
            lightStatusDisplayed = 1; // Dat co hien thi trang thai den thanh true
        }

        
        // Ki?m tra dieu kien de hien thi thông tin chi mot lan trên UART
        if (!uartDisplayed) {
            
            // Hi?n th? thông tin "Co nguoi o day"
            put_str("Co nguoi o day\n");

            // Hi?n th? thông tin "So nguoi" và s? nguoi
            put_str("So nguoi: ");
            put_int(peopleCount);
            put_str("\n");

            // Hi?n th? thông tin "Khong co ai ca"
            put_str("Khong co ai ca\n");

            // Hi?n th? thông tin "Trang thai relay"
            put_str("Trang thai relay: ");
            if (relayStatus) {
                put_str("Bat\n");
            } else {
                put_str("Tat\n");
            }

            // Hi?n th? thông tin "Trang thai den"
            put_str("Trang thai den: ");
            if (lightStatus) {
                put_str("Bat\n");
            } else {
                put_str("Tat\n");
            }
            uartDisplayed = 1; // Ðat bien co thành true
        }
                         
        delay_ms(10);
    }
}
