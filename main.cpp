#include "mbed.h"
#include "bbcar.h"


Ticker servo_ticker;
Ticker encoder_ticker;

PwmOut pin8(D8), pin9(D9);
DigitalIn pin3(D3);
Serial pc(USBTX, USBRX);
BBCar car(pin8, pin9, servo_ticker);
DigitalInOut pin10(D10);
parallax_ping  ping1(pin10);
parallax_encoder encoder0(pin3, encoder_ticker);


void GoStraightPing(int DistanceCm);
void GoStraight(int DistanceCm);
void TurnLeft();
void TurnRight();
void TakePicture();
void ReverseParking();
void st();


int main() {

    /*encoder0.reset();

    car.goStraight(100);

    while(encoder0.get_cm()<10) wait_ms(50);

    car.stop();

    wait(1);

    car.TurnL(100);

    wait(1);
   
    car.stop();

    wait(1.1);

    car.goStraight(100);

    encoder0.reset();

    while(encoder0.get_cm()<10) wait_ms(50);

    car.stop();*/

// ************ Keep Straight ********************
    /*encoder0.reset();

    while(encoder0.get_cm()<50){
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
    car.stop();*/
// ***************************************




// ************ Turn Left 90 degrees  ********************
    /*car.goStraight(100);
    wait(1);
    car.stop();
    wait(0.5);
    car.TurnL(100);
    wait(1.1);
    car.stop();*/
// *******************************************************

// ************ Turn Right 90 degrees  ********************
    /*car.goStraight(100);
    wait(1);
    car.stop();
    wait(0.5);
    car.TurnR(100);
    wait(1.1);
    car.stop();*/
// *******************************************************

// ************ Turn Right -90 degrees  ********************
    /*car.goStraight(100);
    wait(1);
    car.stop();
    wait(0.5);
    car.TurnR(100);
    wait(1.1);
    car.stop();
    wait(1);

    car.goStraight(-100);
    wait(0.5);
    car.stop();
    wait(0.5);
    car.TurnL(100);
    wait(1.1);
    car.stop();*/
// *******************************************************

/*encoder0.reset();


    car.goStraight(100);

    while(encoder0.get_cm()<30) wait_ms(50);

    car.stop();*/


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
    TurnRight();

    car.goStraight(-100);
    wait(0.5);
    car.stop();
    //send command
    wait(0.5);
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

// **************** Test for scanning
void st(){
    car.goStraight(50);
    for (int i=0; i<10; i++){
        pc.printf("%f\r\n", (float)ping1);
        wait(0.5);
    }
    car.stop(); 
}