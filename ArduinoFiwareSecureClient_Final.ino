#include <Bridge.h>
#include <Process.h>
#include "DHT.h"
// DHT11 sensor pins
#define DHTPIN 2 
#define DHTTYPE DHT11

// DHT instance
DHT dht(DHTPIN, DHTTYPE);


const char *entity_id="Room50";
const char *sensorid="iot_sensor_86dcb3dfad9b46aa855e40a8317e602a";
const char *password="8193251f34c5495ea504fe005f61f920";
char body[400];
String uri="https://130.240.134.129:8443/se.ltu.ssr.webapp/rest/fiwareproxy/ngsi10/updateContext/";

Process proc;
int mem(){
  uint8_t * heapptr, * stackptr;
  stackptr = (uint8_t *)malloc(4);
  heapptr = stackptr;
  free(stackptr);               
  stackptr = (uint8_t *)(SP);
  return stackptr - heapptr;
}
void setup() {
  
  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  dht.begin();
  digitalWrite(13, HIGH);
  SerialUSB.begin(9600);
  
  while (!SerialUSB){
  digitalWrite(13, LOW);
 
  digitalWrite(13, HIGH);
  
  }; // wait for a serial connection
  SerialUSB.print("mem:");
  SerialUSB.println(mem());
}
void loop() {
  
   SerialUSB.println("Starting sending request");
   
    
   digitalWrite(13, LOW); 
    send_request();
    digitalWrite(13, HIGH);
    SerialUSB.println("waiting for response");
    digitalWrite(13, LOW);
    wait_response();
    digitalWrite(13, HIGH);
    
    digitalWrite(13, LOW);
    read_response();
    digitalWrite(13, HIGH);
    SerialUSB.println();
    SerialUSB.flush();
    digitalWrite(13, LOW);
    //delay(900000);
    delay(60000);
    digitalWrite(13, HIGH);
}
/*
** Send an HTTP POST request to the SSR gateway using NGSI 10 API
*/
void send_request()
{
  
 
 
 int temp=dht.readTemperature();
 int humid=dht.readHumidity();
 SerialUSB.print("T:");
 SerialUSB.println(temp);
 SerialUSB.print("H:");
 SerialUSB.println(humid);

  sprintf(body,"{\"contextElements\":[{\"type\":\"Room\",\"isPattern\":\"false\",\"id\":\"%s\",\"attributes\":[{\"name\":\"temperature\",\"type\":\"integer\",\"value\":%d},{\"name\":\"humidity\",\"type\":\"integer\",\"value\":%d}]}],\"updateAction\":\"APPEND\",\"auth\":{\"username\":\"%s\",\"password\":\"%s\"}}",entity_id,temp,humid,sensorid,password) ; 
  proc.begin("curl");
  proc.addParameter("-k");
  proc.addParameter("-X");
  proc.addParameter("POST");
  proc.addParameter("-H");
  proc.addParameter("Content-Type:application/json");
  proc.addParameter("-H");
  proc.addParameter("Accept: application/json");


  // POST body
  proc.addParameter("-d");
  //sprintf(buffer, "{\"value\": %d}", value);
 //proc.addParameter(updateRequest);
  proc.addParameter(body);
  
  SerialUSB.println(body);
  //SerialUSB.println(updateRequest);
  //SerialUSB.println(updateRequest.length());
  // POST URI
  //sprintf(buffer, "https://%s/tables/%s", server, table_name);
  proc.addParameter(uri);
 SerialUSB.println(uri);
  // Run the command synchronously
  proc.run();
  SerialUSB.println("request send");
}

/*
** Wait for response
*/

void wait_response()
{
  long start=millis();
  while (!proc.available()) {
    delay(100);
    SerialUSB.print(".");
    long cur=millis();
    if(cur-start>1000){
      
      break;
    }
      
  }
}


/*
** Read the response and dump to console
*/

void read_response()
{
  bool print = true;
  

  while (proc.available() > 0) {
    char c = proc.read();
    SerialUSB.print(c);
  }
  SerialUSB.println();
}

