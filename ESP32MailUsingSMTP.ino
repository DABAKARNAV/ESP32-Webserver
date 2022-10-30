
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define WIFI_SSID "Airel_9606350096"
#define WIFI_PASSWORD "air84358"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "iiotminiproject@gmail.com"
#define AUTHOR_PASSWORD "qublblfjmavpdqwu"

/* Recipient's email*/
#define RECIPIENT_EMAIL "arnav48.dabak@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;
SMTP_Message message;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);


/* Decalarations for the DHT Sensor*/
#include<DHT.h>
#define DHT11PIN 15
#define DHTTYPE DHT11
DHT dht(DHT11PIN, DHTTYPE);
float tc;


/* Declarations for the Webserver to control the Motor*/
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const int ledPin = 4;
String pwmSliderValue = "0";

// PWM Settings
const int frequencyHz = 5000;
const int pwmChannel = 0;
const int resolution = 8;
const char* INPUT_PARAMETER = "value";

// Instatiate the AsyncWebServer object on port 80
AsyncWebServer webServer(80);

// Now Declaring the Web Page with the HTML Code
// Declare the webpage
// HTML comments look like this <! comment in between here >
const char htmlCode[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <!  define meta data >
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <! define the style CSS of your page >
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h1 {font-size: 2.9rem;}
    h2 {font-size: 2.1rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 30px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 400px; height: 15px; border-radius: 5px; background: #39a6de; outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 25px; height: 25px; border-radius: 12px; background: #f74d4d; cursor: pointer;}
    .slider::-moz-range-thumb { width: 25px; height: 25px; border-radius: 12px; background: #F74D4D; cursor: pointer; } 
  </style>
</head>

<body>
  <! Edit the pages your heading 1 >
  <h1>IIOT MINI PROJECT 2022</h1>
  <h2>Over Ride Control of the Cooling System</h2>

  <! Adding the logo here >
  <img src="https://www.google.com/imgres?imgurl=https%3A%2F%2Fwww.tibco.com%2Fsites%2Ftibco%2Ffiles%2Fmedia_entity%2F2021-04%2Fiiot-final.svg&imgrefurl=https%3A%2F%2Fwww.tibco.com%2Freference-center%2Fwhat-is-iiot&tbnid=qmTjjgw28sW2GM&vet=12ahUKEwiWrN_3kof7AhUUj9gFHa6SBwYQMygAegUIARDeAQ..i&docid=vVhSYTq3y0IfzM&w=820&h=486&q=iiot&ved=2ahUKEwiWrN_3kof7AhUUj9gFHa6SBwYQMygAegUIARDeAQ">
  
  <! Edit the pages your heading 2 >
  <h2>ESP32 PWM Slider</h2>
  <h2>Motor Speed</h2>
  
  <! Displays the value of the slider >
  <p><span id="textSliderValue">%SLIDERVALUE%</span> &#37</p>
  <! displays the range of the slider 0 - 100 in steps of 1 >
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="100" value="%SLIDERVALUE%" step="1" class="slider"></p>
<script>
function updateSliderPWM(element) {
  var pwmSliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = pwmSliderValue;
  console.log(pwmSliderValue);
  var httpRequest = new XMLHttpRequest();
  httpRequest.open("GET", "/slider?value="+pwmSliderValue, true);
  httpRequest.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces the placeholder with the button in your web page
String updateButton(const String& var){
  if (var == "SLIDERVALUE"){
    return pwmSliderValue;
  }
  return String();
}





void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to Access Point");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        delay(200);
      }
  Serial.println("");
  Serial.println("WiFi connected.");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
  //Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
 /* SMTP_Message message;*/

  /* Set the message headers */
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Temperature Alert!!!!";
  message.addRecipient("Arnav", RECIPIENT_EMAIL);

  /*
  //Send HTML message
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;*/

  
  //Send raw text message
  
  
  String textMsg = "The Temperature is beyond Optimal Range, Please over-ride the control of the Cooling System";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
  
  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Now we begin with slider and ESP32 Communications*/
  //Serial.begin(115200);
  ledcSetup(pwmChannel, frequencyHz, resolution);
  ledcAttachPin(ledPin, pwmChannel);
  ledcWrite(pwmChannel, pwmSliderValue.toInt());

    // connect to wifi
    /*WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) 
    {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    } */
    // Printing the IP Address of the device
    Serial.println(WiFi.localIP());
    // Web Page
    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    request->send_P(200, "text/html", htmlCode, updateButton);
    });

    // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
    webServer.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(INPUT_PARAMETER)) {
      inputMessage = request->getParam(INPUT_PARAMETER)->value();
      pwmSliderValue = inputMessage;
      if( tc>35)
      {
       ledcWrite(pwmChannel, pwmSliderValue.toInt()); 
      }
      
    }
    else 
    {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  
  // Start server (remembering its on port 80, 80 is the default port for a Async Web Server )
  webServer.begin(); 


  Serial.begin(9600);
  dht.begin();
  delay(1000);
}

void loop()
{
  delay(5000);
  tc = dht.readTemperature();
  Serial.print('\n');
  Serial.print("Temperature = ");
  Serial.print(tc);
  Serial.print("°C");

  if(tc>35)
  {
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
  } 

   
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}
