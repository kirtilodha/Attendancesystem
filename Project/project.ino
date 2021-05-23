#include <ESP8266WiFi.h>                                                    // esp8266 library
#include <FirebaseArduino.h>                                                // firebase library   
#include <NTPClient.h>
#include <WiFiUdp.h>

//Display
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
/* Declare LCD object for SPI
 Adafruit_PCD8544(CLK,DIN,D/C,CE,RST);*/
Adafruit_PCD8544 display = Adafruit_PCD8544(14, 13, 5, 15, 4); /*D5, D7, D1, D8, D6 */
int contrastValue = 60; /* Default Contrast Value */

#define FIREBASE_HOST "test3-374f2-default-rtdb.firebaseio.com"                          // the project name address from firebase id
#define FIREBASE_AUTH "rFdLiV0YWOTwKt8Wf06ZjLTpJHFBI7yVSNYhycCQ"            // the secret key generated from firebase

#define WIFI_SSID "Raja"                                             // input your home or public wifi name 
#define WIFI_PASSWORD "rajesh123"  
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
void setup() {
  pinMode(D1, INPUT);
  pinMode(D6, INPUT);
  digitalWrite(D4, LOW);
  pinMode(D4, OUTPUT);
  String value;
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
  
  //Serial.println(String getString("II/ADC"));
   timeClient.begin();
   timeClient.setTimeOffset(19800); //19080
   
   //Display------
    display.begin();
  /* Change the contrast using the following API*/
  display.setContrast(contrastValue);
  /* Clear the buffer */
  display.clearDisplay();
  display.display();
  delay(100);
  /* Now let us display some text */
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
//  display.setCursor(15,1);
//  display.println("|ESP8266|");
  display.setCursor(15,13);
//  display.println("Welcome");
//  //display.setTextSize(1);
 //display.setTextColor(BLACK);
//  display.setCursor(22,25);
//  display.println("|Nokia|");
//  display.setCursor(25,37);
//  display.println("|5110|");
//  display.display();
  //delay(2000);
   //-------
}

void loop() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  int currentHour = timeClient.getHours();
   String weekDay = weekDays[timeClient.getDay()];
  Serial.print("Week Day: ");
  Serial.println(weekDay);
  Serial.print("Hour: ");
  Serial.println(currentHour);  
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;

  int currentMonth = ptm->tm_mon+1;

  int currentYear = ptm->tm_year+1900;

  //Print complete date:
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");
  //Serial.println
  // put your main code here, to run repeatedly:
  String path="/";
  FirebaseObject object= Firebase.get(path);
  
  String path2= "id1/II/"+weekDay+"/";
  FirebaseObject object_subject= Firebase.get(path2);
  String usnlist_path="usnlist/II/";
  Serial.println(usnlist_path);
  FirebaseObject object_usn= Firebase.get(usnlist_path);
  
  //String subject="id1/II/"+weekDay+"/"+currentHour;
  
  //Serial.println(subject);
  String Student= object_subject.getString(String(currentHour));
  Serial.println(Student);
  String date= "attendance/II/"+Student +"/" + currentDate;
  Serial.println(date);
  String value;
  String attendance="";
  for(int k=0; k<10; k++){
    display.clearDisplay();
    display.setCursor(15,1);
    display.println("Welcome");
    display.setCursor(15,13);
    display.println(currentDate);
    display.display();
    String usn_id = object_usn.getString(String(k+1));
    Serial.println(usn_id);
    String usn= attendance + usn_id;
    value=" ";
    //-----Display
  //display.setCursor(25,37);
  display.setCursor(22,25);
  display.println(usn_id);
  display.display();
    //-------    
    String att;
    
    int button1=0;
  int button2=0;
  while(button1!=1 && button2!=1){
    button1=int(digitalRead(D1)); //default 0
  button2=int(digitalRead(D6)); //default 1
    delay(100);
    }
  if(button1==1){
    att="P";
  digitalWrite(D4, LOW); 
  //Firebase.pushString("/II/ADC", "P");
  value="Sent P";
  Serial.println(value);
  display.println(value);
  display.display();
  delay(100);
  // turn the LED on (HIGH is the voltage level)
  delay(100);          // wait for a second
  }
  else if(button2==1){
    att="A";
  digitalWrite(D4, HIGH); 
  //Firebase.pushString("/II/ADC", "A");
  value="Sent A";
  Serial.println(value);
  display.println(value); 
  display.display();  // turn the LED off by making the voltage LOW
  delay(100);                       // wait for a second
}

    attendance= usn + "," +att+ ";";
    delay(1000);
  }
  
  Firebase.setString(date,attendance);
  
  Serial.println("Data updated "+attendance);

}
