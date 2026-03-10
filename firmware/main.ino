#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>
#include <ESP32Encoder.h>

#define TFT_CS 7
#define TFT_RST 10
#define TFT_A0 8
//TFT_SCK 4
//TFT_SDA 6

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_A0, TFT_RST);

char* SSID = "INSERT_SSID_HERE";
char* PASSWORD = "INSERT_PASSWORD_HERE";
const char* CLIENT_ID = "CLIENT_ID";
const char* CLIENT_SECRET = "CLIENT_SECRET";
Spotify sp(CLIENT_ID, CLIENT_SECRET);

const int switchPins[] ={2, 5, 9};
const int numSwitches = 3;

ESP32Encoder encoder;
const int en_CLK_PIN = 0;
const int en_DT_PIN = 1;
const int en_SW_PIN = 3;
long lastEncoderValue = 0;

String lastArtist = "";
String lastTrackname = "";

unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    Serial.println("TFT Initialized!");
    tft.fillScreen(ST77XX_BLACK);

    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi...");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.printf("\nConnected!\n");

    tft.setCursor(0,0);
    tft.write(WiFi.localIP().toString().c_str());

    sp.begin();
    while(!sp.is_auth())
    {
        sp.handle_client();
    }
    Serial.println("Connected to Spotify!");

    for (int i = 0; i < numSwitches; i++)
    {
    pinMode(switchPins[i], INPUT_PULLUP);
    }
    
    ESP32Encoder::useInternalWeakPullResistors = puType::up;
    encoder.attachHalfQuad(en_CLK_PIN, en_DT_PIN);
    encoder.setCount(0);
    pinMode(en_SW_PIN, INPUT_PULLUP);
}


void loop() {
    if (millis() - lastUpdate > 2000){
        lastUpdate = millis()
        String currentArtist = sp.current_artist_names();
        String currentTrackname = sp.current_track_name();

        if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {
            tft.fillScreen(ST77XX_BLACK);
            lastArtist = currentArtist;
            Serial.println("Artist: " + lastArtist);
            tft.setCursor(10,10);
            tft.write(lastArtist.c_str());
        }

        if (lastTrackname != currentTrackname && currentTrackname != "Something went wrong" && currentTrackname != "null") {
            lastTrackname = currentTrackname;
            Serial.println("Track: " + lastTrackname);
            tft.setCursor(10,40);
            tft.write(lastTrackname.c_str());
        }
    }

    int state1 = digitalRead(switchPins[0]);
    int state2 = digitalRead(switchPins[1]);
    int state3 = digitalRead(switchPins[2]);

    if (state1 == LOW){
        sp.previos();
    }
    else if (state2 == LOW){
        sp.start_resume_playback();
    }
    else if (state3 == LOW){
        sp.skip();
    }

    long currentEncoderValue = encoder.getCount();
    if (currentEncoderValue != lastEncoderValue){
        int Volume = (int)constrain(currentEncoderValue, 0, 100);
        encoder.setCount(Volume);
        sp.set_volume(Volume);
        lastEncoderValue = Volume;
    }
}
