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

  void spin(bool forward = true) {
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

private:
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
  
}

void loop() {

  // Beispiel
  String command = BT_Module.receiveData();
  if (command == "start") {
    M1.spin(true);
  } else if (command == "stop") {
    M1.stop();
  }
}

// Funktionen des Buggers