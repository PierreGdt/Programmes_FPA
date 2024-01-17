// Programme prototype plateforme équipe 6 THIBAUT Pauline GODET Pierre PESCATORE MENARD Raphaël
/*
Programme permettant de contrôler le véhicule sur une distance de 2 m et de contourner jusqu'à 2 objets par la gauche ou par la droite suivant la position des obstacles détectés.
*/

// Importation des bibliothèques
#include <Arduino.h>
#include "Servo.h"

Servo monServo; // création de l'objet "servo"

// Affectation des pins

// Pins pour les moteurs
const byte PWMA = 3;  // PWM control (speed) for motor A
const byte PWMB = 11; // PWM control (speed) for motor B
const byte DIRA = 12; // Direction control for motor A
const byte DIRB = 13; // Direction control for motor B

// Définition des variables

// Pins pour le capteur à ultrason
#define trigPin 9
#define echoPin 8

// Affectation de variables pour définir le sens de rotation.
#define CW  0 // sens horaire
#define CCW  1 // sens anti-horaire

// création d'une variable pour chaque moteur.
#define MOTOR_A 0
#define MOTOR_B 1

long temps; // variable qui stocke la mesure du temps
float distancedepart; // variable qui stocke la distance parcourue par la plateforme depuis le départ.


const float MAX_SPEED = 2/4.42 ; // m/s vitesse mesurée

//paramètres pour les accumulateurs.
#define BATTERY_IN_PIN A0
#define LED_BATTERY_OUT 4
const float dividerBridgeRatio = 0.5;
const float BATTERY_MAX_VOLTAGE = 9.;
const float CORRECTION_COEFFICIENT = 1.118;
const float BATTERY_LOW_VOLTAGE = 8.;

// fonction pour allumer la led  dès que la tension 'voltage' est inférieure à 'BATTERY_LOW_VOLTAGE'
void turn_battery_indicator_on (float voltage) {
    if (voltage < BATTERY_LOW_VOLTAGE && voltage > 1) {
        digitalWrite(LED_BATTERY_OUT, HIGH);
    } else {
        digitalWrite(LED_BATTERY_OUT, LOW);
    }
}

// fonction pour afficher la tension des accumulateurs.
void print_battery_voltage(float voltage) {
    Serial.print("Battery voltage : ");
    Serial.print(voltage);
    Serial.println("V");
}

// fonction pour mesurer la tension des accumulateurs.
float get_battery_voltage() {
    int batteryAnalogValue = analogRead(BATTERY_IN_PIN);

    float voltage = CORRECTION_COEFFICIENT * BATTERY_MAX_VOLTAGE / 1023 * batteryAnalogValue;

    return voltage;
}



// driveArdumoto permet de contrôler un moteur nommé 'motor' dans le sens 'dir' à la vitesse'spd'
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
  else if (motor == MOTOR_B)
  {
    digitalWrite(DIRB, dir);
    analogWrite(PWMB, spd);
  }
}

// stopArdumoto permet d'arrêter le moteur 'motor'
void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}


 // Fonction qui permet de mesurer la distance entre la plateforme et un obstacle
float get_distance() {

    // initialisation : le pin d'envoi est mis à 0
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // envoi d'une impulsion de 10-microsecond au pin 'trigPin', permet d'envoyer une onde ultrasonore.
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
   
    // récupération de l'onde avec le capteur et mesure du temps auquel l'onde est reçue.
    float duration_us = pulseIn(echoPin, HIGH);

    // calcul de la distance entre le véhicule et l'obstacle
    float distance_cm = 0.017 * duration_us; // conversion en centimètre

    if (distance_cm < 200 && distance_cm > 2) {
        return distance_cm; // renvoi de la valeur de distance lorsque l'un obstacle destde distance  détecté 
    } else {
        return -1.0;
    }

}






// setupArdumoto initialise tous lespins
void setupArdumoto()
{
    // les pins sont définis comme des sorties
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(DIRA, OUTPUT);
    pinMode(DIRB, OUTPUT);

    // aucun pin n'est alimenté initialement
    digitalWrite(PWMA, LOW);
    digitalWrite(PWMB, LOW);
    digitalWrite(DIRA, LOW);
    digitalWrite(DIRB, LOW);
}


void setup() {
    // begin serial port
    Serial.begin (9600);
    monServo.attach(2);
    // Ultrasound sensor
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);

    setupArdumoto(); // Set all pins as outputs
    Serial.print(get_distance());
    Serial.println(" cm");   
  // put your main code here, to run repeatedly:

    float distance = get_distance();
    Serial.print("distance: ");
    Serial.println(distance);
    Serial.println(" cm");
    distancedepart=0;

 
}
void avancer(byte sens, float distance) {
    // fonction qui permet d'avancer la plateforme en ligne droite
    // sens doit être CW pour avancer ou CCW pour reculer
    // distance est la distance à parcourir
    temps=millis();
    // mise en marche des moteurs. Les vitesses sont différentes car les moteurs ont des caractéristiques différentes.
    driveArdumoto(MOTOR_A, sens, 160);
    driveArdumoto(MOTOR_B, sens, 138);
    // le délai permet de définir la durée pendant laquelle les moteurs seront allumés. el
    int delai = int(distance / MAX_SPEED * 1000);
    Serial.println(delai);
    // Mesure de la distance initiale
    distance = get_distance();
   
    while (((millis()-temps)<delai && distance>25) || distance==-1){
      distance = get_distance();


}
  // Actualisation de la distance parcourue par la plateforme depuis le départ
    distancedepart=distancedepart+(millis()-temps)* MAX_SPEED/1000;
    // arrêt des moteur
    stopArdumoto(MOTOR_A);
    stopArdumoto(MOTOR_B);
    delay(1000);
}


void avancer2(byte sens, float distance) {
    // fonction qui permet d'avancer la plateforme en ligne droite
    // sens doit être CW pour avancer ou CCW pour reculer
    // distance est la distance à parcourir
     /*
    avancer2 est utilisée lors de l'étape de contournement. avancer2 diffère de avancer car avancer2 n'actualise pas la distance depuis le départ.
    avancer2 permet ainsi d'éviter de comptabiliser les déplacements latéraux de la plateforme lorsqu'un obstacle est contourné.
    Ces déplacements latéraux ne doivent pas être comptés car le véhicule ne progresse pas vers la zone cible lors des déplacements latéraux.*/ 
    temps=millis();
    // mise en marche ds moteurs
    driveArdumoto(MOTOR_A, sens, 160);
    driveArdumoto(MOTOR_B, sens, 138);
    // le délai permet de définir la durée pendant laquelle les moteurs seront allumés.ud
    int delai = int(distance / MAX_SPEED * 1000);
    Serial.println(delai);
    // on continue d'actionner les moteurs tant qu'un obstacle n'est pas détecté ou tant que la plateforme n'a pas parcouru la distance 'distance'
    while (millis()-temps<delai) {
  
}

    stopArdumoto(MOTOR_A);
    stopArdumoto(MOTOR_B);
    delay(1000);
}

void tourner(bool sens, float angle) {
    // fonction qui permet de faire tourner la plateforme dans un des deux sens.
    // sens = True pour tourner à gauche
    temps = millis();
    // rotation d'une valeur d'angle qui correspond en réalité à 90 degrés pour chaque moteur.
    if (sens) {
        driveArdumoto(MOTOR_A, CCW, 0);
        driveArdumoto(MOTOR_B, CCW,95);
        delay(600); // valeur de base 1000
    } else {
        driveArdumoto(MOTOR_A, CCW, 130);
        driveArdumoto(MOTOR_B, CCW, 0);
        delay(600); // valeur de base 1000
    }
  // arrêt des moteurs
    stopArdumoto(MOTOR_A);
    stopArdumoto(MOTOR_B);
    delay(100);}



void contournerParGauche() {
    // Fonction utilisée pour contourner un obstacle par la gauche
    tourner(true, 90);
    avancer2(CW, 0.35);
    tourner(false, 90);
    avancer(CW, 0.5);
    tourner(false, 90);
    avancer2(CW, 0.3);
    tourner(true, 90);

    // Etapes pour contourner l'obstacle
    // fonction tourner à gauche
    // avance (pendant x secondes)
    // rajouter fonction tourner à droite
    // avance (pendant x secondes)
    // rajouter fonction tourner à droite
    // avance (pendant x secondes)
    // rajouter fonction tourner à gauche
}

void contournerParDroite() {
    // Fonction utilisée pour contourner un obstacle par la droite
    tourner(false, 90);
    avancer2(CW, 0.35);
    tourner(true, 90);
    avancer(CW, 0.5);
    tourner(true, 90);
    avancer2(CW, 0.3);
    tourner(false, 90);

    // Etapes pour contourner l'obstacle
    // fonction tourner à droite
    // avance (pendant x secondes)
    // rajouter fonction tourner à gauche
    // avance (pendant x secondes)
    // rajouter fonction tourner à gauche
    // avance (pendant x secondes)
    // rajouter fonction tourner à droite
}

void loop() {
    // initialisation moteurs
    stopArdumoto(MOTOR_A);
    stopArdumoto(MOTOR_B);
    // avance du véhicule si la plateforme n'a pas parcouru 2 mètres.
    if (distancedepart<2) {
        avancer(0,2-distancedepart);
    }

    // Fonction principale du programme
    float voltage=get_battery_voltage();
    turn_battery_indicator_on(voltage);
    
    float distance = get_distance();
    
    // Cas où un obstacle est détecté : procédure de contournement soit par la droite, soit par la gauche.

    if (distance<20 && distance > 0) {
    stopArdumoto(MOTOR_A);
    stopArdumoto(MOTOR_B);
    monServo.write(60); // demande au servo de se déplacer à cette position  
    delay(1000);
    // vérification de l'emplacement de l'obstacle
    distance = get_distance();
    Serial.println("distance APRES DETECTION OBSTACLE: ");
    Serial.println(distance);
    // test de présence de l'obstacle à droite : si l'obstacle est présent, on contourne par la gauche.
     if (distance<15) {
      Serial.println("Contournement gauche");
      contournerParGauche();
     }
     else{
       // sinon l'obstacle est à gauche on contourne par la droite
      Serial.println("Contournement droite");
      contournerParDroite();
      
      }
      // retour du servomoteur à sa position initiale.
      monServo.write(90);
     }
        Serial.println("contourne");
        
    stopArdumoto(MOTOR_A);
    stopArdumoto(MOTOR_B);


    // reprendre le trajet tout droit
    }

    
    
   
