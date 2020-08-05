/* Programa para aquisição de colisões e gravação dos dados em castão SD
Autor:  Victor Oliveira Ferreira
Data da última alteração:   20/07/2020*/

//Incluindo Bibliotecas do micro SD

/*Conexões:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)*/

#include <SPI.h>
#include <SD.h> 
File myFile;

int posicao = 1;
int Vazao = 90;
const int fim = 10;//segundos

  String arquivo = String(Vazao) + "pos" + String(posicao) + ".txt";
  
const int pinoSinal = A0; //PINO ANALÓGICO UTILIZADO PELO MÓDULO
const int pinoLed = 13; //PINO DIGITAL UTILIZADO PELO LED
int i = 0, colisao = 0;
double tempo = 0;

void setup(){
  Serial.begin(9600);
  while(!Serial){;}

  pinMode(pinoSinal, INPUT); //DEFINE O PINO COMO ENTRADA
  pinMode(pinoLed, OUTPUT); //DEFINE O PINO COMO SAÍDA
  digitalWrite(pinoLed, LOW); //LED INICIA DESLIGADO

  Serial.println("Inicializando cartão SD");
  
  if (!SD.begin(4)) {
    Serial.println("Falha no cartão SD!");
    while (1);
  }
  Serial.println("Inicialização de SD completa.");
  myFile = SD.open(arquivo, FILE_WRITE);
  
}
void loop(){

if(myFile){

 if (i < 1){
  myFile.println("#Vazão = " + String(Vazao) + " Lpm");
  myFile.println("#Posição = " + String(posicao));
  i = 1;
 }

tempo = millis();

colisao = analogRead(pinoSinal);

/*Serial.print(colisao);
Serial.print("\t");
Serial.println(tempo);*/

myFile.print(tempo);
myFile.print(" , ");
myFile.println(colisao);

if (tempo/1000 >= fim){ //critério de parada
      Serial.println ("FIM!");
      myFile.close();
      while(1);
}
}else{
    Serial.println("Erro ao criar arquivo");
    while(1);
    }
}
