/*
  ======================================================================
  TEST DYNAMIXEL AX-12A sur carte OpenCM9.04
  Bibliotheque utilisee : Dynamixel2Arduino (officielle ROBOTIS)
  ======================================================================
  Ce programme :
   1) Detecte le servo connecte (ping)
   2) Teste le mode POSITION (deplacement vers plusieurs angles, 0-300 deg)
   3) Teste le mode ROUE / VELOCITY (rotation continue, 2 sens, vitesse reglable)

  A ADAPTER avant utilisation (section "PARAMETRES" ci-dessous) :
   - DXL_ID              : identifiant du servo a tester (1 par defaut en usine)
   - DXL_BAUDRATE         : vitesse de communication (1 000 000 bps = valeur d'usine AX-12A)
   - WHEEL_SPEED_PERCENT  : vitesse utilisee pour le test en mode roue

  IMPORTANT (specifique OpenCM9.04) :
   - Le port DXL 3 broches integre a la carte correspond a Serial1, broche
     de direction 28. Il FAUT appeler Serial1.setDxlMode(true) avant
     dxl.begin(), sinon la communication ne passe pas.
   - Si vous utilisez la carte d'extension OpenCM 485EXP a la place du port
     integre, changez DXL_SERIAL pour Serial3 et DXL_DIR_PIN pour 22
     (et retirez l'appel a setDxlMode, qui n'est pas necessaire dans ce cas).
   - Verifiez que Outils > Type de carte est bien reglé sur "OpenCM9.04"
     dans l'IDE Arduino, sinon la detection automatique ci-dessous ne
     fonctionnera pas.
   - Les AX-12A doivent etre alimentes en 7-12V externe (pas seulement l'USB).
  ======================================================================
*/

#include <Dynamixel2Arduino.h>

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

// ----------------------------------------------------------------------
// 2) PARAMETRES DU TEST (a adapter selon votre servo / vos besoins)
// ----------------------------------------------------------------------
const uint8_t ID1 = 0;//moteur 1
const uint8_t ID2 = 9;  // moteur 2
const uint8_t ID3 = 23;  // moteur 3       // ID du servo (1 = valeur d'usine)
const float DXL_PROTOCOL_VERSION = 1.0;    // AX-12A -> protocole DYNAMIXEL 1.0
const long  DXL_BAUDRATE = 1000000;        // 1 000 000 bps = baudrate d'usine AX-12A

const float WHEEL_SPEED_PERCENT = 30.0;    // vitesse test mode roue, en % du max (-100 a 100)

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
using namespace ControlTableItem;           // permet d'utiliser OP_POSITION, OP_VELOCITY, UNIT_...

// ----------------------------------------------------------------------
void setup() {
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL) { ; }   // attend l'ouverture du moniteur serie

 #ifdef NEED_DXL_MODE_SETUP
    // Indispensable sur OpenCM9.04 : bascule Serial1 en mode half-duplex DYNAMIXEL
    Serial1.setDxlMode(true);
 #endif

  dxl.begin(DXL_BAUDRATE);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);

  DEBUG_SERIAL.println(F("=== Recherche du servo AX-12A ==="));
  DEBUG_SERIAL.println(F("=== Recherche des ID en cours ==="));
  
  // On teste toutes les adresses possibles de 0 à 252
  for (int i = 0; i <= 252; i++) {
    if (dxl.ping(i)) {
      DEBUG_SERIAL.print(F("-> Moteur detecte ! Son ID est : "));
      DEBUG_SERIAL.println(i);
    }
  }
  
  DEBUG_SERIAL.println(F("=== Recherche terminee ==="));
  Serial.print("Scan: ");
  Serial.println(dxl.scan());

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

  testPositionMode();
  delay(1000);
  //testWheelMode();

  DEBUG_SERIAL.println(F("\n=== Tests termines ==="));
}

void loop() {
  // Rien ici : les tests s'executent une seule fois, dans setup().
}

// ----------------------------------------------------------------------
// TEST 1 : MODE POSITION (asservissement en angle, 0-300 deg pour l'AX-12A)
// ----------------------------------------------------------------------
void testPositionMode() {
  DEBUG_SERIAL.println(F("\n--- TEST MODE POSITION ---"));

// On change le mode de fonctionnement des moteurs
  dxl.torqueOff(ID1);
  dxl.torqueOff(ID2); 
  dxl.torqueOff(ID3);    
  // ON setup le mode de fonctionnement                  // obligatoire pour changer le mode de fonctionnement
  dxl.setOperatingMode(ID1, OP_POSITION);
  dxl.setOperatingMode(ID2, OP_POSITION);
  dxl.setOperatingMode(ID3, OP_POSITION); // mode "joint" (position)

  dxl.torqueOn(ID1);
  dxl.torqueOn(ID2);
  dxl.torqueOn(ID3);

  //float positions[] = {0.0, 150.0, 300.0, 150.0}; // angles a tester, en degres
  //const int nbPositions = sizeof(positions) / sizeof(positions[0]);
  float pos = 296;
  for (int i = 0; i < 1; i++) {
    DEBUG_SERIAL.print(F("Consigne : "));
    DEBUG_SERIAL.print(30);
    DEBUG_SERIAL.println(F(" deg"));
    dxl.setGoalVelocity(ID1, 20.0, UNIT_PERCENT);
    dxl.setGoalPosition(ID1, pos, UNIT_DEGREE);
    dxl.setGoalPosition(ID2, pos, UNIT_DEGREE);
    dxl.setGoalPosition(ID3, pos, UNIT_DEGREE);
    delay(3000); // laisse le temps au servo d'atteindre la position

    float mesure = dxl.getPresentPosition(ID1, UNIT_DEGREE);
    DEBUG_SERIAL.print(F("Position mesuree : "));
    DEBUG_SERIAL.print(mesure);
    DEBUG_SERIAL.println(F(" deg"));
  }

  float pos2 = 0;

    for (int i = 0; i < 1; i++) {
    DEBUG_SERIAL.print(F("Consigne : "));
    DEBUG_SERIAL.print(pos2);
    DEBUG_SERIAL.println(F(" deg"));
    dxl.setGoalVelocity(ID1, 20.0, UNIT_PERCENT);
    dxl.setGoalPosition(ID1, pos2, UNIT_DEGREE);
    dxl.setGoalPosition(ID2, pos2, UNIT_DEGREE);
    dxl.setGoalPosition(ID3, pos2, UNIT_DEGREE);
    delay(3000); // laisse le temps au servo d'atteindre la position

    float mesure = dxl.getPresentPosition(ID1, UNIT_DEGREE);
    DEBUG_SERIAL.print(F("Position mesuree : "));
    DEBUG_SERIAL.print(mesure);
    DEBUG_SERIAL.println(F(" deg"));
  }
}


