
#include <SPI.h>
#include <Ethernet.h>

// MAC address and IP address for controller
// The IP address will be dependent on the local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,12);

// Initialize the Ethernet server library
EthernetServer server(80);
// Initialize array for sensorvalues
int sensorValues[400];
int reading = 0;

// used for delaying the sensor reading without delaying the webserver
unsigned long startMillis = 0;
int count = 0;


void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  startMillis = millis();
}


void loop() {
  if (millis() - startMillis > 10000){ //increse count every ten second
    count += 1;
    startMillis = millis();
  }
  readSoilValues();
  if (reading == 99) { //restart list after 100 readings. Because of low arduino memory (could wrap the list instead)
    emptyList();
  }
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // Standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 600");  // refresh the page automatically every 10 min
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          client.println("reading: 0 <br />");
          // print the read data in the sensor array
          for (int i = 0; i < reading*4; i += 1){ 
            client.print("analog input ");
            client.print(i%4);
            client.print(" is ");
            client.print(sensorValues[i]);
            client.println("<br />"); 
            if (i % 4 == 3){
              client.println("<br />"); 
              client.println("reading: ");
              client.print(i / 4 + 1);
              client.print("<br />");
            }
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // Starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // Character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void readSoilValues(){
  if (count >= 30) { //only does a read every five minutes. Should be 30.
    count = 0;
    for (int   analogChannel = 0; analogChannel < 4; analogChannel++) {
      digitalWrite(analogChannel + 3, HIGH); // send power through the soil in the selected pot
      delay(500);
      int sensorReading = analogRead(analogChannel); //read level of ressitance
      sensorValues[4*reading + analogChannel] = sensorReading; // This creates entrys in the sensor value list
      digitalWrite(analogChannel + 3, LOW); // stop power
    }
    reading += 1;
  }
}

void emptyList(){
  for (int j = 0; j < 400; j += 1){
    sensorValues[j] = 0;
  }
  reading = 0;
}




