// Wiring
/*
The following Deneyap modules were connected in a daisy-chain configuration to Deneyap Kart 1A via the I2C connector:

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

const char* ssid = "YOUR_WIFI_SSID";  // Enter your WiFi SSID here.
const char* password = "YOUR_WIFI_PASSWORD";  // Enter your WiFi password here.
#define BOTtoken "YOUR_BOT_TOKEN"  // Enter your Telegram bot token here.
#define CHAT_ID "YOUR_CHAT_ID"  // Enter your Telegram chat ID here.

DeneyapTelegram telegram;
WiFiClientSecure client;

void setup() {
    Serial.begin(115200);  
    delay(1000);
    
    // Initialize Accelerometer and Gyro 
    AccGyro.begin(0x6B);  
    
    // Initialize GPS
    if (!GPS.begin(0x2F)) {
        Serial.println("ERROR: GPS could not be initialized!");
        while (1);
    }

    // Initialize Barometer 
    BaroSensor.begin(0x76);

    // Initialize UV sensor 
    UVlight.begin(0x53);                            
    UVlight.setMode(LTR390_MODE_ALS);      

    // Connect to WiFi
    client.setInsecure();
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize Telegram bot
    telegram.begin(BOTtoken);
    telegram.send(CHAT_ID, "Device is Connected ");

    Serial.println("All sensors initialized successfully!");
}

void loop() {
    // Read accelerometer data
    float accelX = AccGyro.readFloatAccelX();
    float accelY = AccGyro.readFloatAccelY();
    float accelZ = AccGyro.readFloatAccelZ();

    // Read gyro data
    float gyroX = AccGyro.readFloatGyroX();
    float gyroY = AccGyro.readFloatGyroY();
    float gyroZ = AccGyro.readFloatGyroZ();

    // Read temperature data
    float tempC = AccGyro.readTempC();
    float tempF = AccGyro.readTempF();

    // Read GPS data
    float lat = GPS.readLocationLat();
    float lng = GPS.readLocationLng();
    int day = GPS.readDay();
    int month = GPS.readMonth();
    int year = GPS.readYear();
    int hour = GPS.readHour();
    int minute = GPS.readMinute();
    int second = GPS.readSecond();

    // Read pressure and temperature data
    float pressure = BaroSensor.getPressure();
    float baroTemp = BaroSensor.getTemp();

    // Read ambient light data
    float lux = UVlight.getLUX();
    float als = UVlight.readALS();
    
    // Calculate altitude (using pressure data in hPa)
    float altitude = 44330 * (1.0 - pow(pressure / 1013.25, 0.1903));

    // Print data to Serial Monitor
    Serial.print("Location: ");
    Serial.print(lat, 7); Serial.print(", ");
    Serial.println(lng, 7);
    
    Serial.print("Date: ");
    Serial.print(day); Serial.print("/");
    Serial.print(month); Serial.print("/");
    Serial.println(year);
    
    Serial.print("Time: ");
    Serial.print(hour); Serial.print(":");
    Serial.print(minute); Serial.print(":");
    Serial.println(second);

    Serial.print("Accelerometer (X,Y,Z): ");
    Serial.print(accelX); Serial.print(", ");
    Serial.print(accelY); Serial.print(", ");
    Serial.println(accelZ);

    Serial.print("Gyro (X,Y,Z): ");
    Serial.print(gyroX); Serial.print(", ");
    Serial.print(gyroY); Serial.print(", ");
    Serial.println(gyroZ);

    Serial.print("Temperature: ");
    Serial.print(tempC); Serial.print(" °C, ");
    Serial.print(tempF); Serial.println(" °F");

    Serial.print("Pressure (hPa): ");
    Serial.println(pressure);
    Serial.print("Pressure Sensor Temperature: ");
    Serial.println(baroTemp);

    Serial.print("Altitude: ");
    Serial.print(altitude); 
    Serial.println(" meters");

    Serial.print("LUX: ");
    Serial.print(UVlight.getLUX());
    Serial.print("\tALS: ");
    Serial.println(UVlight.readALS());

    // Send data to Telegram

    
    String locationMessage = "Location: " + String(lat, 7) + ", " + String(lng, 7);
    telegram.send(CHAT_ID, locationMessage);


    String dateMessage = "Date: " + String(day) + "/" + String(month) + "/" + String(year);
    telegram.send(CHAT_ID, dateMessage);


    String timeMessage = "Time: " + String(hour) + ":" + String(minute) + ":" + String(second);
    telegram.send(CHAT_ID, timeMessage);


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