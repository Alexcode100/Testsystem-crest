//Test
// Libraries
#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include "HardwareSerial.h"
#include "Adafruit_Thermal.h"
#include "timeObj.h"
#include "blinker.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "HTTPClient.h"
#include "database.h"

// Zoektermen
// Init
String Firmware;
String Vbatt;
String RTC_con;
String Flash_con;
String Network;
String Revision;
String IMEI;
String IMSI;
String ICCID;
String RTC;
String RTC_sync;
String Sleep;
// Metingen
String SHT20;
String VB86;
String Klay;

// Arrays voor opslag van binnenkomende data
const byte numChars = 200;
char receivedChars[numChars];   
const byte numChars2 = 200;
char receivedChars2[numChars2];   

// Tijdelijke opslag
String RTU_String;
String UPR_String;
String Current;
char Current_buf[40];
char Firmware_buf[40];
char Vbatt_buf[40];
char RTC_con_buf[40];
char Flash_con_buf[40];
char Network_buf[40];
char Revision_buf[40];
char IMEI_buf[40];
char IMSI_buf[40];
char ICCID_buf[40];
char RTC_buf[40];
char RTC_sync_buf[40];
char SHT20_buf[40];
char VB86_buf[40];
char Klay_buf[40];
boolean newData = false;
boolean newData2 = false;

// Rx- en Tx-pins
#define RX_RTU 43
#define TX_RTU 44

#define RX_BON 18
#define TX_BON 17

// USB-OTG
USBHIDKeyboard Keyboard;

// Bonnenprinter
Adafruit_Thermal printer(&Serial2); // UART-poort waarop bonnenprinter aangesloten is

// Start- en Stopknop
const int StartKnop = 21;
const int StopKnop = 47;
int vorigeStartKnopStatus = HIGH;
int vorigeStopKnopStatus = HIGH;

// Timer
timeObj Timer1Min;
timeObj Timer1Min2;
timeObj TestsuiteChecker;
timeObj FirmwareChecker;
timeObj Timer5Min;
timeObj EraseTimer;
timeObj FirstErase;

// 24V Relais Stoplicht
const int SL_ROOD = 38;
const int SL_ORANJE = 39;
const int SL_GROEN = 40;
const int SL_BUZZER = 41;

#define BLINK_INTERVAL  1000  // Ms
#define BLINK_AAN     500   // ms
blinker Rood(SL_ROOD, BLINK_AAN, BLINK_INTERVAL);
blinker Oranje(SL_ORANJE, BLINK_AAN, BLINK_INTERVAL);
blinker Groen(SL_GROEN, BLINK_AAN, BLINK_INTERVAL);
blinker Buzzer(SL_BUZZER, BLINK_AAN, BLINK_INTERVAL);

// 5V Relais
#define RELAY_NO    false // True = NO, False = NC
#define NUM_RELAYS  15 // Aantal relais
const int relayGPIOs[NUM_RELAYS] = {14, 13, 12, 11, 10, 9, 8, 2, 1, 16, 15, 7, 6, 5, 4}; // Gebruikte GPIO voor relais
const int Programmer[] = {11, 10, 9, 8, 2, 1};
const int Printer = 14;
const int RTU_0ohm = 12;
const int RTU_9ohm = 13;

// WiFi-gegevens
const char* ssid = "Protoworkz 2.4 GHz";
const char* password = "marine_51a";
//const char* ssid = "Galaxy Note10f9e4";
//const char* password = "Patrick01";
//const char* ssid = "HOME";
//const char* password = "#6Lisbr@m40";
//const char* ssid = "KPNBF3C06";
//const char* password = "adLeR10+75kAk";

//Server URL
const char* serverName = "http://192.168.50.188/inserttestdata.php";

// Inputs
const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

// Overig
bool SPITS_EMERGENCY = false;
bool RTU_Data = false;
bool Testsuite_Success = false;
int TestsuiteFlagArray[12] = {0};
String TestsuiteFlagArrayOK = "111111111111";
String Tests;
bool Install_Firmware_Command = false;
bool Install_Testsuite_Command = false;
int UPR_NUM;
bool UPR_CHECK = false;
bool Curr_Testsuite = false;
bool Curr_Firmware = false;
bool ProgrammerToggled = false;
bool ChecklistLoader = true;
bool TestsuiteInstall = false;
bool SPITS_Ready = true;
bool NetworkFilter = false;
bool Prog_VB86 = false;
bool Prog_Klay = false;
bool eBuzzer = false;

unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;
AsyncEventSource events("/events");

// Checklist
String Timer1Min_Check = "Device opladen 9Ω 1min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String Timer1Min2_Check = "Device opladen 0Ω 1min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String Meting1_Check = "Stroomverbruik < 100mA:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String TestsuiteControle_Check = "Testsuite programmeren:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String TestsuiteComplete_Check = "Testsuite voltooid:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String FirmwareData_Check = "Firmware programmeren:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String Timer5Min_Check = "Device sleep 5min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String Meting2_Check = "Stroomverbruik < 100μA:&nbsp&nbsp&nbsp&nbsp&thinsp;";
String SPITS_Status;


// Open AsyncWebServer op poort 80
AsyncWebServer server(80);

// Opbouw webpagina
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 4.0rem; line-height: 6px;}
    h10 {font-size: 3.0rem; margin: 0.1px;}
    h11 {line-height: 2px; margin: 0.1px;}
    h12 {line-height: 2px; font-size: 4.0rem; margin: 0.1px;}
    p {font-size: 3.0rem;}
    body {max-width: 1920px; margin:0px auto; padding-bottom: 25px; background-color:#d9d9d9; transform: scale(0.8); transform-origin: top;}

    .grid-container {
      display: grid;
      grid-template-rows: repeat(6, 40px); // 5 kolommen
      grid-template-columns: repeat(5, 120px);
      grid-auto-flow: column;
      grid-gap: 50px 70px; // Ruimte tussen objecten
      text-align: center;
    }

    .grid-container2 {
      display: grid;
      grid-template-rows: repeat(3, 40px); // 5 kolommen
      grid-template-columns: repeat(2);
      grid-auto-flow: column;
      grid-gap: 100px 0px; // Ruimte tussen objecten
      text-align: center;
      font-size: 18px;
    }

    .switch {
      position: relative;
      display: inline-block;
      width: 120px;
      height: 68px;
    }

    .switch input {
      display: none;
    }

    .slider {
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      border-radius: 34px;
    }

    .slider:before {
      position: absolute;
      content: "";
      height: 52px;
      width: 52px;
      left: 8px;
      bottom: 8px;
      background-color: #474d4d;
      transition: .4s;
      border-radius: 68px;
    }

    input:checked + .slider {
      background-color: #ffd101;
    }

    input:checked + .slider:before {
      transform: translateX(52px);
    }

    .button {
      display: inline-block;
      padding: 10px 20px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #474d4d;
      background-color: #ffd101;
      border: none;
      margin: 10px auto;
      border-radius: 5px;
      box-shadow: 0 6px #999;
      cursor: pointer;
      user-select: none;
    }

    .button:hover {background-color: #cca701}
    .button:active {
      background-color: #969696;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }

    .startbutton {
      display: inline-block;
      padding: 10px 20px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #474d4d;
      background-color: #32de2f;
      border: none;
      margin: 10px auto;
      border-radius: 5px;
      box-shadow: 0 6px #999;
      cursor: pointer;
      user-select: none;
    }

    .startbutton:hover {background-color: #239c21}
    .startbutton:active {
      background-color: #969696;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }

    .stopbutton {
      display: inline-block;
      padding: 10px 20px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #050505;
      background-color: #ed2121;
      border: none;
      margin: 10px auto;
      border-radius: 5px;
      box-shadow: 0 6px #999;
      cursor: pointer;
      user-select: none;
    }  
    
    .stopbutton:hover {background-color: #9c2921}
    .stopbutton:active {
      background-color: #969696;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }

    .dropbtn {
      display: block;
      width: 128px;
      background-color: #ffd101;
      color: #474d4d;
      padding: 10px 20px;
      font-size: 24px;
      border: none;
      margin: 10px auto;
      position: relative;
      cursor:pointer;
      border-radius: 5px;
      box-shadow: 0 6px #999;
    }

    .dropbtn:hover {background-color: #cca701}
    .dropbtn:active {
      background-color: #969696;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }
    
    .dropdown {
      position: relative;
      display: block;
      padding: 2px 16px;
      margin: 10px auto;
      padding-top: 20px;
    }
    
    .dropdown-content {
      display: none;
      position: relative;
      background-color: #f1f1f1;
      width: 1100px;
      height: 550px;
      z-index: 2;
      box-shadow: 0px 8px 16px 0px rgba(0, 0, 0, 0.2);
      
      padding: 2px 16px;
      border-radius: 15px;
      text-align: center;
    }
    
    .dropdown-content a {
      color: black;
      padding: 12px 16px;
      text-decoration: none;
      display: block;
    }
    
    .dropdown-content a:hover {
      background-color: #ddd;
    }
    
    .dropcheck {
        position: absolute;
        left: -9999px;
    }
    
    .dropcheck:checked ~ .dropdown-content {
      display: inline-block;
    }
    
    .dropcheck:checked + .dropbtn {
      background-color: #cca701;
    }

    .dropbtn2 {
      display: block;
      width: 128px;
      background-color: #ffd101;
      color: #474d4d;
      padding: 10px 20px;
      font-size: 24px;
      border: none;
      margin: 10px auto;
      position: relative;
      cursor:pointer;
      border-radius: 5px;
      box-shadow: 0 6px #999;
    }

    .dropbtn2:hover {background-color: #cca701}
    .dropbtn2:active {
      background-color: #969696;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }
    
    .dropdown2 {
      position: relative;
      display: block;
      padding: 2px 16px;
      margin: 10px auto;
    }
    
    .dropdown-content2 {
      display: none;
      position: relative;
      background-color: #f1f1f1;
      width: 400px;
      height: 430px;
      z-index: 1;
      box-shadow: 0px 8px 16px 0px rgba(0, 0, 0, 0.2);
      margin: 10px auto;
      border-radius: 15px;
    }
    
    .dropdown-content a2 {
      color: black;
      padding: 12px 16px;
      text-decoration: none;
      display: block;
    }
    
    .dropdown-content a2:hover {
      background-color: #ddd;
    }
    
    .dropcheck2 {
        position: absolute;
        left: -9999px;
    }
    
    .dropcheck2:checked ~ .dropdown-content2 {
      display: inline-block;
    }
    
    .dropcheck2:checked + .dropbtn2 {
      background-color: #cca701;
    }
    
    .card {
      background-color: white;
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      max-width: 800px;
      display: inline-block;
      padding: 10px 16px;
      padding-bottom: 20px;
      border-radius: 15px;
    }
    
    .card-text {
      text-align: right;
    }
    
    .reading {
      font-size: 24px;
    }

    .spst {
      font-size: 40px;
      text-align: center;
    }
    
  </style>
</head>
<body>
  <h2>S.P.I.T.S.</h2>
  <div class="content">
    <div class="card">
      <h10 class="card-title"><b>VOORTGANG</b></h10>
      <h11 class="card-text"><b><p><span class="reading"><span id="T1M">%TIMER1M%</span></p>
      <p><span class="reading"><span id="T1M2">%TIMER1M2%</span></p>
      <p><span class="reading"><span id="M1">%METING1%</span></p>
      <p><span class="reading"><span id="TCTRL">%TESTSUITECONTROLE%</span></p>
      <p><span class="reading"><span id="TCMPL">%TESTSUITECOMPLETE%</span></p>
      <p><span class="reading"><span id="FD">%FIRMWAREDATA%</span></p>
      <p><span class="reading"><span id="T5M">%TIMER5M%</span></p>
      <p><span class="reading"><span id="M2">%METING2%</span></p></b></h11>
      <h12><br><span class="spst"><span id="STS"><b>%STATUS%</b></span></h12>
    </div>
  </div>
  <div class="dropdown">
    <input id="dropcheck" class="dropcheck" type="checkbox">
    <label for="dropcheck" class="dropbtn">Relais</label>
    <div class="dropdown-content">
      <div class="grid-container">
        %BUTTONPLACEHOLDER%
      </div>
    </div>
  </div>
  <div class="dropdown2">
    <input id="dropcheck2" class="dropcheck2" type="checkbox">
    <label for="dropcheck2" class="dropbtn2">Programma</label>
    <div class="dropdown-content2">
      <div class = "grid-container2">
        <div class="grid-item">
          %PROGRAMMER%
        </div>
        <div class="grid-item">
          %VB86%
        </div>
        <div class="grid-item">
          %KLAY%
        </div>
        <div class="grid-item">
          <h4>Programma 4</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="switch4"><span class="slider"></span></label>
        </div>
        <div class="grid-item">
          <h4>Programma 5</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="switch5"><span class="slider"></span></label>
        </div>
        <div class="grid-item">
          <h4>Programma 6</h4><label class="switch"><input type="checkbox" onchange="toggleCheckbox(this)" id="switch6"><span class="slider"></span></label>
        </div>
      </div>
    </div>
  </div>
  <h5><button class="startbutton" onclick="Start_SPITS('start');">Start S.P.I.T.S.</button>
  <button class="stopbutton" onclick="Stop_SPITS('stop');">Stop S.P.I.T.S.</button></h5>
  
  <h6><button class="button" onclick="UPR_get('UPR_get');">Request U.P.R.</button>
  <button class="button" onclick="UPR_reset('UPR_reset');">Reset U.P.R.</button>
  <button class="button" onclick="printknop('bon');">PRINT</button>
  <button class="button" onclick="Erase_RTU('erase');">Erase R.T.U.</button></h6>
  <br><br><br><br><br><br><br>
<script>
  if (!!window.EventSource) {
   var source = new EventSource('/events');
   
   source.addEventListener('open', function(e) {
    console.log("Events Connected");
   }, false);
   source.addEventListener('error', function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      console.log("Events Disconnected");
    }
   }, false);
   
   source.addEventListener('message', function(e) {
    console.log("message", e.data);
   }, false);
   
   source.addEventListener('Timer1Min_Check', function(e) {
    console.log("Timer1Min_Check", e.data);
    document.getElementById("T1M").innerHTML = e.data;
   }, false);

   source.addEventListener('Timer1Min2_Check', function(e) {
    console.log("Timer1Min2_Check", e.data);
    document.getElementById("T1M2").innerHTML = e.data;
   }, false);

   source.addEventListener('Meting1_Check', function(e) {
    console.log("Meting1_Check", e.data);
    document.getElementById("M1").innerHTML = e.data;
   }, false);

   source.addEventListener('TestsuiteControle_Check', function(e) {
    console.log("TestsuiteControle_Check", e.data);
    document.getElementById("TCTRL").innerHTML = e.data;
   }, false);

   source.addEventListener('TestsuiteComplete_Check', function(e) {
    console.log("TestsuiteComplete_Check", e.data);
    document.getElementById("TCMPL").innerHTML = e.data;
   }, false);

   source.addEventListener('FirmwareData_Check', function(e) {
    console.log("FirmwareData_Check", e.data);
    document.getElementById("FD").innerHTML = e.data;
   }, false);

   source.addEventListener('Timer5Min_Check', function(e) {
    console.log("Timer5Min_Check", e.data);
    document.getElementById("T5M").innerHTML = e.data;
   }, false);

   source.addEventListener('Meting2_Check', function(e) {
    console.log("Meting2_Check", e.data);
    document.getElementById("M2").innerHTML = e.data;
   }, false);

   source.addEventListener('SPITS_Status', function(e) {
    console.log("SPITS_Status", e.data);
    document.getElementById("STS").innerHTML = e.data;
   }, false);

//   source.addEventListener('RTU_String', function(e) {
//    console.log("RTU_String", e.data);
//    document.getElementById("temp").innerHTML = e.data;
//   }, false);
  }

  function toggleCheckbox(element) {
    var xhr = new XMLHttpRequest();
    if (element.checked) {
      xhr.open("GET", "/update?relay=" + element.id + "&state=1", true);
    } else {
      xhr.open("GET", "/update?relay=" + element.id + "&state=0", true);
    }
    xhr.send();
  }
  function printknop(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
  }
  function UPR_get(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
  }
  function UPR_reset(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
  }
  function Erase_RTU(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
  }
  function toggleCheckbox1(element) {
    var xhr = new XMLHttpRequest();
    if (element.checked) {
      xhr.open("GET", "/programmer?state=1", true);
    }
    else {
      xhr.open("GET", "/programmer?state=0", true);
    }
    xhr.send();
  }
  function toggleCheckbox2(element) {
    var xhr = new XMLHttpRequest();
    if (element.checked) {
      xhr.open("GET", "/vb86?state=1", true);
    }
    else {
      xhr.open("GET", "/vb86?state=0", true);
    }
    xhr.send();
  }
  function toggleCheckbox3(element) {
    var xhr = new XMLHttpRequest();
    if (element.checked) {
      xhr.open("GET", "/klay?state=1", true);
    }
    else {
      xhr.open("GET", "/klay?state=0", true);
    }
    xhr.send();
  }
  function Start_SPITS(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
  }
  function Stop_SPITS(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
  }
</script>
</body>
</html>
)rawliteral";

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}

String Prog_Toggle(int i){
  for (i=0; i<6; i++) {
    if(digitalRead(Programmer[i])){
      return "checked";
    }
    else {
      return "";
    }
  }
}

String VB86_Toggle(int i){
  for (i=1; i<=1; i++) {
    if(Prog_VB86 == true){
      return "checked";
    }
    else {
      return "";
    }
  }
}

String Klay_Toggle(int i){
  for (i=1; i<=1; i++) {
    if(Prog_Klay == true){
      return "checked";
    }
    else {
      return "";
    }
  }
}

// Vervangt placeholder met knoppen
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    for(int i=1; i<=NUM_RELAYS; i++){
      String relayStateValue = relayState(i);
      buttons+= "<h3><br><br>Relais " + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    }
    return buttons;
  }

  else if(var == "PROGRAMMER"){
    String prog ="";
    for(int i=1; i<=1; i++){
      String ProgStateValue = Prog_Toggle(i);
      prog= "<h4>Programmer</h4><label class=\"switch\"><input type=\"checkbox\" id=\"" + String(i) + "\" "+ ProgStateValue +" onchange=\"toggleCheckbox1(this)\"><span class=\"slider\"></span></label>";
    }
    return prog;
  }

  else if(var == "VB86"){
    String vb ="";
    for(int i=1; i<=1; i++){
      String VegabarStateValue = VB86_Toggle(i);
      vb= "<h4>VegaBar86</h4><label class=\"switch\"><input type=\"checkbox\" id=\"" + String(i) + "\" "+ VegabarStateValue +" onchange=\"toggleCheckbox2(this)\"><span class=\"slider\"></span></label>";
    }
    return vb;
  }

  else if(var == "KLAY"){
    String klay ="";
    for(int i=1; i<=1; i++){
      String KlayStateValue = Klay_Toggle(i);
      klay= "<h4>Klay</h4><label class=\"switch\"><input type=\"checkbox\" id=\"" + String(i) + "\" "+ KlayStateValue +" onchange=\"toggleCheckbox3(this)\"><span class=\"slider\"></span></label>";
    }
    return klay;
  }
  return String();
}


void setup() {
  Serial.begin(115200); // Serial Monitor
  Serial1.begin(4800, SERIAL_8N1, RX_RTU, TX_RTU); // RTU
  Serial2.begin(19200, SERIAL_8N1, RX_BON, TX_BON); // Bonnenprinter

  pinMode(StartKnop, INPUT);
  pinMode(StopKnop, INPUT);

  pinMode(SL_ROOD, OUTPUT);
  pinMode(SL_ORANJE, OUTPUT);
  pinMode(SL_GROEN, OUTPUT);
  pinMode(SL_BUZZER, OUTPUT);

  pinMode(Printer, OUTPUT);
  digitalWrite(Printer, LOW);

  pinMode(RTU_0ohm, OUTPUT);
  pinMode(RTU_9ohm, OUTPUT);

  digitalWrite(SL_ROOD, LOW);
  digitalWrite(SL_ORANJE, LOW);
  digitalWrite(SL_GROEN, LOW);
  digitalWrite(SL_BUZZER, LOW);

  Keyboard.begin();
  USB.begin();

  Timer1Min.setTime(1000);
  Timer1Min2.setTime(1000);
  TestsuiteChecker.setTime(60000);
  FirmwareChecker.setTime(20000);
  Timer5Min.setTime(270000);
  EraseTimer.setTime(20000);
  FirstErase.setTime(20000);

  Timer1Min.reset();
  Timer1Min2.reset();
  TestsuiteChecker.reset();
  FirmwareChecker.reset();
  Timer5Min.reset();
  EraseTimer.reset();
  FirstErase.reset();

  printer.begin(255);
  printer.justify('C'); // Midden
  printer.boldOn();
  printer.boldOff();
  printer.justify('L'); // Links
  printer.setTimes(5000, 2100);

  for (int i=0; i<6; i++)
  {
    pinMode(Programmer[i], OUTPUT);
  }

  for(int i=1; i<=NUM_RELAYS; i++){
    pinMode(relayGPIOs[i-1], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i-1], HIGH);
    }
    else{
      digitalWrite(relayGPIOs[i-1], LOW);
    }
  }

  digitalWrite(RTU_0ohm, LOW); // Relais zonder 9ohm uitschakelen
  digitalWrite(RTU_9ohm, HIGH); // Relais met 9ohm inschakelen
  
  // Verbind met Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("S.P.I.T.S. WORDT GESTART");
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Lokaal IP-Adres
  Serial.println(WiFi.localIP());

  // Index webpagina
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/UPR_get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial2.println("UPR_get");
    request->send(200, "text/plain", "ok");
  });

  server.on("/UPR_reset", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial2.println("UPR_reset");
    request->send(200, "text/plain", "ok");
  });

  server.on("/bon", HTTP_GET, [] (AsyncWebServerRequest *request) {
    printer.println(RTC_buf);
    printer.feed(1);
    printer.print("Firmware: ");
    printer.println(Firmware_buf);
    printer.print("Vbatt: ");
    printer.println(Vbatt_buf);
    printer.print("RTC_con: ");
    printer.println(RTC_con_buf);
    printer.print("Flash_con: ");
    printer.println(Flash_con_buf);
    printer.print("Network: ");
    printer.println(Network_buf);
    printer.print("Tel. Rev.: ");
    printer.println(Revision_buf);
    printer.print("IMSI: ");
    printer.println(IMSI_buf);
    printer.print("RTC_sync: ");
    printer.println(RTC_sync_buf);
    printer.print("SHT20: ");
    printer.println(SHT20_buf);
    printer.justify('C');
    printer.feed(1);
    printer.println("IMEI: ");
    printer.printBarcode(IMEI_buf, CODE128); // Print IMEI
    printer.feed(1);
    printer.println("ICCID: ");
    printer.printBarcode(ICCID_buf, CODE128); // Print ICCID
    printer.feed(1);
    if (Prog_VB86 == true) {
      printer.println("VB86: ");
      printer.printBarcode(VB86_buf, CODE128); // Print Vegabar
    }
    if (Prog_Klay == true) {
      printer.println("Klay: ");
      printer.printBarcode(Klay_buf, CODE128); // Print Klay
    }
    printer.justify('L');
    printer.feed(3);
    request->send(200, "text/plain", "ok");
  });

  server.on("/erase", HTTP_GET, [] (AsyncWebServerRequest *request) {
    for (int i=0; i<6; i++) {
      digitalWrite(Programmer[i], HIGH);
    }
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_UP_ARROW);
    delay(100);
    Keyboard.releaseAll();
    EraseTimer.start();
    request->send(200, "text/plain", "ok");
  });

  server.on("/start", HTTP_GET, [] (AsyncWebServerRequest *request) {
    SPITS_Ready = false;
    Serial.println("STARTKNOP");
    digitalWrite(SL_ORANJE, HIGH);
    digitalWrite(Printer, LOW); // Bonnenprinter uitschakelen
    Install_Testsuite_Command = true;
    request->send(200, "text/plain", "ok");
  });

  server.on("/stop", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("STOPKNOP");
    Timer1Min_Check = "Device opladen 9Ω 1min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Timer1Min2_Check = "Device opladen 0Ω 1min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Meting1_Check = "Stroomverbruik < 100mA:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    TestsuiteControle_Check = "Testsuite programmeren:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    TestsuiteComplete_Check = "Testsuite voltooid:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    FirmwareData_Check = "Firmware programmeren:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Timer5Min_Check = "Device sleep 5min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Meting2_Check = "Stroomverbruik < 100μA:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    SPITS_EMERGENCY = false;
    for(int i=1; i<=NUM_RELAYS; i++){
      digitalWrite(relayGPIOs[i-1], LOW);
    }
    digitalWrite(SL_ROOD, LOW);
    digitalWrite(SL_ORANJE, LOW);
    digitalWrite(SL_BUZZER, LOW);
    digitalWrite(SL_GROEN, LOW);
    Buzzer.setOnOff(false);
    Timer1Min.reset();
    Timer1Min2.reset();
    TestsuiteChecker.reset();
    FirmwareChecker.reset();
    Timer5Min.reset();
    EraseTimer.reset();
    FirstErase.reset();
    digitalWrite(RTU_9ohm, HIGH);
    SPITS_Ready = true;
    request->send(200, "text/plain", "ok");
  });

  // Stuur een GET request naar <ESP_IP>/update?relay=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
    // GET input1 waarde van <ESP_IP>/update?relay=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
      Serial.print("RELAIS: ");
      digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.print(inputMessage);
    Serial.print(" STAND: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  server.on("/programmer", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      for (int i=0; i<6; i++) {
        digitalWrite(Programmer[i], inputMessage.toInt());
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/vb86", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      Prog_VB86 = !Prog_VB86;
      Serial.print("Prog_VB86: ");
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/klay", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      Prog_Klay = !Prog_Klay;
      Serial.print("Prog_Klay: ");
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin(); // Start server
}
  

void Serial_Read() { // Hier wordt de RTU-output uitgelezen
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial1.available() > 0 && newData == false) {
    rc = Serial1.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // String terminator
      ndx = 0;
      newData = true;
      RTU_Data = true;
    }
  }
}

void Serial2_Read() { // Hier wordt de RTU-output uitgelezen
  static byte ndx2 = 0;
  char endMarker2 = '\n';
  char rc2;
  

  while (Serial2.available() > 0 && newData2 == false) {
    //Serial.println("NIEUWE DATA UPR NEE");
    rc2 = Serial2.read();

    if (rc2 != endMarker2) {
      receivedChars2[ndx2] = rc2;
      ndx2++;
      if (ndx2 >= numChars2) {
        ndx2 = numChars2 - 1;
      }
    }
    else {
      receivedChars2[ndx2] = '\0'; // String terminator
      ndx2 = 0;
      newData2 = true;
      //Serial.println("NIEUWE DATA UPR");
    }
  }
}


void RTU_scan() { // Hier wordt de RTU-output gescand op zoektermen
  if (newData == true) {
    Serial.println(receivedChars); // Print RTU-output naar monitor
    RTU_String = String(receivedChars); // Tijdelijke string waarin gezocht wordt naar een zoekterm

    // Substrings voor alle zoektermen
    Firmware = RTU_String.substring(1, 9);
    Vbatt = RTU_String.substring(11, 16);
    RTC_con = RTU_String.substring(11, 28);
    Flash_con = RTU_String.substring(11, 28);
    Network = RTU_String.substring(11, 28);
    Revision = RTU_String.substring(1, 9);
    IMEI = RTU_String.substring(1, 5);
    IMSI = RTU_String.substring(1, 5);
    ICCID = RTU_String.substring(1, 6);
    RTC = RTU_String.substring(1, 10);
    RTC_sync = RTU_String.substring(11, 28);
    SHT20 = RTU_String.substring(11, 26);
    VB86 = RTU_String.substring(11, 25);
    Klay = RTU_String.substring(11, 25);
    Sleep = RTU_String.substring(1, 6);

    if (Firmware == "EEPROM W") {
      Firmware = RTU_String.substring(10, 15);
      Serial.println("FIRMWARE FOUND!");
//      Serial.println(Firmware);
      Firmware.toCharArray(Firmware_buf, 40);
      TestsuiteFlagArray[0] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (Vbatt == "Vbatt") {
      Vbatt = RTU_String.substring(19, 23);
      Serial.println("VBATT FOUND!");
//      Serial.println(Vbatt);
      Vbatt.toCharArray(Vbatt_buf, 40);
      TestsuiteFlagArray[1] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (RTC_con == "RTC Con : SUCCESS") {
      RTC_con = RTU_String.substring(21, 28);
      Serial.println("RTC_CON FOUND!");
//      Serial.println(RTC_con);
      RTC_con.toCharArray(RTC_con_buf, 40);
      TestsuiteFlagArray[2] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (Flash_con == "FlashCon: SUCCESS") {
      Flash_con = RTU_String.substring(21, 28);
      Serial.println("FLASH_CON FOUND!");
//      Serial.println(Flash_con);
      Flash_con.toCharArray(Flash_con_buf, 40);
      TestsuiteFlagArray[3] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (Network == "Telecom : SUCCESS") {
      Network = RTU_String.substring(21, 32);
      Serial.println("NETWORK FOUND!");
//      Serial.println(Network);
      Network.toCharArray(Network_buf, 40);
      if (NetworkFilter == true) {
        TestsuiteFlagArray[4] = 1;
        Serial.print("Tests: ");
        Serial.println(Tests);
        NetworkFilter = false;
      }
    }
    
    if (Prog_VB86 == true || Prog_Klay == true) {
      if (Revision == "Revision") {
        Revision = RTU_String.substring(11, 40);
        if (Revision == "BC95GVBAR02A03") {
          Revision.toCharArray(Revision_buf, 40);
          Serial.println("REVISION FOUND!");
  //        Serial.println(Revision);
          TestsuiteFlagArray[5] = 1;
          Serial.print("Tests: ");
          Serial.println(Tests);
        }
      }
    }
    
    if (Prog_VB86 == false && Prog_Klay == false && Revision == "Revision") {
      Revision = RTU_String.substring(11, 40);
      Revision.toCharArray(Revision_buf, 40);
      TestsuiteFlagArray[5] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (IMEI == "IMEI") {
      IMEI = RTU_String.substring(8, 23);
      Serial.println("IMEI FOUND!");
//      Serial.println(IMEI);
      IMEI.toCharArray(IMEI_buf, 40); // Tijdelijke opslag zodat IMEI pas na RTC uitgeprint wordt
      TestsuiteFlagArray[6] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (IMSI == "IMSI") {
      IMSI = RTU_String.substring(8, 23);
      Serial.println("IMSI FOUND!");
//      Serial.println(IMSI);
      IMSI.toCharArray(IMSI_buf, 40);
      TestsuiteFlagArray[7] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (ICCID == "ICCID") {
      ICCID = RTU_String.substring(9, 29);
      Serial.println("ICCID FOUND!");
//      Serial.println(ICCID);
      ICCID.toCharArray(ICCID_buf, 40);
      TestsuiteFlagArray[8] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (RTC == "Sync RTC:") {
      RTC = RTU_String.substring(15, 34);
      RTC.toCharArray(RTC_buf, 40);
    }

    if (RTC_sync == "RTC Sync: SUCCESS") {
      RTC_sync = RTU_String.substring(21, 28);
      Serial.println("RTC_SYNC FOUND!");
//      Serial.println(RTC_sync);
      RTC_sync.toCharArray(RTC_sync_buf, 40);
      TestsuiteFlagArray[9] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (SHT20 == "SHT20\t: SUCCESS") {
      SHT20 = RTU_String.substring(19, 26);
      Serial.println("SHT20 FOUND!");
//      Serial.println(SHT20);
      SHT20.toCharArray(SHT20_buf, 40);
      TestsuiteFlagArray[10] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (VB86 == "VB86\t: SUCCESS") {
      VB86 = RTU_String.substring(29, 37);
      Serial.println("VB86 FOUND!");
//      Serial.println(VB86);
      VB86.toCharArray(VB86_buf, 40);
      TestsuiteFlagArray[11] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (Klay == "Klay\t: SUCCESS") {
      Klay = RTU_String.substring(29, 37);
      Serial.println("KLAY FOUND!");
//    Serial.println(Klay);
      Klay.toCharArray(Klay_buf, 40);
      TestsuiteFlagArray[11] = 1;
      Serial.print("Tests: ");
      Serial.println(Tests);
    }

    if (Prog_VB86 == false && Prog_Klay == false) {
      TestsuiteFlagArray[11] = 1;
    }
    
    Tests = "";
    for (int i = 0; i < 12; i++) {
      Tests += String(TestsuiteFlagArray[i]);
    }

    if (Tests.compareTo(TestsuiteFlagArrayOK)== 0) {
      Testsuite_Success = true;
    }
    
    newData = false;
  }
}

void UPR_scan() { // Hier wordt de RTU-output gescand op zoektermen
  if (newData2 == true) {
    Serial.println(receivedChars2); // Print UPR-output naar monitor
    UPR_String = String(receivedChars2); // Tijdelijke string waarin gezocht wordt naar een zoekterm
    Current = UPR_String.substring(0, 4);

    if (Current == "CUR:") {
      UPR_CHECK = true;
      Current = UPR_String.substring(6, 13);
      Current.trim();
      Serial.println("CURRENT FOUND!");
      UPR_NUM = Current.toInt();
    }
    newData2 = false;
  }
}


void Install_Testsuite() {
  if (Install_Testsuite_Command == true) {
    SPITS_Status = "<span style=color:orange;font-weight:bold>TESTING...</span>";
    Timer1Min_Check = "Device opladen 9Ω 1min.:  &#9203";
    events.send("ping",NULL,millis());
    events.send(String(SPITS_Status).c_str(),"SPITS_Status",millis());
    events.send(String(Timer1Min_Check).c_str(),"Timer1Min_Check",millis());
    Serial2.println("UPR_reset"); // Stuur verzoek voor meting naar U.P.R.
    delay(1000);
    Serial2.println("UPR_reset"); // Stuur verzoek voor meting naar U.P.R.
    
    for (int i=0; i<6; i++) {
      digitalWrite(Programmer[i], LOW);
    }
    
    digitalWrite(RTU_0ohm, LOW); // Relais zonder 9ohm uitschakelen
    digitalWrite(RTU_9ohm, HIGH); // Relais met 9ohm inschakelen
    Timer1Min.start(); // Start timer van 1 minuut
    Serial.println("Eerste timer 1 min. gestart");
    Install_Testsuite_Command = false;
  }

  if (Timer1Min.ding()) { // Start wanneer timer afgaat
    Timer1Min_Check = "Device opladen 9Ω 1min.: &#9989";
    Timer1Min2_Check = "Device opladen 0Ω 1min.:  &#9203";
    digitalWrite(RTU_9ohm, LOW); // Relais met 9ohm uitschakelen
    digitalWrite(RTU_0ohm, HIGH); // Relais zonder 9ohm inschakelen
    Timer1Min2.start(); // Start timer van 2 minuten
    Serial.println("Tweede timer 1 min. gestart");
    Timer1Min.reset();
  }

  if (Timer1Min2.ding()) { // Start wanneer timer afgaat
    Timer1Min2_Check = "Device opladen 0Ω 1min.: &#9989";
    Meting1_Check = "Stroomverbruik < 100mA:  &#9203";
    for (int i=0; i<6; i++) {
      digitalWrite(Programmer[i], HIGH);
    }
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_UP_ARROW);
    delay(100);
    Keyboard.releaseAll();
    FirstErase.start();
    Timer1Min2.reset();
  }

  if (FirstErase.ding()) {
    Serial.println("R.T.U. Wiped!");
    
    for (int i=0; i<6; i++) {
      digitalWrite(Programmer[i], LOW);
    }
    
    memset(Firmware_buf, 0, sizeof Firmware_buf);
    memset(Vbatt_buf, 0, sizeof Vbatt_buf);
    memset(RTC_con_buf, 0, sizeof RTC_con_buf);
    memset(Flash_con_buf, 0, sizeof Flash_con_buf);
    memset(Network_buf, 0, sizeof Network_buf);
    memset(Revision_buf, 0, sizeof Revision_buf);
    memset(IMEI_buf, 0, sizeof IMEI_buf);
    memset(IMSI_buf, 0, sizeof IMSI_buf);
    memset(ICCID_buf, 0, sizeof ICCID_buf);
    memset(RTC_buf, 0, sizeof RTC_buf);
    memset(RTC_sync_buf, 0, sizeof RTC_sync_buf);
    memset(SHT20_buf, 0, sizeof SHT20_buf);
    memset(VB86_buf, 0, sizeof VB86_buf);
    memset(Klay_buf, 0, sizeof Klay_buf);
    memset(TestsuiteFlagArray, 0, sizeof TestsuiteFlagArray);
    Tests = "";
    Testsuite_Success = false;
    Serial.println("Buffers wiped.");
    Serial2.println("UPR_get"); // Stuur verzoek voor meting naar U.P.R.
    delay(1000);
    Curr_Testsuite = true;
    FirstErase.reset();
  }

  if (UPR_CHECK == true && Curr_Testsuite == true) {
    long int hndd = 100000; // Honderdduizend micro-ampere (100.000μA)
    if (UPR_NUM <= hndd) {
      Meting1_Check = "Stroomverbruik < 100mA: &#9989";
      TestsuiteControle_Check = "Testsuite programmeren:  &#9203";
      Serial.println("Lager dan 100mA");
      
      for (int i=0; i<6; i++) {
        digitalWrite(Programmer[i], HIGH);
      }
      
      // Roep .bat-file Testsuite aan
      if (Prog_Klay == false) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_HOME);
        delay(100);
        Keyboard.releaseAll();
      }
      else if (Prog_Klay == true) {
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_DOWN_ARROW);
        delay(100);
        Keyboard.releaseAll();
      }
      NetworkFilter = true;
      TestsuiteInstall = true;
      TestsuiteChecker.start();
    }
    if (UPR_NUM > hndd) {
      Serial.println("Hoger dan 100mA");
      Meting1_Check = "Stroomverbruik < 100mA: &#10060";
      SPITS_Status = "<span style=color:#de0404;font-weight:bold>FAILED</span>";
      digitalWrite(RTU_0ohm, LOW); // Relais zonder 9ohm uitschakelen
      digitalWrite(SL_BUZZER, HIGH);
      delay(200);
      digitalWrite(SL_BUZZER, LOW);
      delay(200);
      digitalWrite(SL_BUZZER, HIGH);
      delay(200);
      digitalWrite(SL_BUZZER, LOW);
      eBuzzer = true;
      SPITS_EMERGENCY = true;
    }
    digitalWrite(Printer, HIGH);
    UPR_CHECK = false;
    Curr_Testsuite = false;
  }

  if (TestsuiteInstall == true && RTU_Data == true) {
    TestsuiteControle_Check = "Testsuite programmeren: &#9989";
    TestsuiteComplete_Check = "Testsuite voltooid:  &#9203";
    TestsuiteInstall = false;
  }
  
  if (TestsuiteChecker.ding()) {
    if (Testsuite_Success == true) {
      Serial.println("ALLE FLAGS GEVONDEN!");
      Serial.print(Tests);
      Serial.print(" ");
      Serial.println(TestsuiteFlagArrayOK);
      printer.justify('C');
      printer.println(RTC_buf);
      printer.feed(1);
      printer.println("IMEI: ");
      printer.setTimes(30000, 2100);
      printer.flush();
      printer.printBarcode(IMEI_buf, CODE128); // Print IMEI
      printer.feed(1);
      printer.setTimes(5000, 2100);
      printer.println("ICCID: ");
      printer.setTimes(30000, 2100);
      printer.printBarcode(ICCID_buf, CODE128); // Print ICCID
      printer.setTimes(5000, 2100);
      printer.feed(1);
      if (Prog_VB86 == true) {
        printer.println("VB86: ");
        printer.setTimes(30000, 2100);
        printer.printBarcode(VB86_buf, CODE128); // Print VB86
        printer.setTimes(5000, 2100);
        printer.feed(1);
      }
      if (Prog_Klay == true) {
        printer.println("Klay: ");
        printer.setTimes(30000, 2100);
        printer.printBarcode(Klay_buf, CODE128); // Print Klay
        printer.setTimes(5000, 2100);
        printer.feed(1);
      }
      printer.setSize('M');
      printer.println("TESTSUITE OK");
      printer.setSize('S');
      printer.justify('L');
      printer.feed(2);
      TestsuiteComplete_Check = "Testsuite voltooid: &#9989";
      FirmwareData_Check = "Firmware programmeren:  &#9203";
      Install_Firmware_Command = true;
      Testsuite_Success = false;
    }
    else if (Testsuite_Success != true) {
      printer.print("RTC: ");
      printer.println(RTC_buf);
      if (TestsuiteFlagArray[0] == 0) {
        printer.print("Firmware: ");
        printer.println(Firmware_buf);
      }
      if (TestsuiteFlagArray[0] == 0) {
        Serial.println("Firmware niet gevonden");
        printer.println("Firmware niet gevonden");
      }
      if (TestsuiteFlagArray[1] == 0) {
        Serial.println("Vbatt niet gevonden");
        printer.println("Vbatt niet gevonden");
      }
      if (TestsuiteFlagArray[2] == 0) {
        Serial.println("RTC_con gefaald");
        printer.println("RTC_con gefaald");
      }
      if (TestsuiteFlagArray[3] == 0) {
        Serial.println("Flash_con gefaald");
        printer.println("Flash_con gefaald");
      }
      if (TestsuiteFlagArray[4] == 0) {
        Serial.println("Network gefaald");
        printer.println("Network gefaald");
      }
      if (TestsuiteFlagArray[5] == 0) {
        Serial.println("Tel. Rev. niet gevonden");
        printer.println("Tel. Rev. niet gevonden");
      }
      if (TestsuiteFlagArray[6] == 0) {
        Serial.println("IMEI niet gevonden");
        printer.println("IMEI niet gevonden");
      }
      if (TestsuiteFlagArray[7] == 0) {
        Serial.println("IMSI niet gevonden");
        printer.println("IMSI niet gevonden");
      }
      if (TestsuiteFlagArray[8] == 0) {
        Serial.println("ICCID niet gevonden");
        printer.println("ICCID niet gevonden");
      }
      if (TestsuiteFlagArray[9] == 0) {
        Serial.println("RTC_sync gefaald");
        printer.println("RTC_sync gefaald");
      }
      if (TestsuiteFlagArray[10] == 0) {
        Serial.println("SHT20 gefaald");
        printer.println("SHT20 gefaald");
      }
      if (TestsuiteFlagArray[11] == 0) {
        if (Prog_VB86 == true) {
          Serial.println("VB86 niet gevonden");
          printer.println("VB86 niet gevonden");
        }
        if (Prog_Klay == true) {
          Serial.println("Klay niet gevonden");
          printer.println("Klay niet gevonden");
        }
        if (Prog_VB86 != true && Prog_Klay != true) {
          Serial.println("Sensor niet gevonden");
          printer.println("Sensor niet gevonden");
        }
      }
      printer.feed(2);
      printer.setSize('M');
      printer.justify('C');
      printer.println("TESTSUITE GEFAALD");
      printer.setSize('S');
      printer.justify('L');
      printer.feed(3);
      TestsuiteComplete_Check = "Testsuite voltooid: &#10060";
      SPITS_Status = "<span style=color:#de0404;font-weight:bold>FAILED</span>";
      eBuzzer = true;
      SPITS_EMERGENCY = true;
    }
    //printer.setDefault(); // Printer terugzetten op default
    //digitalWrite(Printer, LOW);
    TestsuiteChecker.reset();
  }
}

void Install_Firmware() {
  if (Install_Firmware_Command == true) {
    // Roep .bat-file Firmware aan
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_INSERT);
    delay(100);
    Keyboard.releaseAll();
    RTU_Data = false;
    Install_Firmware_Command = false;
    FirmwareChecker.start();
  }
  if (FirmwareChecker.ding()) {
    if (RTU_Data != true) {
      digitalWrite(Printer, HIGH);
      Serial.println("GEEN NIEUWE DATA GEVONDEN");
      FirmwareData_Check = "Firmware programmeren: &#10060";
      SPITS_Status = "<span style=color:#de0404;font-weight:bold>FAILED</span>";
      printer.justify('C');
      printer.feed(1);
      printer.setSize('L');
      printer.println("GEEN NIEUWE DATA GEVONDEN");
      printer.setSize('S');
      printer.justify('L');
      printer.feed(3);
      eBuzzer = true;
      SPITS_EMERGENCY = true;
    }
    if (RTU_Data == true) {
      FirmwareData_Check = "Firmware programmeren: &#9989";
      Timer5Min_Check = "Device sleep 5min.:  &#9203";
      Serial.println("NIEUWE DATA");
      for (int i=0; i<6; i++) {
        digitalWrite(Programmer[i], LOW);
      }
      digitalWrite(Printer, LOW);
      Timer5Min.start();
      Serial.println("5 Min. Timer gestart");
      RTU_Data = false;
    }
    FirmwareChecker.reset();
  }
  if (Timer5Min.ding()) {
    Timer5Min_Check = "Device sleep 5min.: &#9989";
    Meting2_Check = "Stroomverbruik < 100μA:  &#9203";
    Serial.println("Timer 5 min. afgegaan");
    Serial2.println("UPR_reset"); // Stuur verzoek voor meting naar U.P.R.
    delay(1000);
    Serial2.println("UPR_reset"); // Stuur verzoek voor meting naar U.P.R.
    delay(2000);
    Serial2.println("UPR_get"); // Stuur verzoek voor meting naar U.P.R.
    delay(1000);
    digitalWrite(Printer, HIGH);
    Curr_Firmware = true;
    Timer5Min.reset();
  }

  if (UPR_CHECK == true && Curr_Firmware == true) {
    if (UPR_NUM <= 100) {
      Serial.println("Lager dan 100μA");
      delay(5000);
      printer.justify('C');
      delay(100);
      printer.setSize('L');
      delay(100);
      printer.println("SPITS OK");
      delay(100);
      printer.setSize('S');
      delay(100);
      printer.justify('L');
      delay(100);
      printer.feed(3); // Print 3 witregels
      printer.setDefault(); // Printer terugzetten op default
      Meting2_Check = "Stroomverbruik < 100μA: &#9989";
      SPITS_Status = "<span style=color:#4cd43d;font-weight:bold>SUCCESS</span>";
      digitalWrite(SL_ORANJE, LOW);
      digitalWrite(SL_GROEN, HIGH);
      digitalWrite(SL_BUZZER, HIGH);
      delay(200);
      digitalWrite(SL_BUZZER, LOW);
    }
    
    if (UPR_NUM > 100) {
      Serial.println("Hoger dan 100μA");
      delay(5000);
      Meting2_Check = "Stroomverbruik < 100μA: &#10060";
      SPITS_Status = "<span style=color:#de0404;font-weight:bold>FAILED</span>";
      printer.justify('C');
      delay(100);
      printer.print("Stroom na 5 minuten: ");
      delay(100);
      printer.print(Current);
      delay(100);
      printer.print("uA");
      delay(100);
      printer.feed(1);
      printer.setSize('L');
      printer.println("SPITS GEFAALD");
      printer.setSize('S');
      printer.justify('L');
      printer.feed(3); // Print 3 witregels
      printer.setDefault(); // Printer terugzetten op default
      eBuzzer = true;
      SPITS_EMERGENCY = true;
    }
    //digitalWrite(Printer, LOW);
    UPR_CHECK = false;
    Curr_Firmware = false;
  }
}



void loop() {
  Serial_Read();
  Serial2_Read();
  RTU_scan();
  UPR_scan();
  Install_Firmware();
  Install_Testsuite();

  int StartKnopStatus = digitalRead(StartKnop);
  if ((StartKnopStatus != vorigeStartKnopStatus) && (StartKnopStatus == LOW)) {
    SPITS_Ready = false;
    Serial.println("STARTKNOP");
    digitalWrite(SL_ORANJE, HIGH);
    digitalWrite(Printer, LOW); // Bonnenprinter uitschakelen
    Install_Testsuite_Command = true;
  }
  vorigeStartKnopStatus = StartKnopStatus;
  
  int StopKnopStatus = digitalRead(StopKnop);
  if ((StopKnopStatus != vorigeStopKnopStatus) && (StopKnopStatus == LOW)) {
    Serial.println("STOPKNOP");
    Timer1Min_Check = "Device opladen 9Ω 1min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Timer1Min2_Check = "Device opladen 0Ω 1min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Meting1_Check = "Stroomverbruik < 100mA:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    TestsuiteControle_Check = "Testsuite programmeren:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    TestsuiteComplete_Check = "Testsuite voltooid:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    FirmwareData_Check = "Firmware programmeren:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Timer5Min_Check = "Device sleep 5min.:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    Meting2_Check = "Stroomverbruik < 100μA:&nbsp&nbsp&nbsp&nbsp&thinsp;";
    SPITS_EMERGENCY = false;
    for(int i=1; i<=NUM_RELAYS; i++){
      digitalWrite(relayGPIOs[i-1], LOW);
    }
    digitalWrite(SL_ROOD, LOW);
    digitalWrite(SL_ORANJE, LOW);
    digitalWrite(SL_BUZZER, LOW);
    digitalWrite(SL_GROEN, LOW);
    Buzzer.setOnOff(false);
    Timer1Min.reset();
    Timer1Min2.reset();
    TestsuiteChecker.reset();
    FirmwareChecker.reset();
    Timer5Min.reset();
    EraseTimer.reset();
    FirstErase.reset();
    digitalWrite(RTU_9ohm, HIGH);
    SPITS_Ready = true;
  }
  
  vorigeStopKnopStatus = StopKnopStatus;
  idle();
  
  if (SPITS_EMERGENCY == true) {
    Timer1Min.reset();
    Timer1Min2.reset();
    TestsuiteChecker.reset();
    FirmwareChecker.reset();
    Timer5Min.reset();
    EraseTimer.reset();
    FirstErase.reset();
    digitalWrite(SL_ORANJE, LOW);
    digitalWrite(SL_ROOD, HIGH);
    digitalWrite(RTU_0ohm, LOW);
    digitalWrite(RTU_9ohm, LOW);
    if (eBuzzer == true) {
      digitalWrite(SL_BUZZER, HIGH);
      delay(200);
      digitalWrite(SL_BUZZER, LOW);
      delay(200);
      digitalWrite(SL_BUZZER, HIGH);
      delay(200);
      digitalWrite(SL_BUZZER, LOW);
      eBuzzer = false;
    }
  }
  
  if ((millis() - lastTime) > timerDelay) {
    events.send("ping",NULL,millis());
    events.send(String(Timer1Min_Check).c_str(),"Timer1Min_Check",millis());
    events.send(String(Timer1Min2_Check).c_str(),"Timer1Min2_Check",millis());
    events.send(String(Meting1_Check).c_str(),"Meting1_Check",millis());
    events.send(String(TestsuiteControle_Check).c_str(),"TestsuiteControle_Check",millis());
    events.send(String(TestsuiteComplete_Check).c_str(),"TestsuiteComplete_Check",millis());
    events.send(String(FirmwareData_Check).c_str(),"FirmwareData_Check",millis());
    events.send(String(Timer5Min_Check).c_str(),"Timer5Min_Check",millis());
    events.send(String(Meting2_Check).c_str(),"Meting2_Check",millis());
    if (SPITS_Ready == true) {
      SPITS_Status = "<span style=color:black;font-weight:bold>READY</span>";
    }
    events.send(String(SPITS_Status).c_str(),"SPITS_Status",millis());
    lastTime = millis();
  }

  if (EraseTimer.ding()) {
    Serial.println("R.T.U. Wiped!");
    for (int i=0; i<6; i++) {
      digitalWrite(Programmer[i], LOW);
    }
    EraseTimer.reset();
  }
}