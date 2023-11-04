#include "DHT.h"
#include <LiquidCrystal.h>
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //ATRIBUIÇÃO DE ENDEREÇO MAC AO ETHERNET SHIELD W5100
//byte ip[] = { 192, 168, 1, 166 }; //ATRIBUIÇÃO DE IP
//byte gateway[] = {192, 168, 1, 0}; //GATEWAY DE CONEXÃO 
byte ip[] = { 10, 90, 36, 166 };
byte gateway[] = {10, 90, 36, 1};
byte subnet[] = {255, 255, 254, 0}; //MASCARA DE REDE 
EthernetServer server(80); //PORTA EM QUE A CONEXÃO SERÁ FEITA

String readString = String(30); //VARIÁVEL PARA BUSCAR DADOS NO ENDEREÇO (URL)

int porta_rele1 = 30;//IN rele 01
int status = 0;


int porta_rele2 = 46;//IN rele 02
int status2 = 0;

float tempFloat;
float humFloat;

#define DHTPIN 22     //Pino22 DHT11
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(8,9,4,5,6,7); 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
 
 Serial.begin(9600); 

 Ethernet.begin(mac, ip, gateway, subnet); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE
 server.begin();
 dht.begin();
  
 pinMode(porta_rele1, OUTPUT);
 digitalWrite(porta_rele1, HIGH); //LED INICIA DESLIGADO 
 pinMode(porta_rele2, OUTPUT);
 digitalWrite(porta_rele2, HIGH); //LED INICIA DESLIGADO

 lcd.begin(16, 2);  
 lcd.setCursor(0,0);  
 lcd.print("Grower bot");  
 lcd.setCursor(0,1);  
  
}  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 void temp(float temperature,float humidity){
      
  
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  delay(2000);
  lcd.clear();

 char tempF[6]; 
 char humF[6];
 dtostrf(temperature, 2, 0, tempF);
 dtostrf(humidity, 2, 0, humF);

 lcd.setCursor(0,0);  
 lcd.print("Temperatura:"); 
 lcd.print(tempF);
 lcd.print((char)223);
 lcd.print("C ");
 lcd.setCursor(0,1);  
 lcd.print("Humidade: ");
 lcd.print(humF);
 lcd.print("%");
 
 float tempFloat = atof(tempF); // Converte o valor de string para float
 float humFloat = atof(humF);   // Converte o valor de string para float

if(tempFloat > 25){
     onRele01();//Liga relé 01
     //Serial.print("Valor lido: ");
     //Serial.println(tempF);   
  }
if(tempFloat < 23){
     offRele01();//Liga relé 01
     //Serial.print("Valor lido: ");
     //Serial.println(tempF);
  }
if(humFloat < 93){
     onRele02();//Liga relé 01
     //Serial.print("Valor lido: ");
     //Serial.println(humF);
  }
if(humFloat >= 95){
    offRele02();//Liga relé 01
     //Serial.print("Valor lido: ");
     //Serial.println(humF);
    }
   
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onRele01(){
  digitalWrite(porta_rele1, LOW );  //Liga relé 01
      status = 1;
      lcd.clear();
      lcd.print ("Rele 1 Ligado");
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void offRele01(){
  digitalWrite(porta_rele1, HIGH );  //Desliga relé 01
      status = 0;
      lcd.clear();
      lcd.print ("Rele 1 Desligado");
  }
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onRele02(){
  digitalWrite(porta_rele2, LOW );  //Liga relé 02
      status2 = 1;
      lcd.clear();
      lcd.print ("Rele 2 Ligado");
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void offRele02(){
  digitalWrite(porta_rele2, HIGH );  //Desliga relé 02
      status2 = 0;
      lcd.clear();
      lcd.print ("Rele 2 Desligado");
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
void loop(){  

 int botao;  
 botao = analogRead (0);  //Leitura do valor da porta analógica A0

  if (botao < 50){ 
      temp(0,0);   
 // lcd botão ("Direita ")
 }
 else if (botao < 100){  
   onRele01();//Liga relé 01
  // lcd botão ("Cima ")
 }    
  else if (botao < 300){  
  offRele01(); //Desliga relé 01 
  // lcd botão ("Baixo ")
 }  
 else if (botao < 450){  
  onRele02();  //Liga relé 02
     // lcd botão ("Esquerda ")   
 }  
 else if (botao < 800){  
  offRele02();  //Desliga relé 02
  //lcd botão ("Select  ");
 }
  
// Exibição no Serial Monitor

// int valor = analogRead(A0);
// Serial.print("Valor lido: ");
// Serial.println(valor);
//  delay(1000); // Aguarda 1 segundo 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EthernetClient client = server.available(); //CRIA UMA CONEXÃO COM O CLIENTE
  if (client) { // SE EXISTE CLIENTE, FAZ
    while (client.connected()) {//ENQUANTO EXISTIR CLIENTE CONECTADO, FAZ
   if (client.available()) { //SE O CLIENTE ESTÁ HABILITADO, FAZ
    char c = client.read(); //LÊ CARACTERE A CARACTERE DA REQUISIÇÃO HTTP
    if (readString.length() < 100) //SE O ARRAY FOR MENOR QUE 100, FAZ
      {
        readString += c; // "readstring" VAI RECEBER OS CARACTERES LIDO
      }  
        if (c == '\n') { //SE ENCONTRAR "\n" É O FINAL DO CABEÇALHO DA REQUISIÇÃO HTTP
          
          if (readString.indexOf("?") <0){ //SE ENCONTRAR O CARACTER "?", FAZ
          }else //SENÃO,FAZ
        
        if(readString.indexOf("rele1=1") >0){ //SE ENCONTRAR O PARÂMETRO "rele1=1", FAZ
             onRele01();
                        
           }else if(readString.indexOf("rele1=0") >0){ //SENÃO, FAZ
             offRele01();
             
           }

        if(readString.indexOf("rele2=1") >0){ //SE ENCONTRAR O PARÂMETRO "rele2=1", FAZ
             onRele02();
             
           }else if(readString.indexOf("rele2=0") >0){ //SENÃO, FAZ
             offRele02();
             
           }
           
          client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
          client.println("Content-Type: text/html"); //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
          client.println("");
          client.println("<!DOCTYPE HTML>"); //INFORMA AO NAVEGADOR A ESPECIFICAÇÃO DO HTML
          client.println("<html>"); //ABRE A TAG "html"
          client.println("<head>"); //ABRE A TAG "head"
          //client.println("<link rel='icon' type='image/png' href=''/>"); //DEFINIÇÃO DO ICONE DA PÁGINA
          client.println("<title>Central de controle Humidade e Temperatura:</title>"); //ESCREVE O TEXTO NA PÁGINA
          client.println("</head>"); //FECHA A TAG "head"
          client.println("<body style=background-color:#ADD8E6>"); //DEFINE A COR DE FUNDO DA PÁGINA
           //A LINHA ABAIXO ESCREVE "Central de Controle Humidade e Temperatura: " EM COR AZUL NA PÁGINA
          client.println("<center><font color='blue'><h1>Central de Controle Humidade e Temperatura:</font></center></h1>");
          client.println("<h1><center>CONTROLE RELES</center></h1>"); //ESCREVE "CONTROLE RELES" NA PÁGINA
          
          if (status == 1){ //SE VARIÁVEL FOR IGUAL A 1, FAZ
          
          /*A LINHA ABAIXO CRIA UM FORMULÁRIO CONTENDO UMA ENTRADA INVISÍVEL(hidden)
          COM O PARÂMETRO DA URL E CRIA UM BOTÃO APAGAR (CASO O LED ESTEJA LIGADO)*/
          client.println("<center><form method=get name=rele01><input type=hidden name=rele1 value=0 /><input type=submit value=APAGAR></form></center>");
          }else{ //SENÃO, FAZ
            
          /*A LINHA ABAIXO CRIA UM FORMULÁRIO CONTENDO UMA ENTRADA INVISÍVEL(hidden) 
           COM O PARÂMETRO DA URL E CRIA UM BOTÃO ACENDER (CASO O LED ESTEJA DESLIGADO)*/
          client.println("<center><form method=get name=rele01><input type=hidden name=rele1 value=1 /><input type=submit value=ACENDER></form></center>");
          }
          client.println("<center><font size='5'>Status atual do rele temperatura: </center>"); //ESCREVE "Status atual do rele 01:" NA PÁGINA
          if (status == 1){ //SE VARIÁVEL FOR IGUAL A 1, FAZ
              client.println("<center><font color='green' size='5'>LIGADO</center>"); //ESCREVE "LIGADO" EM COR VERDE NA PÁGINA
          }else{ //SENÃO, FAZ
              client.println("<center><font color='red' size='5'>DESLIGADO</center>"); //ESCREVE "DESLIGADO" EM COR VERMELHA NA PÁGINA
          }     
          client.println("<hr/>"); //TAG HTML QUE CRIA UMA LINHA HORIZONTAL NA PÁGINA
          
          if (status2 == 1){ //SE VARIÁVEL FOR IGUAL A 1, FAZ
            
          /*A LINHA ABAIXO CRIA UM FORMULÁRIO CONTENDO UMA ENTRADA INVISÍVEL(hidden)
          COM O PARÂMETRO DA URL E CRIA UM BOTÃO APAGAR (CASO O LED ESTEJA LIGADO)*/
          client.println("<center><form method=get name=rele02><input type=hidden name=rele2 value=0 /><input type=submit value=APAGAR></form></center>");
          }else{ //SENÃO, FAZ
            
          /*A LINHA ABAIXO CRIA UM FORMULÁRIO CONTENDO UMA ENTRADA INVISÍVEL(hidden)
          COM O PARÂMETRO DA URL E CRIA UM BOTÃO ACENDER (CASO O LED ESTEJA DESLIGADO)*/
          client.println("<center><form method=get name=rele02><input type=hidden name=rele2 value=1 /><input type=submit value=ACENDER></form></center>");
          }
          client.println("<center><font size='5' color='black'>Status atual do rele Humidade: </center>");  
          if (status2 == 1){ //SE VARIÁVEL FOR IGUAL A 1, FAZ
              client.println("<center><font color='green' size='5'>LIGADO</center>"); 
          }else{ //SENÃO, FAZ
              client.println("<center><font color='red' size='5'>DESLIGADO</center>"); 
          }
               
          client.println("<hr/>"); //TAG HTML QUE CRIA UMA LINHA HORIZONTAL NA PÁGINA
          client.println("<hr/>"); //TAG HTML QUE CRIA UMA LINHA HORIZONTAL NA PÁGINA

          client.println("<p>Informações Adcionais:</p>");
          
          client.print("<p>Temperatura atual: ");
          client.print(tempFloat);
          client.println("°C </p>");
          
          client.print("<p>Humidade atual: ");
          client.print(humFloat);
          client.println("% </p>");
          
          client.println("</body>"); 
          client.println("</html>"); 
          readString=""; //A VARIÁVEL É REINICIALIZADA
          c="";
          client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
          
            }
          }
        }
      }
 }

 
