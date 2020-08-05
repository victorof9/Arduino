void setup() {
  Serial.begin(9600);
  //Qual file deletar?
  String nome = "AQUI NOME FILE";
  Serial.println("Removing " + nome);
  SD.remove(nome);
}

void loop() {
  // put your main code here, to run repeatedly:

}
