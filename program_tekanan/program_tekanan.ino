#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define press_in A0

// Parameter Server
#define IP_address 192.168.43.254

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "OPPOAAAA";
const char* password = "qwertyuiop";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "IP_address";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// Variabel untuk diupload ke MQTT broker
String presStr;

// Variable untuk sensor tekanan
float tekanan_ruang;
const float offset_tekanan_ruang = 0.07;

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void tekanan_sen(){
  int nilai_adc = analogRead(0);
  float tekanan = (0.0783 * nilai_adc) - 96398
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");

  now = millis();

  if (now - lastMeasure > 30000) // Setiap 30 detik publish nilai baru
  {
    lastMeasure = now;
    
//---------> Mendeteksi & Menghitung Tekanan
    tekanan_sen(); // Didalam fungsinya juga terdapat hitungan untuk mencari tekanan
    
//---------> Variable yang dikonversi ke string: nilai tekanan, suhu, kelembaban, volume.
    presStr = String(tekanan_ruang);

//---------> Publishes Temperature and Humidity values
    client.publish("room/Pressure", presStr.c_str());
    
//---------> Print serial nilai tekanan
    Serial.print("Tekanan Ruang: ");
    Serial.print(tekanan_ruang);
    }
}
