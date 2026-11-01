#include <Dynamixel2Arduino.h>
#include <cmath> // Nécessaire pour std::floor
// ----------------------------------------------------------------------
// 1) CONFIGURATION MATERIELLE (port serie + broche de direction half-duplex)
// ----------------------------------------------------------------------
#if defined(ARDUINO_OpenCM904)
  #define DXL_SERIAL Serial3     // Port DXL integre a l'OpenCM9.04
  #define DEBUG_SERIAL Serial      // Port USB, utilise pour le moniteur serie
  const int DXL_DIR_PIN = 22;      // Broche de direction du port DXL integre
  #define NEED_DXL_MODE_SETUP      // Marqueur : il faut appeler Serial1.setDxlMode(true)
#else
  // Repli generique (ex: Arduino Uno + Dynamixel Shield) - a adapter si besoin
  #define DXL_SERIAL   Serial1
  #define DEBUG_SERIAL Serial
  const int DXL_DIR_PIN = 2;
#endif

const uint8_t ID1 = 7;
const float DXL_PROTOCOL_VERSION = 1.0;    // AX-12A -> protocole DYNAMIXEL 1.0
const long  DXL_BAUDRATE = 1000000;        // 1 000 000 bps = baudrate d'usine AX-12A

float t0 = millis();

float fileCommande[10]; // buffer de commandes en attentes la commande 1 correspond à la commande en cour
int lenFilCommande = 0;
float CommandeEnCours[6];

int ID[5]; //listes des ID des 5 moteurs
float ZMdebut = 200;
float ZMfin = 3;

float V = 10;


Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
using namespace ControlTableItem;     

void setup(){
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL) { ; }   // attend l'ouverture du moniteur serie
  Serial.println('-1');

  #ifdef NEED_DXL_MODE_SETUP
  // Indispensable sur OpenCM9.04 : bascule Serial1 en mode half-duplex DYNAMIXEL
    Serial1.setDxlMode(true);
  #endif
  dxl.begin(DXL_BAUDRATE);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  if (dxl.ping(ID1)) {
    DEBUG_SERIAL.print(F("Servo detecte, ID = "));
    DEBUG_SERIAL.println(ID1);

  } else {
    DEBUG_SERIAL.println(F("ERREUR : aucun servo ne repond a cet ID/baudrate."));
    DEBUG_SERIAL.println(F("Verifiez : alimentation externe 7-12V, cablage TTL, ID, baudrate."));
    DEBUG_SERIAL.println(F("Astuce : utilisez DYNAMIXEL Wizard 2.0 (via U2D2/USB2Dynamixel)"));
    DEBUG_SERIAL.println(F("pour verifier ou reinitialiser l'ID et le baudrate du servo."));
    while (true) { delay(1000); }  // on bloque le programme, inutile de continuer
  }

  //dxl.setOperatingMode(ID1, OP_POSITION);
  dxl.setOperatingMode(ID1, OP_VELOCITY);
  dxl.torqueOn(ID1);
  dxl.writeControlTableItem(MOVING_SPEED, ID1, 300);

  
 
}

void loop() {


// Vérifie si un message complet est arrivé
  if (Serial.available() > 0) {
    // Lit le message jusqu'au caractère de fin '\n'
    String trame = Serial.readStringUntil('\n'); 

    float q1,q2,q3,q4,p

    // sscanf analyse la trame selon le format "lettre:entier:entier"
    // et stocke les résultats dans nos variables
    if (sscanf(trame.c_str(), "%c:%d:%d", &q1, &q2, &q3) == 3) {
      
      // a terminer ajouter à la file la commande recu

      // faire passer en priorité arret d'urgence
    }
  }

  // On met a jour l'éxecution des commandes
  if (lenfileCommande > 0){//Si il y a une commande en cours d'éxecution 

    // On verifie si elle a atteint sa position
    // La position est atteint si on se trouve dans l'intervalle définis pour un axe
    // Si ce n'est pas le cas on met a jour la commande des moteurs
    
    for(i = 0; i<=6;i++){// On parcourt chacun des axes

      VerifCommandeAxe(pos[i],CommandeActuelle[i],errr[i],ID[i])

    }

  }



}
  

// Définition de l'ensemble des fonctions permettant de réaliser le déplacement du robot

void NCommande(com){
    // Ajoute une nouvelle commande à la file de commande à executer
    fileCommande[lenfileCommande] = com;
    lenfileCommande += 1; 

}

void VerifCommandeAxe(q_pos,q_targ,q_e,ID){
  
  // On verifie si on a atteint la pos targ
  if (q_pos>q_targ-qe && q_pos< q_targ + qe){
    
  }else{// Si la position finale n'est pas atteinte il faut vérifier si on est pas dans l'approche finale
    // On calcule les bornes dans lequelles on est dans la zone d'approche finale
  
    float p = std::floor(q_targ/360);//p est le nombre de tours que doit effectuer le moteur à partir de la pos d'origine pour atteindre la pos souhaité
    if(q_pos > ZMfin+p*360 && q_pos < ZMdebut + p*360){// on est dans la zone d'approche finale
      PosPrecision(q_targ- p*360 ,ID)

    }else:{
      // On conserve l'approche générale

      // Il faut calculer le sens..
      if(q_pos>q_target){
        // tourner dans le sens négatif => vérifer le sens des moteurs
      }else{{
        // Sens positif ?

      }}
    }


  }


}

void PosPrecision(q_targ,ID){
  dxl.setGoalPosition(ID , q_targ, UNIT_DEGREE); 
}


void angleComm(){
  int target = 40;
  dxl.setGoalPosition(ID1 , target, UNIT_DEGREE);  
  trackMvt(target);
  
  target = 150;
  dxl.setGoalPosition(ID1 , target, UNIT_DEGREE);  
  trackMvt(target);
}

void trackMvt(int t){
  while (abs(t-dxl.getPresentPosition(ID1, UNIT_DEGREE))>1){
    int rawSpeed=dxl.readControlTableItem(PRESENT_SPEED,ID1);
    float SpeedRPM=(rawSpeed%1024)*0.111;
    //Serial.print(dxl.getPresentPosition(ID1, UNIT_DEGREE));
    Serial.print(SpeedRPM);
    Serial.print(";");
    Serial.println(millis()-t0);

  }
}

void printAngle() {
  float mesure = dxl.getPresentPosition(ID1, UNIT_DEGREE);
  //DEBUG_SERIAL.print(F("Position mesuree : "));
  DEBUG_SERIAL.print(mesure);
  //DEBUG_SERIAL.println(F(" deg"));
}