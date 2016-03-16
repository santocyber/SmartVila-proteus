#include <SPI.h>
#include <Ethernet.h>
//#include <SendCarriots.h> // You must include the library here in order to use it in the sketch
float reading = 0;       
float currentValue = 0;

// Xively parameters
const String APIKEY = "44921ed682813e2bd3e2224de44ee5cd8c92a95d7530459e3aeac8ea6f60e12a"; // Insira aqui sua APIKEY da conta no Carriots, com aspas mesmo
const String DEVICE = "defaultDevice2@santocyber.santocyber"; // ID do dispositivo no Carriots, com aspas também
int flag;
char buffer;
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte gateway[] = { 192,168,1,1 };
byte subnet[] = { 255,255,255,0 };
byte mac[] = {0x00, 0x23, 0xdf, 0x82, 0xd4, 0x01};
byte ip[] = { 192,168,1,20 };
byte server[] = {82,223,244,60};
int port = 80;
//SendCarriots sender; // Instantiate an instance of the SendCarriots library
EthernetClient client; // Inicializa o cliente da rede Ethernet
//1200000
const long interval = 600000; // Intervalo de 20 minutos (20*60*1000 ms)
unsigned long previousMillis = interval-1000;
void setup() {
Serial.begin(9600); // Start serial port
Serial.println(F("Starting"));
Ethernet.begin(mac,ip); // Start the Ethernet connection
  
}
void loop() {
// Read values from the sensor

  reading = analogRead(5); //Raw data reading
  currentValue = (reading - 510) * 5 / 1024 / 0.04 - 0.82; 
Serial.print("Consumo: ");
Serial.println(currentValue);
    delay(300);



unsigned long currentMillis = millis(); // Rotina de contagem do intervalo de 20 minutos
if(currentMillis - previousMillis >= interval) {
previousMillis = currentMillis;
sendStream(); // Chama a função que envia os dados para o servidor do Carriots
}
while(client.available()) { // Rotina de debug: mostra no terminal serial as mensagens recebidas
char c=client.read(); // do servidor do Carriots
Serial.print(c);
}
if(!client.connected()) client.stop();
delay(500);
}
void sendStream() {
char TempString[32]; // Array temporário para armazenar dados
dtostrf(currentValue,2,1,TempString); // dtostrf( [Float variable] , [Minimum SizeBeforePoint] , [sizeAfterPoint] , [WhereToStoreIt] )
String consumo = String(TempString);

if (client.connect(server, 80)) { // Caso haja conexão
Serial.println(F("connected"));
// Constrói a mensagem de dados a ser enviada para o Carriots:
String json = "{\"protocol\":\"v2\",\"device\":\""+String(DEVICE)+"\",\"at\":\"now\",\"data\":{\"Watts\":"+String(consumo)+"}}";
int length=json.length();
// For debugging:
Serial.print(F("carriots.apikey: "));
Serial.println(String(APIKEY));
Serial.print(F("Content-Length: "));
Serial.println(String(length));
Serial.println(F("Connection: close"));
Serial.println(json);
// Constrói o comando HTTP para estabelecer conexão com o api.carriots.com
client.println(F("POST /streams HTTP/1.1"));
client.println(F("Host: api.carriots.com"));
client.println(F("Accept: application/json"));
client.println(F("User-Agent: Arduino-Carriots"));
client.println(F("Content-Type: application/json"));
client.print(F("carriots.apikey: "));
client.println(String(APIKEY));
client.print(F("Content-Length: "));
int thisLength = json.length();
client.println(String(thisLength));
client.println(F("Connection: close"));
client.println();
client.println(json); // Envia a mensagem de dados.
}
else {
// Caso haja falha na conexão:
Serial.println(F("connection failed"));
}
}
