/* Sistema de aquisição de dados com a arduino para leito fluidizado
 * Módulos: Sensor de temperatura NTC, cartão SD, sensor de pressão diferencial MPX5010dp e conversor ADC ADS1115
 * Por: Victor Oliveira Ferreira            Em: 11/08/2020.
 */

//======================================== Conexões: Ver seção após Void Loop=========================================


/*==================================================================================================================*/
/*============================================ Incluindo Bibliotecas ===============================================*/

// Conversor ADC ADS1115
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads1115(0x48);  // Endereço do ADC na entrada I2C do arduino - Padrão: (0x48)

int16_t adc0;  // Variáveis de medida da entrada analógica do ADC
float multiplier = 0.1875F;               // ADS1115  @ +/- 6.144V gain = 0.1875mV/step
float adcScale   = 4.8828F;               // Arduino 10 bit @ 5 volts   = 4.88mV/step

// Cartão SD
#include <SPI.h>
#include <SD.h> 

File myFile; //Nome dado ao arquivo no programa


// Sensor de temperatura NTC
#include <Thermistor.h> //Pacote
Thermistor temp(0); //Definição da entrada do termopar

/*==================================================================================================================*/

/*======================================== Declarando Variáveis ====================================================*/

// Número de iterações da média móvel:
#define n 20

// Função Média móvel:
long moving_average();

// Vetor de valores para tirar a média
int num[n];

//Definição da condição para que o void loop seja reiniciado
int condition = 1;

// Variáveis de tempo
int temporal = 0;     //Tempo na análise
int temporalaux1 = 0; //Tempo total
int temporalaux2 = 0; //Tempo total no início da análise
int i = 1, j = 1, k = 0; //Contadores de tempo, número de valores para cálculo da média
int dt = 200; //intervalo de tempo entre as medidas da média temporal
int fim = 2 /*minutos*/ *60;//Definição do tempo para finalizar a média temporal


// Nome do arquivo no cartão SD
int Tomada_P = 0;
int Vazao = 90;
String arquivo = String(Vazao) + "lpm-p" + String(Tomada_P) + ".txt";

// Variáveis relativas a Pressão
int P_filtro;

// Variáveis relativas a temperatura
int T_medida; //valor filtrado via média móvel para a medida de temperatura
int vals[n]; //array das medidas com N valores para a temperatura


/*==================================================================================================================*/

/*============================================= Void Setup =========================================================*/
void setup() {

Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Inicializando cartão SD");
  
  if (!SD.begin(4)) {
    Serial.println("Falha no cartão SD!");
    while (1);
  }
  Serial.println("Inicialização de SD completa.");
  
  //Pino do LED
  pinMode(8,OUTPUT);  //Define qual pino
  digitalWrite(8,LOW);//Define qual o estado


// The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1115
  //                                                                -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.0078125mV

ads1115.begin();  // init ADS1115 ADC
}
/*==================================================================================================================*/

/*=================================================== Void Loop ====================================================*/
void loop() {

// Testando se está tudo ok com cartão SD e criando arquivo
if (i==1){
  myFile = SD.open(arquivo, FILE_WRITE);
}

// Abrindo o arquivo no cartão:
if (myFile){
  // Se etiver tudo certo com o arquivo, então:
  if (i == 1){
    Serial.println("Arquivo sendo escrito");  
    Serial.println("Tomada de pressão atual = " + String(Tomada_P));
    delay (2000);
    i = 2;
    } else if (i == 2){
      Serial.println("Começando!");
      myFile.println("Vazão = " + String(Vazao) + " Lpm;");
      Serial.println("Vazão = " + String(Vazao));
      myFile.println("Tomada de Pressão = " + String(Tomada_P) + ";");
      myFile.println("Medida , Sinal de pressão (bits) , Temperatura (ºC)");
      i = 3;
  }

condition = 1; //Retorna o valor para condição um, permitindo que o programa seja executado por completo

// Medição dos valores para a pressão:
adc0 = ads1115.readADC_SingleEnded(0); // Lê valores da entrada 0 do ADC ADS1115
P_filtro = moving_average(); // Aplica o filtro de média móvel aos valores de pressão em bits


//============= TEMPERATURA =============

  int value = temp.getTemp();
  //===========================
  for(int i = n - 1; i > 0; i--){
    vals[i] = vals[i-1];
  }
  vals[0] = value;

  int sum = 0;
  for(int i = 0; i < n; i++){
    sum = sum + vals[i];
  }
  T_medida = (sum / n);

//======================================


temporal = millis()/1000-temporalaux2; //Tempo de análise

//Início da aquisição: dados não levados em consideração para a média temporal
if (temporal < 10){  
  Serial.print (P_filtro);
  Serial.print (" bits");
  Serial.print ('\t');
  Serial.print (T_medida);
  Serial.print (" °C");
  Serial.print ('\t');
  Serial.print (temporal);
  Serial.println (" seg");

  k=0; //Retorna o valor 0 para o número de dados de pressão obtidos
}else{
    k = k+1; //conta a quantidade de dados adiquiridos

    Serial.print (k);
    Serial.print ('\t');
    Serial.print (P_filtro);
    Serial.print (" bits");
    Serial.print ('\t');
    Serial.print (T_medida);
    Serial.print (" °C");
    Serial.print ('\t');
    Serial.print (temporal);
    Serial.println (" seg");


    myFile.print (k);
    myFile.print (" , ");
    myFile.print (P_filtro);   
    myFile.print (" , ");
    myFile.println (T_medida);

    //Algorítimo de final de aquisição de dados
    if (temporal >= fim){ //critério de parada
      Serial.println ("FIM!");
      Serial.println ("TROQUE A TOMADA DE PRESSAO!");
      myFile.close();
      
      for (condition = 1; condition < 2; condition = 1*condition){ //Enquanto o valor condicional for um, só piscar o led e esperar comando
        digitalWrite (8, HIGH);
        delay (500);
        digitalWrite (8, LOW);
        delay (500);

        //Abertura da porta serial
        if (Serial.available() > 0){
        if (Serial.read()!=0){ //Condição para reinício do void loop
        condition = 2;
        i = 1;
        //j = 4*n; //Retira a necessidade da fase inicial da aquisição de dados, pulando direto para a média temporal
        k = 0; //Zera contador de valores da média temporal
        temporalaux1 = millis()/1000; //Calcula o tempo total até o comando de reinício
        temporalaux2 = temporalaux1; //armazena o tempo total até o comando de reinício
        Tomada_P = Tomada_P + 1; //Passando para a próxima tomada de pressão!
        arquivo = "90lpm-p" + String(Tomada_P) + ".txt";
        //fim = 3 /*minutos*/ * 60; //Definição do novo tempo para finalizar a média temporal
        }
       }
      }
     }
    }

// Se der erro no cartão lá em cima...
}else{
    Serial.println("Erro ao criar arquivo");
    while(1);
    }
}

/*================================================ Definição de média móvel =========================================*/

long moving_average(){

for (int i = n-1; i > 0; i--) num[i] = num[i-1];

num[0] = adc0;

long acc = 0;

for (int i = 0; i < n; i++) acc += num[i];

return acc/n;
}

/*==================================================================================================================*/

/*=============================================== Conexões =========================================================*/

/*

// Conversor ADC ADS1115

VDD   - 5V
GND   - GND
SCL   - 2 pin acima do AREF
SDA   - 1 pin acima do AREF
ADDR  - Por enquanto sem nada

Entradas analógicas.......


// Módulo micro SD

VCC   - 5V
GND   - GND
MISO  - 12
MOSI  - 11
SCK   - 13
CS    - 4


// Sensor de pressão

Entrada analógica no A0 do ADS1115

// Temperatura NTC

Direto na entrada analógica


*/

/*==================================================================================================================*/
