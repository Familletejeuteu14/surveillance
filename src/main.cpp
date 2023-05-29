#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_CCS811.h>
#include <WiFi.h>
#include <HTTPClient.h>
 #include <SPI.h>

const int photoresistorPin = 36;  // Broche de la photoresistance

Adafruit_CCS811 ccs;  // Objet pour le capteur CCS811

const char* ssid = "UNIFI_IDO1";
const char* password = "42Bidules!";
void sendDataToDatabase(int luminosite, int co2, int tvoc);
void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!ccs.begin()) {
    Serial.println("Échec de la communication avec le capteur CCS811. Vérifiez les connexions.");
    while (1);
  }

  // Configuration du capteur CCS811
  ccs.setDriveMode(CCS811_DRIVE_MODE_1SEC);

  // Connexion au réseau WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("Connecté au réseau WiFi");
}

void loop() {
  // Lecture de la valeur de la photoresistance
  int photoresistorValue = analogRead(photoresistorPin);
  Serial.print("Luminosité : ");
  Serial.println(photoresistorValue);

  // Lecture de la valeur du CO2 et du TVOC (Total Volatile Organic Compounds)
  if (ccs.available()) {
    if (!ccs.readData()) {
      int eCO2 = ccs.geteCO2();
      int TVOC = ccs.getTVOC();
      Serial.print("CO2 : ");
      Serial.print(eCO2);
      Serial.print(" ppm\t");
      Serial.print("TVOC : ");
      Serial.print(TVOC);
      Serial.println(" ppb");

      // Envoi des données vers la base de données
      sendDataToDatabase(photoresistorValue, eCO2, TVOC);
    }
  }

  delay(1000);
}

void sendDataToDatabase(int luminosite, int co2, int tvoc) {
  // Construction de l'URL de la requête POST
  String url = "http://192.168.1.145:3000/donnees";

  // Création du corps de la requête JSON
  String jsonBody = "{\"luminosite\":";
  jsonBody += luminosite;
  jsonBody += ",\"co2\":";
  jsonBody += co2;
  jsonBody += ",\"tvoc\":";
  jsonBody += tvoc;
  jsonBody += "}";

  // Configuration de la requête HTTP
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Envoi de la requête POST avec le corps JSON
  int httpResponseCode = http.POST(jsonBody);

  // Vérification de la réponse du serveur
  if (httpResponseCode == 200) {
    Serial.println("Données envoyées avec succès à la base de données");
  } else {
    Serial.print("Erreur lors de l'envoi des données à la base de données. Code de réponse : ");
    Serial.println(httpResponseCode);
  }

  // Libération des ressources
  http.end();
}

