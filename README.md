# RC-Excavator

# Excavator Control System

Dieses Projekt steuert einen ferngesteuerten Bagger mit mehreren Motoren, die verschiedene Teile des Baggers bewegen. Es wurde entwickelt, um Bewegungen wie das Fahren, Drehen des Turms und das Bewegen von Armen und Schaufel zu ermöglichen.

## Funktionen

- **Bewegung**: Steuerung der Kettenmotoren für Vorwärts-, Rückwärts- und Drehbewegungen.
- **Turmsteuerung**: Drehen des Turms nach links oder rechts.
- **Armsteuerung**:
  - Oberarm: Hoch- und Runterbewegung.
  - Unterarm: Hoch- und Runterbewegung.
- **Schaufelsteuerung**: Öffnen und Schließen der Schaufel.
- **Notstopp**: Stoppt alle Motoren gleichzeitig.

## Hardwareanforderungen

- ESP32 oder Arduino-kompatibler Mikrocontroller.
- Motorsteuerungen für:
  - Linke und rechte Kettenmotoren.
  - Turmmotor.
  - Oberarmmotor.
  - Unterarmmotor.
  - Schaufelmotor.
- DC-Motoren oder Servomotoren.
- Stromversorgung (z. B. Akku).
- Verbindungskabel.

## Softwareanforderungen

- Arduino IDE (mit ESP32-Unterstützung, falls verwendet).
- Abhängige Bibliotheken:
  - Motorsteuerungsbibliothek (z. B. `Servo.h` oder spezifische Bibliotheken für die verwendeten Motoren).

## Installation

1. Klone dieses Repository:
   ```bash
   git clone https://github.com/dein-benutzername/excavator-control.git
2. Öffne die Datei excavator.ino in der Arduino IDE.
3. Installiere alle benötigten Bibliotheken über den Bibliotheksmanager der Arduino IDE.
4. Lade den Code auf deinen Mikrocontroller hoch.

## Verwendung

- Bewegung: Verwende die Funktionen stopMovement(), turnTower(bool left), moveUpperArm(bool up), etc., um die Motoren zu steuern.
- Notstopp: Rufe die Funktion StopAllMotors() auf, um alle Motoren sofort zu stoppen.

## Codeübersicht

- stopMovement(): Stoppt die Kettenmotoren.
- turnTower(bool left): Dreht den Turm nach links oder rechts.
- moveUpperArm(bool up): Bewegt den Oberarm nach oben oder unten.
- moveLowerArm(bool up): Bewegt den Unterarm nach oben oder unten.
- moveShovel(bool up): Bewegt die Schaufel nach oben oder unten.
- StopAllMotors(): Stoppt alle Motoren.

## Lizenz

Dieses Projekt steht unter der MIT-Lizenz.