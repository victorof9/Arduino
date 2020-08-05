//Programa para encontrar fundo de escala para a pressão
float SensorOffset = 35;
float medida;

#define N 50

float filtro; //valor filtrado
float vals[N]; //array das medidas com N valores

void setup() {
Serial.begin(9600);

 medida = analogRead (A0) - SensorOffset;

Serial.print (medida);
Serial.print ('\t');
Serial.println (SensorOffset);

delay (200);
}
void loop() {

medida = analogRead(A0) - SensorOffset;

//Filtro média móvel
for(int i = N - 1; i > 0; i--){
    vals[i] = vals[i-1];
  }
   vals[0] = medida;
  int sum = 0;
  for(int i = 0; i < N; i++){
    sum = sum + vals[i];
  }
  
  filtro = sum / N;

if (filtro > 0){
  SensorOffset = SensorOffset + 1;
  } else if (medida < 0){
    SensorOffset = SensorOffset - 1;
  }

Serial.print (filtro);
Serial.print ('\t');
Serial.println (SensorOffset);

delay (200);
}
