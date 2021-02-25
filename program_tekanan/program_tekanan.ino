#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define press_in A0

// Parameter Server
#define IP_address 192.168.43.254

// Inisialisasi server dan client
const char* ssid = "OPPOAAAA";
const char* password = "qwertyuiop";
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espclient;
PubSubClient client(espclient);

// Variabel untuk diupload ke MQTT broker
#define MSG_BUFFER_SIZE  (50)
char pressStr[MSG_BUFFER_SIZE];
int nilai_adc;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("status", "Hello! Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void tekanan_sen(){
  nilai_adc = analogRead(0);
  //float tegangan_sensor = (nilai_adc*5.00/1024)-offset_tekanan_ruang;
  //tekanan_ruang = (400*tegangan_sensor)-200;
  tekanan_ruang = (0.5386*nilai_adc) - 66.117;
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
    if (tekanan_ruang<0){ tekanan_ruang = 0;}
//    tekanan_ruang++;
    
//---------> Variable yang dikonversi ke string: nilai tekanan, suhu, kelembaban, volume.
    snprintf (pressStr, MSG_BUFFER_SIZE, "%f", tekanan_ruang);

//---------> Nilai variabel tekanan yang sudah dikonversi diupload
    client.publish("room/Pressure", pressStr);
    
//---------> Print serial nilai tekanan
    Serial.print("Tekanan Ruang: ");
    Serial.print(nilai_adc);
    Serial.print("\tTekanan Ruang: ");
    Serial.println(tekanan_ruang);
    }
}
