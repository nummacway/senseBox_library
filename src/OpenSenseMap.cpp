#include "OpenSenseMap.h"

OpenSenseMap::OpenSenseMap(const char* boxID) {
  boxId = boxID;
}

OpenSenseMap::OpenSenseMap(const char* boxID, bool enableLogging, const char* serverDomain, unsigned int serverPort)
{
  boxId = boxID;
  enableLog = enableLogging;
  server = serverDomain;
  port = serverPort;
}

void OpenSenseMap::beginEthernet() {
  beginEthernet("192.168.1.123");
}

void OpenSenseMap::beginEthernet(const char* ipAddress)
{
  client = new EthernetClient();

  // give the ethernet module time to boot up
  delay(1000);

  log("Starting ethernet connection...");
  if (Ethernet.begin(mac) == 0)
  {
    log("Failed to configure Ethernet using DHCP, trying static IP\n");
    IPAddress myIp = IPAddress().fromString(ipAddress);
    Ethernet.begin(mac, myIp);
  }
  else
  {
    log("done! IP Address is: \n");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    int number = Ethernet.localIP()[thisByte];
    if (number > 99)
    log((char) (int) (number / 100) + 48 );
  if (number > 9)
    log((char) (int) (number / 10) % 10 + 48 );
    log((char) (int) (number % 10) + 48 );
    log(".");
    }
  }
}

void OpenSenseMap::beginWiFi(char* ssid, char* pass)
{
  client = new WiFiClient;

  //Enable Wifi Shield
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);
  delay(2000);

  //Check WiFi Shield status
  if (WiFi.status() == WL_NO_SHIELD)
  {
    log("WiFi shield not present");
    while (true); // don't continue:
  }
  // attempt to connect to Wifi network:
  while (wifistatus != WL_CONNECTED)
  {
    log("Attempting to connect to SSID: ");
    log(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network
    wifistatus = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    log("\n\n");
    log("Waiting 5 seconds for connection...");
    delay(5000);
    log("done.\n");
  }

}

void OpenSenseMap::uploadValue(int measurement, const char* sensorId)
{
  char obs[10];
  dtostrf((float)measurement, 5, 0, obs);
  return postToOsem(obs, sensorId);
}

void OpenSenseMap::uploadValue(float measurement, const char* sensorId)
{
  char obs[10];
  dtostrf(measurement, 5, 2, obs);
  return postToOsem(obs, sensorId);
}

void OpenSenseMap::postToOsem(char* obs, const char* sensorId)
{
  //json must look like: {"value":"12.5"}
  //post observation to: /boxes/boxId/sensorId
  if (client->connected()) {
    client->flush();
    client->stop();
    delay(1000);
  }

  log("connecting... ");
  log(server);
  if (client->connect(server, port))
  {
    log("  connected\n");
    String value = "{\"value\":";
    value += obs;
    value += "}";
    //
    // log the request that we are about to make
    log("POST /boxes/");
    log(boxId);
    log("/");
    log(sensorId);
    log(" HTTP/1.1\n");
    log("Host:");
    log(server);
    log("\nContent-Type: application/json\n");
    log("Connection: close\n");
    log("Content-Length: ");
    log(value.length());
    log("\n\n");
    // Send the data
    log(value);
    log('\n');

    // make a HTTP POST request
    client->print("POST /boxes/");
    client->print(boxId);
    client->print("/");
    client->print(sensorId);
    client->println(" HTTP/1.1");
    // Send the required header parameters
    client->print("Host:");
    client->println(server);
    client->println("Content-Type: application/json");
    client->println("Connection: close");
    client->print("Content-Length: ");
    client->println(value.length());
    client->println();
    // Send the data
    client->print(value);
    client->println();
    waitForResponse();
  }
}

void OpenSenseMap::waitForResponse()
{
  // if there are incoming bytes from the server, read and print them
  delay(100);
  String response = "";
  char c;
  boolean repeat = true;
  do {
    if (client->available()) c = client->read();
    else repeat = false;
    response += c;
    if (response == "HTTP/1.1 ") response = "";
    if (c == '\n') repeat = false;
  }
  while (repeat);

  log("\nServer Response: ");
  log(response);
  log('\n');

  client->flush();
  client->stop();
}

bool OpenSenseMap::log(const char data)
{
  if (!enableLog || !Serial) return false;

  Serial.print(data);
  return true;
}

bool OpenSenseMap::log(const char* data)
{
  if (!enableLog || !Serial) return false;

  Serial.print(data);
  return true;
}

bool OpenSenseMap::log(String data)
{
  if (!enableLog || !Serial) return false;

  Serial.print(data);
  return true;
}
