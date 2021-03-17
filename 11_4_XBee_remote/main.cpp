#include "mbed.h"
#include <string>
//#include <iostream>
using namespace std;

//RawSerial pc(USBTX, USBRX);
//RawSerial xbee(D12, D11);
//UnbufferedSerial pc(USBTX, USBRX);
//UnbufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
//UnbufferedSerial xbee(D1, D0);
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
//static BufferedSerial pc(USBTX, USBRX);
static BufferedSerial xbee(D1, D0);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);

int main(){
  pc.set_baud(9600);

  char xbee_reply[4];

  //printf("write +++\r\n");
  // XBee setting
  xbee.set_baud(9600);
  //xbee.printf("+++");
  //char buffer[] = "+++";
  //xbee.write(buffer, sizeof(buffer));
  xbee.write("+++", 3);
  //printf("write %s size:%d\r\n", buffer, sizeof(buffer));
  //xbee_reply[0] = xbee.getc();
  xbee.read(&xbee_reply[0], sizeof(xbee_reply[0]));
  //xbee_reply[0] = 'O';
  //printf("xbee_reply[0]:%c\r\n", xbee_reply[0]);
  //xbee_reply[1] = xbee.getc();
  xbee.read(&xbee_reply[1], sizeof(xbee_reply[1]));
  //xbee_reply[1] = 'K';
  //printf("%s\r\n", &xbee_reply[0]);
  if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
    //pc.printf("enter AT mode.\r\n");
    printf("enter AT mode.\r\n");
    //printf("%s\r\n", &xbee_reply[0]);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
  }
  //string s;

  //xbee.printf("ATMY <REMOTE_MY>\r\n");
  //s = "ATMY 0x140";
  //cout << "string?" << endl;
  //printf("%d\r\n", s.size());
  //printf("set MY\r\n");
  char buffer1[] = "ATMY 0x240\r\n";
  printf("print %s", buffer1);
  xbee.write(buffer1, sizeof(buffer1));
  reply_messange(xbee_reply, "setting MY : 0x240");

  //xbee.printf("ATDL <REMOTE_DL>\r\n");
  //s = "ATDL 0x240";
  xbee.write("ATDL 0x140\r\n", 12);
  reply_messange(xbee_reply, "setting DL : 0x140");

  //xbee.printf("ATID <PAN_ID>\r\n");
  //s = "ATID 0x1";
  xbee.write("ATID 0x2\r\n", 10);
  reply_messange(xbee_reply, "setting PAN ID : 0x2");

  //xbee.printf("ATWR\r\n");
  //s = "ATWR";
  xbee.write("ATWR\r\n", 6);
  reply_messange(xbee_reply, "write config");

  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';

  //xbee.printf("ATMY\r\n");
  //s = "ATMY";
  xbee.write("ATMY\r\n", 6);
  //printf("%c\r\n", xbee_reply[1]);
  check_addr(xbee_reply, "MY");

  //xbee.printf("ATDL\r\n");
  //s = "ATDL";
  xbee.write("ATDL\r\n", 6);
  check_addr(xbee_reply, "DL");

  //xbee.printf("ATCN\r\n");
  //s = "ATCN";
  xbee.write("ATCN\r\n", 6);
  reply_messange(xbee_reply, "exit AT mode");
  //****************************************************xbee.getc();
  //char *c = new char[1];
  //xbee.read(c, 1);

  // start
  //pc.printf("start\r\n");
  printf("start\r\n");
  t.start(callback(&queue, &EventQueue::dispatch_forever));

  // Setup a serial interrupt function to receive data from xbee
  //xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
  //xbee.attach(xbee_rx_interrupt, SerialBase::RxIrq);
  xbee.set_blocking(false);
  //xbee.sigio(mbed_event_queue()->event(xbee_rx_interrupt));
  xbee.sigio(mbed_event_queue()->event(xbee_rx));
  //xbee.sigio(Callback<void()> xbee_rx_interrupt);
  //xbee.sigio(queue.event(xbee_rx_interrupt));

}

/*void xbee_rx_interrupt(void)
{
  //xbee.attach(NULL, SerialBase::RxIrq); // detach interrupt
  queue.call(&xbee_rx);
}*/

void xbee_rx(void)
{
  static int i = 0;
  static char buf[100] = {0};
  while(xbee.readable()){
    //char c = xbee.getc();
    char *c = new char[1];
    xbee.read(c, 1);
    if(*c!='\r' && *c!='\n'){
      buf[i] = *c;
      i++;
      buf[i] = '\0';
    }else{
      i = 0;
      //pc.printf("Get: %s\r\n", buf);
      printf("Get: %s\r\n", buf);
      //xbee.printf("%s", buf);
      xbee.write(buf, sizeof(buf));
    }
  }
  //wait(0.1);
  ThisThread::sleep_for(100ms);
  //xbee.attach(xbee_rx_interrupt, SerialBase::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
  //xbee_reply[0] = xbee.getc();
  //xbee_reply[1] = xbee.getc();
  //xbee_reply[2] = xbee.getc();
  //printf("reply_message start\r\n");
  xbee.read(&xbee_reply[0], 1);
  xbee.read(&xbee_reply[1], 1);
  xbee.read(&xbee_reply[2], 1);
  //printf("reply_message ...\r\n");
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
   //pc.printf("%s\r\n", messange);
   printf("%s\r\n", messange);
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
   xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
  //xbee_reply[0] = xbee.getc();
  //xbee_reply[1] = xbee.getc();
  //xbee_reply[2] = xbee.getc();
  //xbee_reply[3] = xbee.getc();
  xbee.read(&xbee_reply[0], 1);
  xbee.read(&xbee_reply[1], 1);
  xbee.read(&xbee_reply[2], 1);
  xbee.read(&xbee_reply[3], 1);
  //pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}