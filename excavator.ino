#include <SoftwareSerial.h>
#include <PS4Controller.h>
#include <string>

using namespace std;

// Klassen der Hardware-Komponenten

class Engine {
private:
  int IN1;
  int IN2;

public:
  Engine(int in1, int in2) { // Konstruktor
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

class PS4Controller {
private:
  string macAddress;
public:
  PS4Controller() { // Konstruktor
    macAddress = "00:00:00:00:00:00"; // Standard-MAC-Adresse
  }

  bool begin(string mac) {
    if (isValidMac(mac)) {
      macAddress = mac;
      // Hier sollte die Logik zum Verbinden des Controllers implementiert werden
      return true; // Simuliert eine erfolgreiche Verbindung
    } else {
      return false; // Mac-Adresse ist ungültig
    }
  }

  // Validierung der MAC-Adresse
  bool isValidMac(string mac) {
    if (mac.length() != 17) return false;
    for (int i = 0; i < mac.length(); i++) {
      if (i % 3 == 2) {
        if (mac[i] != ':') return false;
      } else {
        if (!isxdigit(mac[i])) return false;
      }
    }
    return true;
  }
}

// Erstellen der ObjektInstanzen 

PS4Controller PS4;

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

  Serial.begin(115200); // Serielle Kommunikation starten
  
  // PS4 Controller mit Mac initialisieren
  bool connected = false;
  for (int i = 1; i <= 3; i++) {
    Light.blink(500); // LED blinkt, um den Verbindungsversuch anzuzeigen
    if (PS4.begin("00:00:00:00:00:00")) { // Hier sollte die tatsächliche MAC-Adresse des PS4 Controllers stehen
      Serial.println("PS4 Controller verbunden");
      connected = true;
      Light.turnOn(); // LED bleibt an, wenn die Verbindung erfolgreich ist
      break; // Verbindung erfolgreich, Schleife verlassen
    } else {
      Serial.println("Fehler beim Verbinden des PS4 Controllers");
      Serial.println("Versuch " + String(i) + " von 3...");
      delay(3000); // Wartezeit vor erneutem Versuch
      Serial.println("Versuche erneut zu verbinden...");
    }
  }

  if (!connected) {
    Serial.println("Verbindung zum PS4 Controller fehlgeschlagen. Programm wird angehalten.");
    while (true); // Endlosschleife, um das Programm anzuhalten
  }

  delay(2000);
  Light.turnOff(); // LED ausschalten
}

void loop() {
  if (PS4.isConnected()) {
    // LED umschalten
    if (PS4.Options()) Light.toggle(); 
    
    // Tummersteuerung
    if (PS4.L1()) towerTurnLeft();
    else if (PS4.R1()) towerTurnRight();
    else towerStop();

    // Bewegungssteuerung
    if (PS4.Up()) drive(true); // Vorwärts fahren
    else if (PS4.Down()) drive(false); // Rückwärts fahren
    else if (PS4.Left()) turnLeft();
    else if (PS4.Right()) turnRight();
    else stopMovement();

    // Oberarmsteuerung
    if (PS4.Triangle()) upperArmUp();
    else if (PS4.Circle()) upperArmDown();
    else upperArmStop();

    // Unterarm
    if (PS4.Cross()) lowerArmUp();
    else if (PS4.Square()) lowerArmDown();
    else lowerArmStop();

    // Schaufelsteuerung
    if (PS4.L2()) shovelUp();
    else if (PS4.R2()) shovelDown();
    else shovelStop();
    
    // Alle Motoren stoppen, wenn Share gedrückt wird
    if (PS4.Share()) StopAllMotors(); 
  }
}

// Funktionen des Buggers

void drive(bool forward) {
  M1.spin(forward); // Linker Motor fährt vorwärts oder rückwärts
  M2.spin(forward); // Rechter Motor fährt vorwärts oder rückwärts
}

void turn(bool left) {
  if (left) { // Wenn nach links gedreht wird
    M1.stop(); // Linker Motor stoppt
    M2.spin(true); // Rechter Motor fährt vorwärts
  } else { // Wenn nach rechts gedreht wird
    M1.spin(true); // Linker Motor fährt vorwärts
    M2.stop(); // Rechter Motor stoppt
  }
}

void stopMovement() {
  M1.stop(); // Linker Motor stoppt
  M2.stop(); // Rechter Motor stoppt
}

void turnTower(bool left) {
  M3.spin(left); // Turmmotor dreht in die angegebene Richtung
}

void stopTower() {
  M3.stop(); // Turmmotor stoppt
}

void moveUpperArm(bool up) {
  M5.spin(up); // Oberarmmotor fährt in die angegebene Richtung
}

void stopUpperArm() {
  M5.stop(); // Oberarmmotor stoppt
}

void moveLowerArm(bool up) {
  M4.spin(up); // Unterarmmotor fährt in die angegebene Richtung
}

void stopLowerArm() {
  M4.stop(); // Unterarmmotor stoppt
}

void moveShovel(bool up) {
  M6.spin(up); // Schaufelmotor fährt in die angegebene Richtung
}


void stopShovel() {
  M6.stop(); // Schaufelmotor stoppt
}

void StopAllMotors() {
  stopMovement(); // Alle Bewegungsmotoren stoppen
  stopTower(); // Turmmotor stoppen
  stopUpperArm(); // Oberarmmotor stoppen
  stopLowerArm(); // Unterarmmotor stoppen
  stopShovel(); // Schaufelmotor stoppen
}