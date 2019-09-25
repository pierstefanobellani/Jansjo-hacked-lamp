#include <CapacitiveSensor.h>
#include<Wire.h>


CapacitiveSensor   cs_8_7 = CapacitiveSensor(8, 7);       // 10 megohm resistor between pins 8 & 7, pin 7 is sensor pin, add wire, foil


const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ/*,  Tmp, GyX, GyY, GyZ */;

int inclinazione;

int LDR = A1;
int LDRVal;


int red = 9 ;
int green = 10;
int blue = 11;

int lucered;
int lucegreen;
int luceblue;


int stato = 0;

unsigned long previousMillis = 0;
//const long studio = 3000;
//const long pausa = 1000;

int focus = 0;

int sessione = 1;
long ciclo;
long relax;



void setup() {

  cs_8_7.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example


  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);


  pinMode (LDR, INPUT);


  pinMode (red, OUTPUT);
  pinMode (green, OUTPUT);
  pinMode (blue, OUTPUT);


  Serial.begin(9600);

}




void loop() {

  if (stato == 0) {//////////////////////////////////////////////////////


    digitalWrite (red, HIGH);
    digitalWrite (green, HIGH);
    digitalWrite (blue, HIGH);

    delay (2000);

    stato = 1;
  }




  if (stato == 1) {////////////////////////////////////////////////////

    long start = millis();
    long total1 =  cs_8_7.capacitiveSensor(30);

    int m = map (total1, 0, 2700, 0, 500);
    //Serial.print ("cap");
    //Serial.println (m);


    if (m > 12) {
      stato = 2;
    }

  }




  if (stato == 2) {//////////////////////////////////////////////////

    LDRVal = analogRead (LDR);
    Serial.println (LDRVal);

    if (LDRVal >= 805) {
      stato = 0;
    }




    if (Serial.available() > 0) { //analogRead(Serial.read());
      sessione = Serial.read();
    }

    ciclo = sessione * 60000; //4 min=240000  //20 min=1200000 ms
    relax = ciclo / 4;

    //  Serial.println (sessione);
    //  Serial.println (ciclo);
    //  Serial.println (relax);
    //  delay (100);




    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
    
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    delay(30);

    inclinazione = map (AcZ, -17000, 17000, 255, 0); //absolutely useless, I know, redundant
    //    Serial.println (inclinazione);


    lucered = map (inclinazione, 255, 0, 0, 20);
    lucegreen = map (inclinazione, 255, 0, 0, 170);
    luceblue = map (inclinazione, 255, 0, 0, 221);

    analogWrite (red, lucered);
    analogWrite (green, lucegreen);
    analogWrite (blue, luceblue);


    int shaker = 5 * lucered;

    //Serial.println (shaker);

    if (shaker > 115) {
      delay(100);

      for (int volte = 0; volte < sessione; volte++) {

        for (lucegreen = 255; lucegreen > 0; lucegreen--) {
          analogWrite (red, 255);
          analogWrite (green, lucegreen);
          analogWrite (blue, 255);
          delay (2);
        }

        for (lucegreen = 0; lucegreen < 255; lucegreen++) {
          analogWrite (red, 255);
          analogWrite (green, lucegreen);
          analogWrite (blue, 255);
          delay (2);
        }
      }

      focus = 0;
      stato = 3;

    }
  }




  if (stato == 3) {//////////////////////////////////////////////////////
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
    
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    delay(30);


    inclinazione = map (AcZ, -17000, 17000, 255, 0); //absolutely useless, I know, redundant
    //    Serial.println (inclinazione);


    lucered = map (inclinazione, 255, 0, 0, 20);
    int shaker = 5 * lucered;

    if (shaker > 115) {
      delay(100);

      for (int volte = 0; volte < sessione; volte++) {

        for (lucered = 255; lucered > 0; lucered--) {
          analogWrite (red, lucered);
          analogWrite (green, 255);
          analogWrite (blue, 255);
          delay (2);
        }

        for (lucered = 0; lucered < 255; lucered++) {
          analogWrite (red, lucered);
          analogWrite (green, 255);
          analogWrite (blue, 255);
          delay (2);

        }
      }
      stato = 2;
    }


    analogWrite (red, lucered);
    analogWrite (green, lucegreen);
    analogWrite (blue, luceblue);

    unsigned long currentMillis = millis();
    
    if (focus == 0) {

      lucered = 0;
      lucegreen = 0;
      luceblue = 0;

      if (currentMillis - previousMillis >= ciclo) {
        previousMillis = currentMillis;
        focus = 1;
      }
    }

    if (focus == 1) {

      lucered = 20;
      lucegreen = 170;
      luceblue = 221;

      if (currentMillis - previousMillis >= relax) {
        previousMillis = currentMillis;
        focus = 0;
      }
    }
  }
}
