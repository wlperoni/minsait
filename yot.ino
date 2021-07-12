#include "M5CoreInk.h"
#include<Wire.h>
#include "MAX30100_PulseOximeter.h"
#define INTERVAL 60000
long lastSendTime = 0;
const int MPU_addr=0x68; // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int contFrente=0;
int contTras=0;
int anterior=0;
int anterior2=0;
Ink_Sprite InkPageSprite(&M5.M5Ink);
char acxbuff[64];
PulseOximeter pox;

void send_minsait_platorfm(){
    // send data mqtt abstration
    pox.getHeartRate();
    pox.getSpO2();
    AcX;AcZ;Tmp;GyX;GyY;GyZ;
  }

void setup() {

    M5.begin();
    if( !M5.M5Ink.isInit())
    {
        Serial.printf("Ink Init faild");
        while (1) delay(100);   
    }
    M5.M5Ink.clear();
    delay(1000);
    //creat ink refresh Sprite
    if( InkPageSprite.creatSprite(0,0,200,200,true) != 0 )
    {
        Serial.printf("Ink Sprite creat faild");
    }
    InkPageSprite.drawString(90,10,"YoT");
    InkPageSprite.pushSprite();
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

Wire.begin(21,22);
Wire.beginTransmission(MPU_addr);
Wire.write(0x6B); // PWR_MGMT_1 register
Wire.write(0); // set to zero (wakes up the MPU-6050)
Wire.endTransmission(true);
pox.setOnBeatDetectedCallback(getAcX);
}
void getAcX(){
pox.update();    
Wire.beginTransmission(MPU_addr);
Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
Wire.endTransmission(true);
AcX=AcX/10;

sprintf(acxbuff,"%d",AcX);
InkPageSprite.drawString(50,60,"              ");
InkPageSprite.pushSprite();
delay(300);
InkPageSprite.drawString(50,60,"* Analysing *");
InkPageSprite.pushSprite();


}

void loop() {
pox.update();
getAcX();
anterior=AcX;
Serial.print("before-> ");
Serial.println(anterior);
Serial.println("Analysing");
delay(1000);
getAcX();
anterior2=AcX;
Serial.print("After-> ");
Serial.println(anterior);

Serial.print("Change-> ");
Serial.println(abs(anterior-anterior2));

if(abs(anterior-anterior2)> 60 ){
  Serial.println("In Workout");
  InkPageSprite.drawString(55,80,"In Workout  ");
  InkPageSprite.pushSprite();
}else{
  Serial.println("No Workout");
  InkPageSprite.drawString(55,80,"No Workout  ");
  InkPageSprite.pushSprite();
  }

  Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
if(millis() - lastSendTime > INTERVAL){
   send_minsait_platorfm();
  
}
} 
