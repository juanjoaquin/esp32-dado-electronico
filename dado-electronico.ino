/*
  Dado electrónico táctil para ESP32
  - Toca el pin GPIO4 (T0) para lanzar el dado
  - Muestra el resultado y estadísticas de cada número
  Monitor serie: 115200 baudios
*/

const int PIN_TOUCH = 4;           // GPIO4 = T0
const unsigned long ANTIREBOTE_MS = 600;

int umbral = 0;
int conteo[6] = {0, 0, 0, 0, 0, 0};
int totalLanzamientos = 0;
unsigned long ultimoToque = 0;

void dibujarDado(int n) {
  // Cada fila del dado: 3 posiciones (izq, centro, der)
  const bool caras[6][9] = {
    {0,0,0, 0,1,0, 0,0,0},  // 1
    {1,0,0, 0,0,0, 0,0,1},  // 2
    {1,0,0, 0,1,0, 0,0,1},  // 3
    {1,0,1, 0,0,0, 1,0,1},  // 4
    {1,0,1, 0,1,0, 1,0,1},  // 5
    {1,0,1, 1,0,1, 1,0,1}   // 6
  };

  Serial.println("  +-------+");
  for (int fila = 0; fila < 3; fila++) {
    Serial.print("  | ");
    for (int col = 0; col < 3; col++) {
      Serial.print(caras[n - 1][fila * 3 + col] ? "o" : " ");
      if (col < 2) Serial.print(" ");
    }
    Serial.println(" |");
  }
  Serial.println("  +-------+");
}

void mostrarEstadisticas() {
  Serial.println("\n  Estadisticas:");
  for (int i = 0; i < 6; i++) {
    Serial.printf("  %d: ", i + 1);
    for (int j = 0; j < conteo[i]; j++) Serial.print("#");
    Serial.printf(" (%d)\n", conteo[i]);
  }
  Serial.printf("  Total: %d lanzamientos\n", totalLanzamientos);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Dado electronico tactil ===");
  Serial.println("Calibrando... no toques el pin GPIO4");
  delay(1000);

  // Calibración: promedio de lecturas sin tocar
  long suma = 0;
  for (int i = 0; i < 50; i++) {
    suma += touchRead(PIN_TOUCH);
    delay(20);
  }
  int base = suma / 50;
  umbral = base * 0.6;  // al tocar, el valor baja en el ESP32 clasico

  Serial.printf("Valor base: %d | Umbral: %d\n", base, umbral);
  Serial.println("Listo! Toca el pin para lanzar el dado.\n");
}

void loop() {
  int lectura = touchRead(PIN_TOUCH);

  if (lectura < umbral && millis() - ultimoToque > ANTIREBOTE_MS) {
    ultimoToque = millis();

    Serial.println("Lanzando...");
    for (int i = 0; i < 5; i++) {   // pequeña "animación"
      Serial.print(".");
      delay(150);
    }
    Serial.println();

    int resultado = (esp_random() % 6) + 1;
    conteo[resultado - 1]++;
    totalLanzamientos++;

    Serial.printf("\n  Salio el %d!\n", resultado);
    dibujarDado(resultado);
    mostrarEstadisticas();
    Serial.println("\n----------------------------");
  }

  delay(50);
}