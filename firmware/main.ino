#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>
#include <Encoder.h>

#define TFT_CS 7
#define TFT_RST 10
#define TFT_A0 8
//TFT_SCK 4
//TFT_SDA 6

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_A0, TFT_RST);

const char* SSID = xx
const char* PASSWORD = xxx
const char* CLIENT_ID = xxx
const char* CLIENT_SECRET = xx
Spotify sp(CLIENT_ID, CLIENT_SECRET);

const int switchPins[] ={2, 5, 9};
const int numSwitches = 3;

const int en_CLK_PIN = 0;
const int en_DT_PIN = 1;
const int en_SW_PIN = 3;
long lastEncoderValue = 0;
Encoder encoder(en_CLK_PIN, en_DT_PIN);

String lastArtist = "";
String lastTrackname = "";

unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    delay(3000);
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
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
        delay(10);
    }
    Serial.println("Connected to Spotify!");

    Serial.println("Your refresh token is:");
    Serial.println(sp.get_user_tokens().refresh_token);

    for (int i = 0; i < numSwitches; i++)
    {
    pinMode(switchPins[i], INPUT_PULLUP);
    }
    
    pinMode(en_CLK_PIN, INPUT_PULLUP);
    pinMode(en_DT_PIN, INPUT_PULLUP);
    encoder.write(0);
    pinMode(en_SW_PIN, INPUT_PULLUP);
}


void loop() {
    if (millis() - lastUpdate > 2000){
        lastUpdate = millis();
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
        sp.previous();
        delay(300);
    }
    else if (state2 == LOW){
        sp.start_resume_playback();
        delay(300);
    }
    else if (state3 == LOW){
        sp.skip();
        delay(300);
    }

    long currentEncoderValue = encoder.read();
    if (currentEncoderValue != lastEncoderValue){
        int Volume = (int)constrain(currentEncoderValue, 0, 100);
        sp.set_volume(Volume);
        lastEncoderValue = Volume;
    }
}
