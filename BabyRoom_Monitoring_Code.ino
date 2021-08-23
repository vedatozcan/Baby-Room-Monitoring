/*
  Outher: Vedat Özcan 
  -> References: https://www.arduino.cc/reference/en/
  -> Reference_1: https://forum.arduino.cc/t/rtc-library-with-day-number-of-year/401450
  -> Reference_2: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573/2
  
  Thresholds are: 
  -> Humidity ϵ [30%, 50%] → “Optimal Humidity” 
  -> Humidity < 30% → “Low Humidity” 
  -> Humidity > 50% → “High Humidity” 
  -> Light >= 400 → “Day” 
  -> Light < 400 → “Night” 
  -> Temperature during Day ϵ [20°C, 24°C] → “Optimal Temperature” 
  -> Temperature during Day < 20°C → “Cold” 
  -> Temperature during Day > 24°C → “Hot” 
  -> Temperature during Night ϵ [18°C, 21°C] → “Optimal Temperature” 
  -> Temperature during Night < 18°C → “Cold” 
  -> Temperature during Night > 21°C → “Hot” 

  In the environment where the sensor is located:
  -> Humidity ϵ [20%, 35%] → “Optimal Humidity” 
  -> Humidity < 20% → “Low Humidity” 
  -> Humidity > 35% → “High Humidity” 
  -> Light >= 400 → “Day” 
  -> Light < 400 → “Night” 
  -> Temperature during Day ϵ [28°C, 32°C] → “Optimal Temperature” 
  -> Temperature during Day < 28°C → “Cold” 
  -> Temperature during Day > 32°C → “Hot” 
  -> Temperature during Night ϵ [24°C, 28°C] → “Optimal Temperature” 
  -> Temperature during Night < 24°C → “Cold” 
  -> Temperature during Night > 28°C → “Hot” 
  
*/

#include <DHT.h>
#include <DHT_U.h>


#define DHTPIN 3       // digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // the type of sensor in use

DHT dht(DHTPIN, DHTTYPE); // initialize DHT sensor.

int lightValue; // the variable that holds the light value
float humidity; // the variable that holds the humidity value
float temperature; // the variable that holds the temperature value

String l, h, t; // variables that hold the information message that the light, temperature and humidity values correspond to


String CURRENT_TIME = "2021.05.26 - 14:22:03";  

/* Parsing date and converting to integer */
int year = CURRENT_TIME.substring(0,4).toInt();
int month = CURRENT_TIME.substring(5,7).toInt();
int day = CURRENT_TIME.substring(8,10).toInt(); 
  
/* Parsing time and converting to integer */
int hour = CURRENT_TIME.substring(13,15).toInt();
int minute = CURRENT_TIME.substring(16,18).toInt();
int sec = CURRENT_TIME.substring(19,21).toInt();  

/* These variables were kept to increment the seconds value 
   one by one and update the time and date */
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;

/* These variables were kept to switch screens 
   at intervals of 3 seconds */
unsigned long currentTime;
const unsigned long interval = 3000;



void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
    
    currentTime = millis(); // take the start time 
    while(millis() - currentTime <= interval){    
      updateSerialMonitor(); // update the time and show information after 3 seconds 
    }
    cal_light_humidity_temperature(); // show information of baby room

}


void cal_light_humidity_temperature(){
  lightValue = analogRead(A0); // reading light value
  humidity = dht.readHumidity(); // reading humidity 
  temperature = dht.readTemperature(); // reading temperature as Celsius (the default)

  if (isnan(humidity) || isnan(temperature)) { // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
   
  if(humidity < 20.00)
    h = "Low Humidity";
  else if(humidity > 35.00)
    h = "High Humidity";
  else
    h = "Optimal Humidity";
   

  if(lightValue >= 400)
    l = "Day";
  else if(lightValue < 400)
    l = "Night";


  if(l == "Day"){
    if(temperature < 28)
      t = "Cold";
    else if(temperature > 32)
      t = "Hot";
    else
      t = "Optimal Temperature"; 
  }
  else if(l == "Night"){
    if(temperature < 24)
      t = "Cold";
    else if(temperature > 28)
      t = "Hot";
    else
      t = "Optimal Temperature"; 
  }

  Serial.print("Date: ");
  if(day < 10) 
    Serial.print("0");
  Serial.print(day);
  Serial.print(".");
  if(month < 10) 
     Serial.print("0");
  Serial.print(month);
  Serial.print(".");
  Serial.print(year);
  Serial.print(" - ");

  Serial.print("Hour: ");
  if(hour < 10)
    Serial.print("0");
  Serial.print(hour);
  Serial.print(":");
  if(minute < 10) 
     Serial.print("0");
  Serial.print(minute);
  Serial.print(":");
  if(sec < 10) 
     Serial.print("0");
  Serial.print(sec);
  Serial.print(" - ");
  
  Serial.print("Light: ");
  Serial.print(lightValue);
  Serial.print(" - ");
  
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C "));
  Serial.print(" - ");

  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println("%");

  Serial.print(l);
  Serial.print(" - ");
  Serial.print(t);
  Serial.print(" - ");
  Serial.println(h);
  Serial.println("------------------------------------");

}

/* The function that updates the Serial Monitor */
void updateSerialMonitor(){
  currentMillis = millis(); 
  if(currentMillis - startMillis >= period){
    // increase 1 second every 1000 milliseconds
    sec++; 
    if(sec == 60){
      sec = 0;
        // when 60 seconds have passed, increase the minute by 1
      minute++; 
    }
    if(minute == 60){
      minute = 0;
        // when 60 minutes have passed, increase the hour by 1
      hour++; 
    }
    if(hour == 24){
      hour = 0;
       /* Look at what day of the month after 24 hours.
          If it is the last day of any month, set the day to 1. 
          Otherwise increase 1 */
      if(day == getDaysInMonth(month,year)){ 
          day = 1;
           /* If it is the end of the 12th month, increase 
              the year by 1 and make the month value 1.
              Otherwise, increase the month value by 1 */
          if(month == 12){
        month = 1;
          year++;
          }
            else
              month++;
      }
        else
          day++;
    }
    startMillis = currentMillis; 
  }
}

/* Function that returns the days of the 
   week using the Gregorian Calendar formula */
int getWeekDay(int day, int month, int year){
  if (month <= 2){
    month += 12;
    --year;
  }
  int j = year % 100;
  int e = year / 100;
  return (((day + (((month + 1) * 26) / 10) + j + (j / 4) + (e / 4) - (2 * e)) - 2) % 7);
}

/* The function that returns the total 
   number of days of the relevant year */
int getDayOfYear(const int day, const int month, const int year){
  int d = day;
  int m = month;
  while(--m) 
    d += getDaysInMonth(m, year);
  return d;
}

/* Function that returns whether February is 29 days or not */
bool isLeapYear(const int year){
  return ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0));
}

/* Function that returns how many days the months are */
int getDaysInMonth(const int month, const int year){
  if (month == 2)  
    return isLeapYear(year) ? 29 : 28;
  else if (month == 4 || month == 6 || month == 9 || month == 11)  
      return 30;   
  return 31;
}
