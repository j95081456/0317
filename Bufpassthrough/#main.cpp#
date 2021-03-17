/*#include  "mbed.h"
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
static BufferedSerial xbee(D1, D0);
int main()
{
  char c;
  while (1) {
    if (pc.readable()) {
      xbee.write(pc.read(&c, 1), 1);
      }
    if (xbee.readable()) {
      pc.write(device.read(&c, 1), 1);
    }    
  }
}*/
#include "mbed.h"
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
static BufferedSerial device(D1, D0);
int main()
{
    char c;
    while(1){
        if (pc.readable()){
            device.write(pc.read(&c, 1), 1);
        }
        if (device.readable()){
            pc.write(device.read(&c, 1), 1);
        }
    }
}