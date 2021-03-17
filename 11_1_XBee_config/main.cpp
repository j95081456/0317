#include "mbed.h"


//RawSerial pc(USBTX, USBRX);
//Serial pc(USBTX, USBRX);
//static BufferedSerial pc(USBTX, USBRX);
static BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
//SerialBase pc(USBTX, USBRX);
//UnbufferedSerial pc(USBTX, USBRX);

//RawSerial xbee(D12, D11);
static BufferedSerial xbee(D1, D0);
//UnbufferedSerial xbee(D1, D0); // not PA_1 PA_0

EventQueue queue(32 * EVENTS_EVENT_SIZE);

Thread t;

void pc_rx_interrupt(void);

void pc_rx(void);

void xbee_rx_interrupt(void);

void xbee_rx(void);


int main()

{

  //pc.baud(9600);
  pc.set_baud(9600);

  //xbee.baud(9600);
  xbee.set_baud(9600);

  //pc.printf("Begin test\r\n");
  printf("Begin test\r\n");

  t.start(callback(&queue, &EventQueue::dispatch_forever));


  // Setup a serial interrupt function of receiving data from pc

  //pc.attach(pc_rx_interrupt, Serial::RxIrq);
  // pc.rx_irq();
  //pc.attach(pc_rx_interrupt, SerialBase::RxIrq);
  pc.set_blocking(false);
  //pc.sigio(mbed_event_queue()->event(pc_rx_interrupt));
  pc.sigio(mbed_event_queue()->event(pc_rx));

  // Setup a serial interrupt function of receiving data from xbee

  //xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
  //xbee.attach(xbee_rx_interrupt, SerialBase::RxIrq);
  xbee.set_blocking(false);
  //xbee.sigio(mbed_event_queue()->event(xbee_rx_interrupt));
  xbee.sigio(mbed_event_queue()->event(xbee_rx));
  //xbee.attach(queue.event(xbee_rx()), SerialBase::RxIrq);

}


/*void pc_rx_interrupt(void)

{

  //pc.attach(NULL, SerialBase::RxIrq); // detach interrupt

  queue.call(&pc_rx);

}*/


void pc_rx(void)

{

  static int i = 0;

  static char str[50] = {0};

  while(pc.readable()){

    //char c = pc.getc();
    char *c = new char[1];
    pc.read(c, 1);

    //printf("111111\r\n");

    if(*c!='\r' && *c!='\n'){

      //pc.putc(c);
      pc.write(c, 1);

      str[i] = *c;

      i++;

      str[i] = '\r';

      str[i+1] = '\n';

      str[i] = '\0';


      //printf("222222\r\n");

      if(strncmp(str, "+++", 3) == 0){

        //xbee.printf("+++");
        xbee.write("+++", 3);

        i = 0;

        //printf("333333\r\n");

      }

    }else{

      //printf("i = %d\r\n" , i);

      i = 0;

      //xbee.printf("%s\r\n", str);
      xbee.write(str, sizeof(str));

      //pc.printf("\r\n", str);
      printf("\r\n");
      printf("%s size:%d \r\n", str, sizeof(str));

      //printf("444444\r\n");

    }

  }

  //wait(0.001);
  ThisThread::sleep_for(1ms);

  //pc.attach(pc_rx_interrupt, SerialBase::RxIrq); // reattach interrupt
    
}


void xbee_rx_interrupt(void)

{

  //xbee.attach(NULL, SerialBase::RxIrq); // detach interrupt

  queue.call(&xbee_rx);

}


void xbee_rx(void)

{

  while(xbee.readable()){

    char *c = new char[1];
    xbee.read(c, 1);

    //printf("xbee read \r\n");

    if(*c!='\r' && *c!='\n'){

      //pc.printf("%c",c);
      printf("%c",*c);
    }else{

      //pc.printf("\r\n");
      printf("\r\n");
      printf("xbee print\r\n");

    }

  }

  //wait(0.001);
  ThisThread::sleep_for(1ms);

  //xbee.attach(xbee_rx_interrupt, SerialBase::RxIrq); // reattach interrupt
    
}