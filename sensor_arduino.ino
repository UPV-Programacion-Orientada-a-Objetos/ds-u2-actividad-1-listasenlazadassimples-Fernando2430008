// codigo para arduino - simula sensores y envia datos por serial
// subir este codigo al arduino usando el arduino ide

void setup() {
  // inicializar comunicacion serial a 9600 baud
  Serial.begin(9600);
  
  // inicializar generador aleatorio
  randomSeed(analogRead(0));
  
  // esperar a que se abra el puerto serial
  while (!Serial) {
    ;
  }
  
  Serial.println("arduino listo");
  delay(1000);
}

void loop() {
  // generar tipo de sensor aleatorio
  int tipoSensor = random(0, 2);
  
  if (tipoSensor == 0) {
    // sensor de temperatura
    float temp = random(150, 350) / 10.0;  // 15.0 a 35.0 grados
    Serial.print("TEMP,T-001,");
    Serial.println(temp);
  } else {
    // sensor de presion
    int presion = random(950, 1050);  // 950 a 1050 psi
    Serial.print("PRES,P-101,");
    Serial.println(presion);
  }
  
  // esperar 2 segundos antes de siguiente lectura
  delay(2000);
}
