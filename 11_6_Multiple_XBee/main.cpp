#include "mbed.h"


DigitalOut redLED(LED1);

//RawSerial pc(USBTX, USBRX);

//RawSerial xbee(D12, D11);
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
static BufferedSerial xbee(D1, D0);

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;


void xbee_rx_interrupt(void);

void xbee_rx(void);

void reply_messange(char *xbee_reply, char *messange);


int main(){

  pc.set_baud(9600);


  char xbee_reply[3];


  // XBee setting

  xbee.set_baud(9600);

  //xbee.printf("+++");
  xbee.write("+++", 3);
  //xbee_reply[0] = xbee.getc();
  xbee.read(&xbee_reply[0], sizeof(xbee_reply[0]));
  //bee_reply[1] = xbee.getc();
  xbee.read(&xbee_reply[1], sizeof(xbee_reply[1]));

  if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){

    //pc.printf("enter AT mode.\r\n");
    printf("enter AT mode.\r\n");
    xbee_reply[0] = '\0';

    xbee_reply[1] = '\0';

  }


  // SLAVE_MY should be different between Slave A and Slave B

  //xbee.printf("ATMY <SLAVE_MY>\r\n");
  xbee.write("ATMY 0x240\r\n", 12);
  reply_messange(xbee_reply, "setting MY : 0x240");


  //xbee.printf("ATDL 0x0\r\n");
  xbee.write("ATDL 0x0\r\n", 10);
  reply_messange(xbee_reply, "setting DL : 0x0");


  //xbee.printf("ATID <PAN_ID>\r\n");
  xbee.write("ATID 0x9487\r\n", 15);
  reply_messange(xbee_reply, "setting PAN ID : 0x1");


  //xbee.printf("ATWR\r\n");
  xbee.write("ATWR\r\n", 6);
  reply_messange(xbee_reply, "write config");


  //xbee.printf("ATCN\r\n");
  xbee.write("ATCN\r\n", 6);
  reply_messange(xbee_reply, "exit AT mode");

  //xbee.getc();


  // start

  //pc.printf("start\r\n");
  printf("start\r\n");

  redLED = 1;

  t.start(callback(&queue, &EventQueue::dispatch_forever));


  // Setup a serial interrupt function of receiving data from xbee

  //xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
  xbee.set_blocking(false);

  xbee.sigio(mbed_event_queue()->event(xbee_rx));

}


void xbee_rx_interrupt(void)

{

  //xbee.attach(NULL, Serial::RxIrq); // detach interrupt

  queue.call(&xbee_rx);

}


void xbee_rx(void)

{

  //int i = 0;

  while(xbee.readable()){

    //static char receive_data = xbee.getc();
    char *receive_data = new char[1];
    xbee.read(receive_data, 1);
    //pc.printf("Receive data %c\r\n", receive_data);
    printf("Receive data %c\r\n", receive_data);
    // receive_data=='a' for Slave A

    // receive_data=='b' for Slave B

    if (*receive_data == 'a') {

      redLED = 0;

      // "A received the data." for Slave A

      // "B received the data." for Slave B

      //xbee.printf("A received the data.");
      xbee.write("A received the data.", 20);

    }

    //wait(0.5);
    ThisThread::sleep_for(500ms);

    redLED = 1;

    receive_data = 0;

  }

  //xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt

}


void reply_messange(char *xbee_reply, char *messange){

  //xbee_reply[0] = xbee.getc();

  //xbee_reply[1] = xbee.getc();

  //xbee_reply[2] = xbee.getc();
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