#include <PS4Controller.h>
#include <Arduino.h>
#include <regex>

// Klasse für die Motorsteuerung
class Engine {
private:
  int pin1;
  int pin2;

public:
  Engine(int in1, int in2) { // Konstruktor mit zwei Pins
    pin1 = in1;
    pin2 = in2;
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
  }

  // Motorsteuerung 
  // Vorwärts(true) oder rückwärts(false) drehen
  void spin(bool forward) {
    if (forward) {
      startSpin(pin1, pin2);
    } else {
      startSpin(pin2, pin1);
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
  char macAddress[18]; // Array für die MAC-Adresse
public:
  GamePad() { // Konstruktor
    strcpy(macAddress, "00:00:00:00:00:00"); // Standard-MAC-Adresse, falls ungültig
  }

  // Validierung der MAC-Adresse
  bool isValidMac(const char* mac) {
    std::regex macRegex("([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})");
    return std::regex_match(mac, macRegex);
  }
};

// Erstellen der ObjektInstanzen 

GamePad Controller; // PS4 Controller

Engine LeftChainEngine(13, 12); // Kettenmotor links
Engine RightChainEngine(14, 27); // Kettenmotor rechts
Engine TurretEngine(15, 2); // Turmmotor
Engine LowerArmEngine(4, 16); // Unterarmmotor
Engine UpperArmEngine(17, 5); // Oberarmmotor
Engine ShovelEngine(18, 19); // Schaufelmotor

LED Light(21);

// Aufbau und Hauptsteuerung

void setup() {
  Serial.begin(115200);

  Light.turnOn(); // LED einschalten
  
  // PS4 Controller mit Mac initialisieren
  bool connected = false;

  Serial.println("Versuche mit Controller zu Verbinden.");

  for (int i = 1; i <= 3; i++) {
    Light.blink(500); // LED blinkt, um den Verbindungsversuch anzuzeigen
    if (PS4.begin("78:1C:3C:E6:6D:4A")) { // MAC des Bluetooth modules: 78:1C:3C:E6:6D:4A
      for (int j = 0; j < 3; j++) Light.blink(100); // LED blinkt 3 mal, um erfolgreiche Verbindung anzuzeigen
      connected = true;
      Light.turnOn(); // LED bleibt an, wenn die Verbindung erfolgreich ist
      delay(1000); // Kurze Pause, um die erfolgreiche Verbindung anzuzeigen
      Serial.println("Verbindung mit Controller Erfolgreich");
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
    Serial.println("Vrbindung fehlgeschlagen");
    while (true); // Endlosschleife, um das Programm anzuhalten
  }

  delay(2000);
  Light.turnOff(); // LED ausschalten
}

void loop() {
  if (PS4.isConnected()) {
    // LED umschalten
    if (PS4.Share()) {
      Light.toggle();
      delay(500);
    } 
    
    // Turmsteuerung
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
  LeftChainEngine.spin(forward); // Linker Motor fährt vorwärts oder rückwärts
  RightChainEngine.spin(forward); // Rechter Motor fährt vorwärts oder rückwärts
}

void turn(bool left) {
  if (left) { // Wenn nach links gedreht wird
    LeftChainEngine.stop(); // Linker Motor stoppt
    RightChainEngine.spin(true); // Rechter Motor fährt vorwärts
  } else { // Wenn nach rechts gedreht wird
    LeftChainEngine.spin(true); // Linker Motor fährt vorwärts
    RightChainEngine.stop(); // Rechter Motor stoppt
  }
}

void stopMovement() {
  LeftChainEngine.stop(); // Linker Motor stoppt
  RightChainEngine.stop(); // Rechter Motor stoppt
}

void turnTower(bool left) {
  TurretEngine.spin(left); // Turmmotor dreht in die angegebene Richtung
}

void stopTower() {
  TurretEngine.stop(); // Turmmotor stoppt
}

void moveUpperArm(bool up) {
  UpperArmEngine.spin(up); // Oberarmmotor fährt in die angegebene Richtung
}

void stopUpperArm() {
  UpperArmEngine.stop(); // Oberarmmotor stoppt
}

void moveLowerArm(bool up) {
  LowerArmEngine.spin(up); // Unterarmmotor fährt in die angegebene Richtung
}

void stopLowerArm() {
  LowerArmEngine.stop(); // Unterarmmotor stoppt
}

void moveShovel(bool up) {
  ShovelEngine.spin(up); // Schaufelmotor fährt in die angegebene Richtung
}


void stopShovel() {
  ShovelEngine.stop(); // Schaufelmotor stoppt
}

void StopAllMotors() {
  stopMovement(); // Alle Bewegungsmotoren stoppen
  stopTower(); // Turmmotor stoppen
  stopUpperArm(); // Oberarmmotor stoppen
  stopLowerArm(); // Unterarmmotor stoppen
  stopShovel(); // Schaufelmotor stoppen
}