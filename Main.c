#include <LiquidCrystal.h>
    //Output Pins
    const int WaterValvePin = 7;
    const int RaisePhValvePin = 8;
    const int DrainWaterValvePin = 9;

    //LCD Display Pins
    //RS = Reset Pin
    //EN = Enable Pin
    //D = Digital
    const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

    //INPUT Pins
    const int MiddleButtonPin = 8;

    //Sensor Pins
    const int PpmSensorPin = A0;
    const int PhSensorPin = A1;
    const int WaterLevelPin = A2;


    //Settings
    const int PhLowerThreshold = 200;
    const int PhUpperThreshold = 300;

    const int PpmLowerThreshold = 100;
    const int PpmUpperThreshold = 200;

    const int WaterLevelLowerThreshold = 150;


    int current_ph;
    int ppmValue;
    int offset;
    int secondOffset;

    int lastWaterLevelCheck = millis() / 1000;
    int lastPhCheck = millis() / 1000;
    int lastPpmCheck = millis() / 1000;

    LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Calculating the middle of the screen and displaying text on LCD screen
void lcdDisplay(String firstLineText, String secondLineText){
    offset = round(8 - ( firstLineText.length() / 2 ));
    if (secondLineText != "NONE"){
        secondOffset = round(8 - ( secondLineText.length() / 2 ));
        lcd.setCursor(secondOffset,2);
        lcd.print(secondLineText);
    }

    lcd.setCursor(offset, 1);
    lcd.print(firstLineText);


}

int PpmCheck(){

    int CurrentPpmValue = analogRead(PpmSensorPin);

    return CurrentPpmValue;
}

bool WaterLevelCheck(){
  int WaterLevelValue = analogRead(WaterLevelPin);

  return WaterLevelValue;

}

int PhCheck(){
  int CurrentPhValue = analogRead(PhSensorPin);

  return CurrentPhValue;
}

//Opens and Closes Valve; ValveCycle(Pin_of_valve, Time to open valve)
void ValveCycle(int pin, int seconds){
    digitalWrite(pin, HIGH);
    delay(seconds * 1000);
    digitalWrite(pin, LOW);

}

void setup() {

    //Setup Pins
    pinMode(PpmLedPin,OUTPUT);
    pinMode(DrainWaterValvePin,OUTPUT);
    pinMode(RaisePhValvePin,OUTPUT);
    pinMode(WaterValvePin,OUTPUT);

    pinMode(MiddleButtonPin,INPUT);

    lcd.begin(16,2);
}
void loop() {
    int current_time = millis() / 1000; //Records in Seconds


            if (ppmValue != 0){
            lcdDisplay("PPM:" +  String(ppmValue), "Last Check: " + String(( current_time - lastPpmCheck)/60) + " min ago");
            }
            else{
                lcdDisplay("PPM Not Measured","Yet!");
            }



    //Button Forces Manual Check, Or automatically check, whenever an hour has passed from previous check
    if(digitalRead(MiddleButtonPin) == true || (current_time - lastPpmCheck) > 3600){
        ppmValue = PpmCheck();
        if ( ppmValue < PpmLowerThreshold || ppmValue > PpmUpperThreshold) {
            digitalWrite(PpmLedPin,HIGH);
        }

        else{
            digitalWrite(PpmLedPin, LOW);
        }

        lastPpmCheck = millis();
    }

    //Raises Water Level if too Low
    if ( (current_time - lastWaterLevelCheck) > 3600){
        if (WaterLevelCheck() < WaterLevelLowerThreshold){

        ValveCycle(WaterValvePin, 5); //Cycles Valve for 5 Seconds at a time
        }
        else{
            lastWaterLevelCheck = millis() / 1000;
        }
    }


    //Drains Water if Ph is too High, adds buffer if too low
    if ((current_time - lastPhCheck) > 3600){
        current_ph = PhCheck();
        if (current_ph < PhLowerThreshold){
            ValveCycle(RaisePhValvePin, 1);
        }

        else if (current_ph > PhUpperThreshold){
            ValveCycle(DrainWaterValvePin, 5);
        }
        else{
          lastPhCheck = millis() / 1000;
        }

    }
}
