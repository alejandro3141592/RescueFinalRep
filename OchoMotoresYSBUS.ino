//Codigo elaborado por Mike, Carlos, David, Alonso y Fausto


//Patas delanteras, canales 11 y 12. Patas traseras canales 6 y 7
#include "BTS7960.h"


#include "sbus.h"

bfs::SbusRx sbus_rx(&Serial2, 2, 15, true, false);
bfs::SbusData data;


const uint8_t motor1_pins[3] = { 2, 12, 13};  //Enable, Left_PWM, Right_PWM //RPWM3 EN PLACA
const uint8_t motor2_pins[3] = { 2, 18, 5 };  //Enable, Left_PWM, Right_PWM //RPWM6 EN PLACA
const uint8_t motor3_pins[3] = { 2, 17, 16 };  //Enable, Left_PWM, Right_PWM //RPWM7 EN PLACA
const uint8_t motor4_pins[3] = { 2, 4, 0 };  //Enable, Left_PWM, Right_PWM //RPWM8 EN PLACA

const uint8_t motor5_pins[3] = { 2, 32, 33};  //Enable, Left_PWM, Right_PWM //RPWM1 EN PLACA
const uint8_t motor6_pins[3] = { 2, 26, 27 };  //Enable, Left_PWM, Right_PWM //RPWM2 EN PLACA
const uint8_t motor7_pins[3] = { 2, 23, 22 };  //Enable, Left_PWM, Right_PWM //RPWM4 EN PLACA
const uint8_t motor8_pins[3] = { 2, 21, 19 };  //Enable, Left_PWM, Right_PWM //RPWM5 EN PLACA

BTS7960 motorController1(motor1_pins[0], motor1_pins[1], motor1_pins[2]);
BTS7960 motorController2(motor2_pins[0], motor2_pins[1], motor2_pins[2]);
BTS7960 motorController3(motor3_pins[0], motor3_pins[1], motor3_pins[2]);
BTS7960 motorController4(motor4_pins[0], motor4_pins[1], motor4_pins[2]);


BTS7960 motorControllerPatita1(motor5_pins[0], motor5_pins[1], motor5_pins[2]);
BTS7960 motorControllerPatita2(motor6_pins[0], motor6_pins[1], motor6_pins[2]);
BTS7960 motorControllerPatita3(motor7_pins[0], motor7_pins[1], motor7_pins[2]);
BTS7960 motorControllerPatita4(motor8_pins[0], motor8_pins[1], motor8_pins[2]);

int velocidad = 255;
int velocidadPatita = 180;
void setup() {
  Serial.begin(115200);
  motorController1.Enable();
  motorController2.Enable();
  motorController3.Enable();
  motorController4.Enable();
  sbus_rx.Begin();


}

///************************CAMBIAR canales por variables************************

void loop() {

  if (sbus_rx.Read()) {
    /* Grab the received data */
    data = sbus_rx.data();

    //Traccion
    if (data.ch[1]<1100 && data.ch[1]>900){
      if (data.ch[0]<1100 && data.ch[0]>900){
        alto();
        Serial.println("alto");
      }else if(data.ch[0]>1100){
        derecha();
        Serial.println("derecha");
      }else{
        izquierda();
        Serial.println("izquierda");
      }
    }else if(data.ch[1]>1100){
      atras();
      Serial.println("atras");
    }else{
      adelante();
      Serial.println("adelante");
    }

    //Pata adelante derecha
    if(data.ch[10]<1100 && data.ch[10]>900){
      detenerPatita(motorControllerPatita1);
    } else if (data.ch[10]>1100){
      giroSentidoHorario(motorControllerPatita1);
    } else{
      giroSentidoAntiHorario(motorControllerPatita1);
    }

    //Pata adelante izquierda
    if(data.ch[11]<1100 && data.ch[11]>900){
      detenerPatita(motorControllerPatita2);
    } else if (data.ch[11]>1100){
      giroSentidoHorario(motorControllerPatita2);
    } else{
      giroSentidoAntiHorario(motorControllerPatita2);
    }

    //Pata atras derecha
    if(data.ch[5]<1100 && data.ch[5]>900){
      detenerPatita(motorControllerPatita3);
    } else if (data.ch[5]>1100){
      giroSentidoHorario(motorControllerPatita3);
    } else{
      giroSentidoAntiHorario(motorControllerPatita3);
    }

    //Pata atras izuierda
    if(data.ch[6]<1100 && data.ch[6]>900){
      detenerPatita(motorControllerPatita4);
    } else if (data.ch[6]>1100){
      giroSentidoHorario(motorControllerPatita4);
    } else{
      giroSentidoAntiHorario(motorControllerPatita4);
    }
  }
}

void adelante() {

  motorController1.TurnLeft(velocidad);
  motorController2.TurnLeft(velocidad);
  motorController3.TurnLeft(velocidad);
  motorController4.TurnLeft(velocidad);
}

void atras() {

  motorController1.TurnRight(velocidad);
  motorController2.TurnRight(velocidad);
  motorController3.TurnRight(velocidad);
  motorController4.TurnRight(velocidad);
}

void derecha() {

  motorController1.TurnLeft(velocidad);
  motorController2.TurnRight(velocidad);
  motorController3.TurnLeft(velocidad);
  motorController4.TurnRight(velocidad);
}

void izquierda() {

  motorController1.TurnRight(velocidad);
  motorController2.TurnLeft(velocidad);
  motorController3.TurnRight(velocidad);
  motorController4.TurnLeft(velocidad);
}
void alto() {

  motorController1.Stop();
  motorController2.Stop();
  motorController3.Stop();
  motorController4.Stop();
}

void giroSentidoHorario(BTS7960 controller){
  controller.TurnLeft(velocidadPatita);
}

void giroSentidoAntiHorario(BTS7960 controller){
  controller.TurnRight(velocidadPatita);
}

void detenerPatita(BTS7960 controller){
  controller.Stop();
}


