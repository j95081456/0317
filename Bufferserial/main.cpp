#include "mbed.h"

//static BufferedSerial pc(USBTX, USBRX); // tx, rx
UnbufferedSerial pc(USBTX, USBRX);
PwmOut led(LED1);

float brightness = 0.0;

int main()
{
    char msg[] = "Press 'u' to turn LED1 brightness up, 'd' to turn it down\n";
    char *c = new char[1];
    pc.write(msg, sizeof(msg));
    printf("size:%d\r\n", sizeof(msg));

    while (1) {
        pc.read(c, sizeof(c));
        printf("%d\r\n", sizeof(c));
        printf("11111\r\n");
        printf("%c\r\n",*c);
        ThisThread::sleep_for(1);
        //pc.write(c, sizeof(c));
        printf("222222\n");
        printf("%c\n",*c);
        ThisThread::sleep_for(1);
        if ((*c == 'u') && (brightness < 0.5)) {
            brightness += 0.01;
            led = brightness;
        }
        if ((*c == 'd') && (brightness > 0.0)) {
            brightness -= 0.01;
            led = brightness;
        }
    }

}