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
void TurnLeft1();
void TurnRight();
void TakePicture();
void ReverseParking();
void recieve_thread();
void send_thread();
void Scan();

int k=1; //1:GoStraightPing, 2:GoStraight, 3:TurnLeft, 4:TurnRight, 5:TakePicture, 6:ReverseParking, 7:Scan

int main() {

    uart.baud(9600);
    xbee.baud(9600);
    
    xbee_thread.start(callback(&xbee_queue, &EventQueue::dispatch_forever));
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
    
    GoStraightPing(125);
    TurnLeft();
    GoStraightPing(30);
    TurnRight();
    TakePicture();
    GoStraightPing(40);
    ReverseParking();
    GoStraight(55);
    TurnRight();
    GoStraightPing(110);
    TurnRight();
    GoStraight(50);
    Scan();
    GoStraight(20);
    TurnRight();
    GoStraightPing(140);
}

void GoStraightPing(int DistanceCm){
    k = 1;
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
    k = 2;
    encoder0.reset();

    car.goStraight(100);
    while(encoder0.get_cm()<DistanceCm) wait_ms(50);
    car.stop();
}

void TurnLeft(){
    k = 3;
    wait(0.5);
    car.TurnL(100);
    wait(1.2);
    car.stop();
    wait(0.5);
}

void TurnLeft1(){
    wait(0.5);
    car.TurnL(100);
    wait(1.4);
    car.stop();
    wait(0.5);
}

void TurnRight(){
    k = 4;
    wait(0.5);
    car.TurnR(100);
    wait(1.2);
    car.stop();
    wait(0.5);
}

void TakePicture(){
    k = 5;
    wait(0.5);
    encoder0.reset();

    car.goStraight(-100);
    while(encoder0.get_cm()<30) wait_ms(50);
    car.stop();
    Send.start(send_thread);
    wait(5);
    GoStraight(15);
    TurnLeft();
}

void ReverseParking(){
    k = 6;
    TurnRight();

    k = 6;
    encoder0.reset();
    car.goStraight(-100);
    while(encoder0.get_cm()<40) wait_ms(50);
    car.stop();
    wait(0.5);

    encoder0.reset();
    car.goStraight(100);
    while(encoder0.get_cm()<30) wait_ms(50);
    car.stop();
    wait(0.3);

    TurnRight();
}

void Scan(){
    k = 7;
    int i;
    TurnLeft();

    k = 7;
    car.goStraight(-100);
    wait(4);
    car.stop();
    wait(0.5);

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
    if (Distance[i]-Distance[i+1]>1){
        if (Distance[i+2]-Distance[i+3]>1){
            pc.printf("Wedge\r\n");
        }
        else if (Distance[i+3]-Distance[i+2]>1){
            pc.printf("Triangle\r\n");
        }
    }

    else if ((Distance[i]-Distance[i+1]<1)&&(Distance[i]-Distance[i+1]>-1)){
        if ((Distance[i+2]-Distance[i+3]<1)&&(Distance[i+2]-Distance[i+3]>-1)){
            pc.printf("Square\r\n");
        }
    }

    else if ((Distance[i+1]-Distance[i]>1)&&(Distance[i+2]-Distance[i+3]>1)){
        pc.printf("Complement of Triangle\r\n");
    }

    else{
        pc.printf("Can't be identified.\r\n");
    }

    wait(0.5);
    car.stop();
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

void send_thread(){
    char s[21];
    sprintf(s,"image_classification");
    uart.puts(s);
    //pc.printf("send\r\n");
    wait(0.5);    
}

void xbee_rx_interrupt(void){
    xbee.attach(NULL, Serial::RxIrq); // detach interrupt
    xbee_queue.call(&xbee_rx);
}

void xbee_rx(void){

    char buf[100] = {0};
    char outbuf[100] = {0};
    while(xbee.readable()){
        for (int i=0; ; i++) {
            char recv = xbee.getc();
            if (recv == '\r') {
                break;
            }
            buf[i] = recv;
        }
        pc.printf("%s\r\n", outbuf);
        RPC::call(buf, outbuf);
        wait(0.1);
        
        if (k == 1){
            pc.printf("GoStraightPing");
        }
        if (k == 2){
            pc.printf("GoStraight");
        }
        if (k == 3){
            pc.printf("TurnLeft");
        }
        if (k == 4){
            pc.printf("TurnRight");
        }
        if (k == 5){
            pc.printf("TakePicture");
        }
        if (k == 6){
            pc.printf("ReverseParking");
        }
        if (k == 7){
            pc.printf("Scan");
        }
        
    }
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}
