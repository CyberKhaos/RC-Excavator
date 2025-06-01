#include <SoftwareSerial.h>

// Klassen der Hardware-Komponenten

class Engine {
private:
  int IN1;
  int IN2;

public:
  Engine(int in1, int in2) {
    IN1 = in1;
    IN2 = in2;
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
  }

  void spin(bool forward) {
    if (forward) {
      startSpin(IN1, IN2);
    } else {
      startSpin(IN2, IN1);
    }
  }

  void stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  void startSpin(int a, int b){
    digitalWrite(a, HIGH);
    digitalWrite(b, LOW);
  }
};

class LED {
private:
  int pin;
  bool state;

public:
  LED(int pinNumber) { // Konstruktor
    pin = pinNumber;
    state = LOW; // Initialer Zustand
    pinMode(pin, OUTPUT);
  }

  void turnOn() {
    digitalWrite(pin, HIGH);
    state = HIGH;
  }

  void turnOff() {
    digitalWrite(pin, LOW);
    state = LOW;
  }

  void toggle() {
    if (state == HIGH) {
      turnOff();
    } else {
      turnOn();
    }
  }
};

class BT {
private:
  SoftwareSerial* btSerial; // Zeiger auf SoftwareSerial

public:
  BT(int rx, int tx) {
    btSerial = new SoftwareSerial(rx, tx);
    btSerial->begin(9600); // Bluetooth-Modul starten
  }

  String receiveData() {
    if (btSerial->available()) {
      return btSerial->readString();
    }
    return "";
  }
};

// Erstellen der ObjektInstanzen 

BT BT_Module(10, 11);

Engine M1(A0, A1); // Kettenmotor links
Engine M2(A2, A3); // Kettenmotor rechts
Engine M3(A4, A5); // Turmmotor
Engine M4(A6, A7); // Unterarmmotor
Engine M5(2, 3); // Oberarmmotor
Engine M6(4, 5); // Schaufelmotor

LED Light(6);

// Aufbau und Hauptsteuerung
void setup() {
  Light.turnOn(); // LED einschalten
  delay(2000); // Kurze Verzögerung, um sicherzustellen, dass die LED eingeschaltet ist
  Light.turnOff(); // LED ausschalten
}

void loop() {
  
}

// Funktionen des Buggers

void driveForward() {
  M1.spin(true); // Linker Motor fährt vorwärts
  M2.spin(true); // Rechter Motor fährt vorwärts
}

void driveBackward() {
  M1.spin(false); // Linker Motor fährt rückwärts
  M2.spin(false); // Rechter Motor fährt rückwärts
}

void turnLeft() {
  M1.stop(); // Linker Motor stoppt
  M2.spin(true); // Rechter Motor fährt vorwärts
}

void turnRight() {
  M1.spin(true); // Linker Motor fährt vorwärts
  M2.stop(); // Rechter Motor stoppt
}

void stopMovement() {
  M1.stop(); // Linker Motor stoppt
  M2.stop(); // Rechter Motor stoppt
}

void towerTurnLeft() {
  M3.spin(true); // Turmmotor dreht nach links
}

void towerTurnRight() {
  M3.spin(false); // Turmmotor dreht nach rechts
}

void towerStop() {
  M3.stop(); // Turmmotor stoppt
}

void upperArmUp() {
  M5.spin(true); // Oberarmmotor fährt nach oben
}

void upperArmDown() {
  M5.spin(false); // Oberarmmotor fährt nach unten
}

void upperArmStop() {
  M5.stop(); // Oberarmmotor stoppt
}

void lowerArmUp() {
  M4.spin(true); // Unterarmmotor fährt nach oben
}

void lowerArmDown() {
  M4.spin(false); // Unterarmmotor fährt nach unten
}

void lowerArmStop() {
  M4.stop(); // Unterarmmotor stoppt
}

void shovelUp() {
  M6.spin(true); // Schaufelmotor fährt nach oben
}

void shovelDown() {
  M6.spin(false); // Schaufelmotor fährt nach unten
}

void shovelStop() {
  M6.stop(); // Schaufelmotor stoppt
}

void toggleLight() {
  Light.toggle(); // LED umschalten
}