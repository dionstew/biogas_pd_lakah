/*****

  All the resources for this project:
  http://randomnerdtutorials.com/

*****/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11
#define trig_pin D4
#define echo_pin D2 
#define press_in A0

// Parameter Tangki
#define D_tb 120  // masukkan angka disini!
#define t_tb 180   // masukkan angka disini!
#define PI 3.14

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

// DHT Sensor - GPIO 6 on ESP-12E NodeMCU board
const int DHTPin = 6;
DHT dht(DHTPin, DHTTYPE);

// Variabel untuk diupload ke MQTT broker
String presStr, volStr, tempStr, humStr;

// Variable for ultrasonic_sen
float distance, offset = 0.9, Vb = 0.0;
long duration = 0;

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

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
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
void ultrasonic_sen() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo_pin, HIGH);

  // Calculating the distance
  distance = (duration * 0.034 / 2) - offset;
}
void tekanan_sen(){
  int nilai_adc = analogRead(0);
  int tegangan_sensor = (nilai_adc*5.00/1024)-offset_tekanan_ruang;
  tekanan_ruang = (400*tegangan_sensor)-200;
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  pinMode(DHTPin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  dht.begin();

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP8266Client");

  now = millis();

  // Jika menambahkan interface input, tambahkan dibawah ini!
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 30000) {
    lastMeasure = now;
    // Pembacaan Sensor DHT11
    float h = dht.readHumidity();     // Membaca nilai kelembaban
    float t = dht.readTemperature();  // Membaca suhu dalam satuan Celcius 
    // Pengecekan nilai DHT, jika gagal bisa exit cepat dan baca lagi
    if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Menghitung nilai temperatur dalam satuan Celcius
    float hic = dht.computeHeatIndex(t, h, false);
    
    // Mendeteksi & Menghitung volume biomass
    ultrasonic_sen();
    Vb = PI * ((D_tb-distance)/2) * t_tb; 

    // Mendeteksi & Menghitung Tekanan
    tekanan_sen(); // Didalam fungsinya juga terdapat hitungan untuk mencari tekanan
    
    // Variable yang dikonversi ke string: nilai tekanan, suhu, kelembaban, volume.
    tempStr = String(hic);   
    humStr  = String(h);
    volStr  = String(Vb);
    presStr = String(tekanan_ruang);
    
    // Publishes Temperature and Humidity values
    client.publish("room/Temperature", tempStr.c_str());
    client.publish("room/Humidity", humStr.c_str());
    client.publish("room/Volume", volStr.c_str());
    client.publish("room/Pressure", presStr.c_str());
    Serial.print("Temperature (C): ");
    Serial.print(hic);
    Serial.print(" \t Kelembaban: ");
    Serial.print(h);
    Serial.print("\t Volume biomass: ");
    Serial.print(Vb);
    Serial.print("\t Tekanan ruang biogas: ");
    Serial.println(tekanan_ruang);
  }
}
