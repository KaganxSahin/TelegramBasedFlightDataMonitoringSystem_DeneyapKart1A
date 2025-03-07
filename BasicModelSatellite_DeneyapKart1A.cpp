// Wiring
/*
          Türkçe
                                                         
Aşağıdaki Deneyap modülleri I2C haberleşme konnektörü aracılığıyla Deneyap Kart 1A'ya daisy-chain (zincirleme) konfigürasyonunda bağlandı:

Deneyap GPS ve GLONASS Konumlandırma Modülü (Quectel L86-M33 modülü ve STM8S003F3 mikroişlemcisi)
Deneyap Basınç Ölçer (TE Connectivity MS5637-02BA03)
Deneyap 6 Eksenli Ataletsel Ölçüm Birimi (IMU) (ST Microelectronics LSM6DSM)
Deneyap Morötesi (UV) Işık Dedektörü (Lite-On LTR-390UV-01)

          English

The following Deneyap modules were connected in a daisy-chain configuration to Deneyap Kart 1A via the I2C communication connector:

Deneyap GPS and GLONASS Positioning Module (Quectel L86-M33 module with STM8S003F3 microprocessor)
Deneyap Pressure Measurer (TE Connectivity MS5637-02BA03)
Deneyap 6-Axis Inertial Measurement Unit (ST Microelectronics LSM6DSM)
Deneyap Ultraviolet Light Detector (Lite-On LTR-390UV-01)
*/

// Arduino IDE Code

#include <Deneyap_6EksenAtaletselOlcumBirimi.h>       
#include <Deneyap_GPSveGLONASSkonumBelirleyici.h>    
#include <Deneyap_BasincOlcer.h>                     
#include <Deneyap_UltraviyoleIsikAlgilici.h>           
#include <DeneyapTelegram.h>                         
#include <WiFi.h>
#include <WiFiClientSecure.h>

LSM6DSM AccGyro;                                     
GPS GPS;                                             
AtmosphericPressure BaroSensor;                     
UVlight UVlight;

// WiFi ağınızın adını giriniz. (Enter your WiFi SSID here.)
const char* ssid = "YOUR_WIFI_SSID";  

// WiFi şifrenizi giriniz. (Enter your WiFi password here.)
const char* password = "YOUR_WIFI_PASSWORD";  

// Telegram bot tokeninizi giriniz. (Enter your Telegram bot token here.)
#define BOTtoken "YOUR_BOT_TOKEN"  

// Telegram chat kimliğinizi giriniz. (Enter your Telegram chat ID here.)
#define CHAT_ID "YOUR_CHAT_ID"  

DeneyapTelegram telegram;
WiFiClientSecure client;

void setup() {
    Serial.begin(115200);  
    delay(1000);
    
    AccGyro.begin(0x6B);  
    
    if (!GPS.begin(0x2F)) {
        Serial.println("GPS başlatılamadı! (GPS could not be initialized!)");
        while (1);
    }

    BaroSensor.begin(0x76);

    UVlight.begin(0x53);                            
    UVlight.setMode(LTR390_MODE_ALS);      

    client.setInsecure();
    Serial.print("WiFi'ye bağlanılıyor: (Connecting to WiFi:)");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("WiFi'ye bağlandı. (WiFi connected.)");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    telegram.begin(BOTtoken);
    telegram.send(CHAT_ID, "Cihaz bağlandı. (Device is Connected.)");

}

void loop() {
   
    float accelX = AccGyro.readFloatAccelX();
    float accelY = AccGyro.readFloatAccelY();
    float accelZ = AccGyro.readFloatAccelZ();

    float gyroX = AccGyro.readFloatGyroX();
    float gyroY = AccGyro.readFloatGyroY();
    float gyroZ = AccGyro.readFloatGyroZ();

    float tempC = AccGyro.readTempC();
    float tempF = AccGyro.readTempF();

    float lat = GPS.readLocationLat();
    float lng = GPS.readLocationLng();
  
    float pressure = BaroSensor.getPressure();
    float baroTemp = BaroSensor.getTemp();

    float lux = UVlight.getLUX();
    float als = UVlight.readALS();
    
    float altitude = 44330 * (1.0 - pow(pressure / 1013.25, 0.1903));

    String locationMessage = "Location: " + String(lat, 7) + ", " + String(lng, 7);
    telegram.send(CHAT_ID, locationMessage);

    String accelMessage = "Accel (X,Y,Z): " + String(accelX) + ", " + String(accelY) + ", " + String(accelZ);
    telegram.send(CHAT_ID, accelMessage);

    String gyroMessage = "Gyro (X,Y,Z): " + String(gyroX) + ", " + String(gyroY) + ", " + String(gyroZ);
    telegram.send(CHAT_ID, gyroMessage);

    String tempMessage = "Temperature: " + String(tempC) + " °C, " + String(tempF) + " °F";
    telegram.send(CHAT_ID, tempMessage);

    String pressureMessage = "Pressure: " + String(pressure) + " hPa";
    telegram.send(CHAT_ID, pressureMessage);

    String altitudeMessage = "Altitude: " + String(altitude) + " meters";
    telegram.send(CHAT_ID, altitudeMessage);

    String luxMessage = "UV Light (LUX): " + String(lux);
    telegram.send(CHAT_ID, luxMessage);

    String alsMessage = "UV Light (ALS): " + String(als);
    telegram.send(CHAT_ID, alsMessage);

    delay(5000);
    
}
