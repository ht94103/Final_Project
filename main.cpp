#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"

Ticker servo_ticker;
Ticker encoder_ticker;
Thread Receive;
Thread Send;
Thread xbee_thread;
EventQueue xbee_queue(32 * EVENTS_EVENT_SIZE);

PwmOut pin8(D8), pin9(D9);
DigitalIn pin3(D3);
RawSerial pc(USBTX, USBRX);
Serial uart(D1,D0);
RawSerial xbee(D12, D11);
BBCar car(pin8, pin9, servo_ticker);
DigitalInOut pin10(D10);
parallax_ping  ping1(pin10);
parallax_encoder encoder0(pin3, encoder_ticker);
RpcDigitalOut myled1(LED1,"myled1");

void xbee_rx_interrupt(void);
void xbee_rx(void);
void GoStraightPing(int DistanceCm);
void GoStraight(int DistanceCm);
void TurnLeft();
void TurnRight();
void TakePicture();
void ReverseParking();
void recieve_thread();
void send_thread();
void st();
void tp();

int k=0;


int main() {

    //uart.baud(9600);
    xbee.baud(9600);
    
    xbee_thread.start(callback(&xbee_queue, &EventQueue::dispatch_forever));
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
    //Receive.start(recieve_thread);
    //pc.printf("Hello, world! Test\n");
    //xbee.printf("Hello, world!");
    
    //tp();


    /*GoStraightPing(50);
    TurnLeft();
    GoStraightPing(10);
    TakePicture();
    GoStraightPing(50);
    ReverseParking();
    GoStraight(70);
    TurnRight();
    GoStraightPing(100);
    TurnRight();
    GoStraightPing(70);
    Scan();
    TurnRight();
    GoStraightPing(100);*/
}

void GoStraightPing(int DistanceCm){
    encoder0.reset();

    while(encoder0.get_cm()<DistanceCm){
        if ((float)ping1-10>1){
            car.moveR(100);
        }
        else if ((float)ping1-10<1){
            car.moveL(100);
        }
        else {
            car.goStraight(100);
        }
        wait_ms(50);
    }
    car.stop();
}

void GoStraight(int DistanceCm){
    encoder0.reset();

    car.goStraight(100);
    while(encoder0.get_cm()<DistanceCm) wait_ms(50);
    car.stop();
}


void TurnLeft(){
    wait(0.5);
    car.TurnL(100);
    wait(1.1);
    car.stop();
    wait(0.5);
}

void TurnRight(){
    wait(0.5);
    car.TurnR(100);
    wait(1.1);
    car.stop();
    wait(0.5);
}

void TakePicture(){
    encoder0.reset();

    car.goStraight(100);
    while(encoder0.get_cm()<20) wait_ms(50);
    car.stop();
    Send.start(send_thread);
    wait(0.5);
    GoStraight(15);
    TurnLeft();
}

void ReverseParking(){
    TurnRight();

    encoder0.reset();
    car.goStraight(-100);
    while(encoder0.get_cm()<10) wait_ms(50);
    car.stop();
    wait(1);

    GoStraight(10);
    wait(0.5);

    TurnRight();
}

void Scan(){
    int i;
    TurnLeft();

    car.goStraight(-100);
    wait(0.5);
    car.stop();

    car.goStraight(50);
    float Distance[10]={0};

    for (i=0; i<10; i++){
        Distance[i]=(float)ping1;
        wait(0.5);
    }

    for (i=0; i<9; i++){
        if (Distance[i]-Distance[i+1]>5){
            break;
        }
    }

    i=i+1;
    if (Distance[i]-Distance[i+1]>0.5){
        if (Distance[i+2]-Distance[i+3]>0.5){
            // Wedge
        }
        else if (Distance[i+2]-Distance[i+3]<0.5){
            // Triangle
        }
    }

    else if (Distance[i]-Distance[i+1]<0.5){
        if (Distance[i+2]-Distance[i+3]<0.5){
            // Square
        }
    }

    else {
        // Complement of Triangle
    }

    TurnRight();
}

void recieve_thread(){
   while(1) {
      if(uart.readable()){
            char recv = uart.getc();
            pc.putc(recv);
            pc.printf("\r\n");
      }
   }
}

// **************** Test for scanning
void st(){
    car.goStraight(50);
    for (int i=0; i<10; i++){
        pc.printf("%f\r\n", (float)ping1);
        wait(0.5);
    }
    car.stop(); 
}

// **************** Test for taking picture
void tp(){
    //wait(3);
    char s[2];
    sprintf(s,"image_classification");
    uart.puts(s);
    pc.printf("send\r\n");
    if(uart.readable()){
            char recv = uart.getc();
            pc.putc(recv);
            pc.printf("\r\n");
      }
    //wait(5);
}

void send_thread(){
    char s[21];
    sprintf(s,"image_classification");
    uart.puts(s);
    pc.printf("send\r\n");
    wait(0.5);    
}

void xbee_rx_interrupt(void){
    //pc.printf("Hello1\n");
    xbee.attach(NULL, Serial::RxIrq); // detach interrupt
    //pc.printf("Hello2\n");
    xbee_queue.call(&xbee_rx);
}


void xbee_rx(void){
    //pc.printf("Hello3\n");

    char buf[100] = {0};
    char outbuf[100] = {0};
    while(xbee.readable()){
        for (int i=0; ; i++) {
            char recv = xbee.getc();
            if (recv == '\r') {
                break;
            }
            buf[i] = pc.putc(recv);//recv; 
        }
        pc.printf("%s\r\n", outbuf);
        RPC::call(buf, outbuf);
        wait(0.1);
        //xbee.printf("Hello, world!\r\n");
        //pc.printf("Hello, world!\n");
    }

    //xbee.printf("Hello, world!\r\n");
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}
