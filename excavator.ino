#include <PS4Controller.h>
#include <Arduino.h>

// Klassen der Hardware-Komponenten

// Klasse für die Motorsteuerung
class Engine {
private:
  int pin1;
  int pin1;

public:
  Engine(int in1, int in2) { // Konstruktor mint zwei Pins
    pin1 = in1;
    pin2 = in2;
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
  }

  // Motorsteuerung 
  // Vorwärts(true) oder rückwärts(false) drehen
  void spin(bool forward) {
    if (forward) {
      startSpin(IN1, IN2);
    } else {
      startSpin(IN2, IN1);
    }
  }

  // Motor stoppen
  void stop() {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }

  // Startet den Motor in die angegebene Richtung
  void startSpin(int high, int low){
    digitalWrite(high, HIGH);
    digitalWrite(low, LOW);
  }
};

// Klasse für die LED-Steuerung
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

  // LED einschalten
  void turnOn() {
    digitalWrite(pin, HIGH);
    state = HIGH;
  }

  // LED ausschalten
  void turnOff() {
    digitalWrite(pin, LOW);
    state = LOW;
  }

  // LED blinken für eine bestimmte Dauer
  void blink(int duration) {
    turnOn();
    delay(duration);
    turnOff();
    delay(duration);
  }

  // LED umschalten
  void toggle() {
    if (state == HIGH) {
      turnOff();
    } else {
      turnOn();
    }
  }
};

// Klasse für den PS4 Controller
class GamePad : public PS4Controller {
private:
  String macAddress;
public:
  GamePad(String mac) { // Konstruktor
    if (isValidMac(mac)) {
      macAddress = mac; // Setzt die MAC-Adresse, wenn sie gültig ist
    } else {
      macAddress = "00:00:00:00:00:00"; // Standard-MAC-Adresse, falls ungültig
    }
  }

  bool begin(String mac) {
    if (isValidMac(mac)) {
      macAddress = mac;
      // Hier sollte die Logik zum Verbinden des Controllers implementiert werden
      return true; // Simuliert eine erfolgreiche Verbindung
    } else {
      return false; // Mac-Adresse ist ungültig
    }
  }

  // Validierung der MAC-Adresse
  bool isValidMac(String mac) {
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
};

// Erstellen der ObjektInstanzen 

GamePad Controller("00:00:00:00:00:00"); // PS4 Controller mit einer seiner MAC-Adresse

Engine M1(23, 22); // Kettenmotor links
Engine M2(21, 19); // Kettenmotor rechts
Engine M3(18, 5); // Turmmotor
Engine M4(17, 16); // Unterarmmotor
Engine M5(4, 0); // Oberarmmotor
Engine M6(2, 15); // Schaufelmotor

LED Light(13);

// Aufbau und Hauptsteuerung

void setup() {
  Light.turnOn(); // LED einschalten
  
  // PS4 Controller mit Mac initialisieren
  bool connected = false;
  for (int i = 1; i <= 3; i++) {
    Light.blink(500); // LED blinkt, um den Verbindungsversuch anzuzeigen
    if (PS4.begin("00:00:00:00:00:00")) { // Hier sollte die tatsächliche MAC-Adresse des PS4 Controllers stehen
      for (int j = 0; j < 3; j++) Light.blink(100); // LED blinkt 3 mal, um erfolgreiche Verbindung anzuzeigen
      connected = true;
      Light.turnOn(); // LED bleibt an, wenn die Verbindung erfolgreich ist
      delay(1000); // Kurze Pause, um die erfolgreiche Verbindung anzuzeigen
      break; // Verbindung erfolgreich, Schleife verlassen
    } else {
      Serial.println("Fehler beim Verbinden des PS4 Controllers");
      Serial.println("Versuch " + String(i) + " von 3...");
      delay(3000); // Wartezeit vor erneutem Versuch
      Serial.println("Versuche erneut zu verbinden...");
      for (int j = 0; j < 5; j++) Light.blink(100); // LED blinkt 5 mal, um den Verbindungsversuch anzuzeigen 
      delay(1000); // Kurze Pause vor dem nächsten Versuch
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
    if (PS4.Share()) Light.toggle(); 
    
    // Tummersteuerung
    if (PS4.L1()) turnTower(true);
    else if (PS4.R1()) turnTower(false);
    else stopTower();

    // Bewegungssteuerung
    if (PS4.Up()) drive(true); // Vorwärts fahren
    else if (PS4.Down()) drive(false); // Rückwärts fahren
    else if (PS4.Left()) turn(true);
    else if (PS4.Right()) turn(false);
    else stopMovement();

    // Oberarmsteuerung
    if (PS4.Triangle()) moveUpperArm(true);
    else if (PS4.Circle()) moveUpperArm(false);
    else stopUpperArm();

    // Unterarm
    if (PS4.Cross()) moveLowerArm(true);
    else if (PS4.Square()) moveLowerArm(false);
    else stopLowerArm();

    // Schaufelsteuerung
    if (PS4.L2()) moveShovel(true);
    else if (PS4.R2()) moveShovel(false);
    else stopShovel();
    
    // Alle Motoren stoppen, wenn Share gedrückt wird
    if (PS4.Options()) StopAllMotors(); 
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