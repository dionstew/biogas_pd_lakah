/*****

  All the resources for this project:
  http://github.com/dionstew/biogas_pd_lakah

*****/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <DHT.h>
#include <MQUnifiedsensor.h>

#define DHTTYPE DHT11   // DHT 11
#define trig_pin D4
#define echo_pin D2 
#define mq_in A0

// Parameter Tangki
#define D_tb 100   // masukkan angka disini!
#define t_tb 113   // masukkan angka disini!
#define PI 3.14

// Inisialisasi server dan client
const char* ssid = "OPPOAAAA";
const char* password = "qwertyuiop";
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espclient;
PubSubClient client(espclient);

// DHT Sensor - GPIO 6 on ESP-12E NodeMCU board
#define DHTPin D6
DHT dht(DHTPin, DHTTYPE);

// Variable for ultrasonic_sen
float distance, offset = 0.9;
long duration = 0;

// Variabel penyimpanan nilai akhir sebelum dikonversi
float hic = 0;
float h = 0, t = 0;
float Vb = 0;
float kmet_percent = 0;

// Variabel untuk diupload ke MQTT broker
#define MSG_BUFFER_SIZE  (50)
char tempStr[MSG_BUFFER_SIZE];
char humStr[MSG_BUFFER_SIZE];
char volStr[MSG_BUFFER_SIZE];
char kmetStr_percent[MSG_BUFFER_SIZE];


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
      client.publish("statusTopic", "Hello, Connected !!");
    } else {
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


void setup() {
  //pinMode(DHTPin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  dht.begin();

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

  // Jika menambahkan interface input, tambahkan dibawah ini!
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 2000) {
    lastMeasure = now;
    
//--------> Pembacaan Sensor DHT11
    float h = dht.readHumidity();     // Membaca nilai kelembaban
    float t = dht.readTemperature();  // Membaca suhu dalam satuan Celcius 
    // Pengecekan nilai DHT, jika gagal bisa exit cepat dan baca lagi
    if (isnan(h) || isnan(t) ) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Menghitung nilai temperatur dalam satuan Celcius
    float hic = dht.computeHeatIndex(t, h, false);
    
    //--------> Mendeteksi & Menghitung volume biomass
    ultrasonic_sen();
    //Vb = PI * ((D_tb - distance) / 2) * t_tb; // volume tabung.
    Vb = 19 * 30 * (20.50- distance); // volume kotak. NB: khusus untuk demo
    if (Vb<0){Vb = 0;}
    
    //--------> Mendeteksi & menghitung kadar metana didalam ruangan
    ppm = X * analogRead(mq_in);
    //float RsRo_LPG = (-0.0001 * ppm) + 1.0449;
    //float RsRo_CH4 = (-0.00044 * ppm) + 3.0875;
    float RsRo_CO = (-0.0003 * ppm) + 3.7696;
    //kmet_percent = (-10 * RsRo_LPG) + 100;
    //kmet_percent = (-10 * RsRo_CH4) + 100;
    kmet_percent = (-10 * RsRo_CO) + 100;
    
    /*
    hic++;
    h++;
    Vb++;
    kmet_percent++;
    */
    // Variable yang dikonversi ke string: nilai tekanan, suhu, kelembaban, volume
    snprintf (tempStr, MSG_BUFFER_SIZE, "%f", hic);
    snprintf (humStr, MSG_BUFFER_SIZE, "%f", h);
    snprintf (volStr, MSG_BUFFER_SIZE, "%f", Vb);
    snprintf (kmetStr_percent, MSG_BUFFER_SIZE, "%f", kmet_percent);
      
    // Publishes Temperature and Humidity values
    client.publish("room/Temperature", tempStr);
    client.publish("room/Humidity", humStr);
    client.publish("room/Volume", volStr);
    client.publish("room/GasCons", kmetStr_percent);
    
    Serial.print("Temperature (C): ");
    Serial.print(hic);
    Serial.print(" \t Kelembaban: ");
    Serial.print(h);
    Serial.print("\t Volume biomass: ");
    Serial.print(Vb);
    Serial.print("\t Kadar Gas Metana: ");
    Serial.println(kmetStr_percent);
    Serial.print("\t Bonus: ");
    Serial.println(distance);
  }
}
