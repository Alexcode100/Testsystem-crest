#include <WiFi.h>
#include <HTTPClient.h>
#include "database.h"

void sendData(String capturedIMEI, String capturedICCID, String capturedFirmware, String capturedVbatt, 
              String capturedRTC_con, String capturedFlash_con, String capturedNetwork, String capturedRevision, 
              String capturedIMSI, String capturedRTC_sync, String capturedSHT20, String capturedVB86, 
              String capturedKlay, String deviceType) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Gebruik de correcte URL
    http.begin("http://192.168.50.188/inserttestdata.php");

    // Specificeer de content type en payload
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "imei=" + capturedIMEI + "&iccid=" + capturedICCID + "&firmware=" + capturedFirmware + 
                             "&vbatt=" + capturedVbatt + "&rtc_con=" + capturedRTC_con + "&flash_con=" + capturedFlash_con + 
                             "&network=" + capturedNetwork + "&revision=" + capturedRevision + "&imsi=" + capturedIMSI + 
                             "&rtc_sync=" + capturedRTC_sync + "&sht20=" + capturedSHT20 + "&vb86=" + capturedVB86 + 
                             "&klay=" + capturedKlay + "&deviceType=" + deviceType;

    // Maak een POST verzoek
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Fout bij verzenden POST: ");
      Serial.println(httpResponseCode);
    }

    // Beëindig de verbinding
    http.end();
  }
}
