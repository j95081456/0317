#include "mbed.h"
#include "mbed_rpc.h"

//RawSerial pc(USBTX, USBRX);
//RawSerial xbee(D12, D11);
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
static BufferedSerial xbee(D1, D0);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

RpcDigitalOut myled1(LED1,"myled1");
RpcDigitalOut myled2(LED2,"myled2");
RpcDigitalOut myled3(LED3,"myled3");

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);

int main(){
  //pc.baud(9600);
    pc.set_baud(9600);

  char xbee_reply[4];

  // XBee setting
  //xbee.baud(9600);
  xbee.set_baud(9600);
  //xbee.printf("+++");
  xbee.write("+++", 3);
  //xbee_reply[0] = xbee.getc();
  //xbee_reply[1] = xbee.getc();
  xbee.read(&xbee_reply[0], sizeof(xbee_reply[0]));
  xbee.read(&xbee_reply[1], sizeof(xbee_reply[1]));
  if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
    //pc.printf("enter AT mode.\r\n");
    printf("enter AT mode.\r\n");
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
  }
  //xbee.printf("ATMY <REMOTE_MY>\r\n");
  //reply_messange(xbee_reply, "setting MY : <REMOTE_MY>");
  xbee.write("ATMY 0x240\r\n", 12);
  reply_messange(xbee_reply, "setting MY : 0x240");  

  //xbee.printf("ATDL <REMOTE_DL>\r\n");
  //reply_messange(xbee_reply, "setting DL : <REMOTE_DL>");
  xbee.write("ATDL 0x140\r\n", 12);
  reply_messange(xbee_reply, "setting DL : 0x140");  

  //xbee.printf("ATID <PAN_ID>\r\n");
  //reply_messange(xbee_reply, "setting PAN ID : <PAN_ID>");
  xbee.write("ATID 0x1\r\n", 10);
  reply_messange(xbee_reply, "setting PAN ID : 0x1");  

  //xbee.printf("ATWR\r\n");
  //reply_messange(xbee_reply, "write config");
  xbee.write("ATWR\r\n", 6);
  reply_messange(xbee_reply, "write config"); 

  //xbee.printf("ATMY\r\n");
  //check_addr(xbee_reply, "MY");
  xbee.write("ATMY\r\n", 6);
  check_addr(xbee_reply, "MY");

  //xbee.printf("ATDL\r\n");
  //check_addr(xbee_reply, "DL");
  xbee.write("ATDL\r\n", 6);
  check_addr(xbee_reply, "DL");

  //xbee.printf("ATCN\r\n");
  //reply_messange(xbee_reply, "exit AT mode");
  xbee.write("ATCN\r\n", 6);
  reply_messange(xbee_reply, "exit AT mode");
  //*********************************************xbee.getc();
  // start
  //pc.printf("start\r\n");
  printf("start\r\n");
  t.start(callback(&queue, &EventQueue::dispatch_forever));

  // Setup a serial interrupt function of receiving data from xbee
  //xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
  xbee.set_blocking(false);
  xbee.sigio(mbed_event_queue()->event(xbee_rx_interrupt));
}

void xbee_rx_interrupt(void)
{
  //xbee.attach(NULL, Serial::RxIrq); // detach interrupt
  //xbee.sigio(mbed_event_queue()->event(xbee_rx_interrupt));
  queue.call(&xbee_rx);
}

void xbee_rx(void)
{
  char buf[100] = {0};
  char outbuf[100] = {0};
  while(xbee.readable()){
    for (int i=0; ; i++) {
      //char recv = xbee.getc();
      char *recv = new char[1];
      xbee.read(recv, 1);
      buf[i] = *recv;
      if (*recv == '\r') {
        break;
      }
      //buf[i] = pc.putc(recv);
      //pc.write(recv, 1);
      //pc.read(&buf[i], 1);
      //buf[i] = *recv;
    }
    printf("buf: %s\r\n", buf);
    RPC::call(buf, outbuf);
    //printf("RPC get\r\n");
    //pc.printf("%s\r\n", outbuf);
    printf("outbuf: %s\r\n", outbuf);
    //wait(0.1);
    ThisThread::sleep_for(1s);
  }
  //printf("out \r\n");
  //xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
  //xbee.sigio(mbed_event_queue()->event(xbee_rx_interrupt));
}

void reply_messange(char *xbee_reply, char *messange){
 /* xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();*/
  xbee.read(&xbee_reply[0], 1); 
  xbee.read(&xbee_reply[1], 1);
  xbee.read(&xbee_reply[2], 1);
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    //pc.printf("%s\r\n", messange);
    printf("%s\r\n", messange);
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
 /* xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();*/
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