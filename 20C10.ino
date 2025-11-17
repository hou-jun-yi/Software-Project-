// Arduino pin assignment
#define PIN_IR A0

float volt[20];
float dist[20];
int idx = 0;
int N = 0;

float A[10][10];
float B[10];
float X_coef[10];

float volt_to_distance(unsigned int a_value) {
  return (6762.0 / (a_value - 9) - 4.0) * 10.0;
}

// ---------------------------
// 안전한 중간값 필터 함수
// ---------------------------
unsigned int ir_sensor_filtered(unsigned int n, float position, int verbose)
{
  if (n == 0 || n > 200) return 0;
  if (position >= 1.0) position = 0.999;

  unsigned int ir_val[n];

  for (int i = 0; i < n; i++) {
    ir_val[i] = analogRead(PIN_IR);
  }

  // simple insertion sort (n max 200 → OK on Arduino)
  for (int i = 1; i < n; i++) {
    unsigned int key = ir_val[i];
    int j = i - 1;
    while (j >= 0 && ir_val[j] > key) {
      ir_val[j + 1] = ir_val[j];
      j--;
    }
    ir_val[j + 1] = key;
  }

  return ir_val[(int)(n * position)];
}

// ---------------------------
// Gaussian elimination
// ---------------------------
void gaussian(int n) {

  for (int i = 0; i < n; i++) {

    if (A[i][i] == 0) {
      for (int r = i + 1; r < n; r++) {
        if (A[r][i] != 0) {
          for (int c = 0; c < n; c++) {
            float tmp = A[i][c];
            A[i][c] = A[r][c];
            A[r][c] = tmp;
          }
          float tmp2 = B[i];
          B[i] = B[r];
          B[r] = tmp2;
          break;
        }
      }
    }

    float pivot = A[i][i];
    if (pivot == 0) continue;

    float inv = 1.0 / pivot;
    for (int c = 0; c < n; c++) A[i][c] *= inv;
    B[i] *= inv;

    for (int r = i + 1; r < n; r++) {
      float factor = A[r][i];
      for (int c = 0; c < n; c++) {
        A[r][c] -= factor * A[i][c];
      }
      B[r] -= factor * B[i];
    }
  }

  for (int i = n - 1; i >= 0; i--) {
    float sum = B[i];
    for (int c = i + 1; c < n; c++) sum -= A[i][c] * X_coef[c];
    X_coef[i] = sum;
  }
}

// ---------------------------
// Polynomial regression
// ---------------------------
void polynomialRegression(int degree) {
  int m = degree + 1;

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < m; j++) A[i][j] = 0;
    B[i] = 0;
  }

  float S[20] = {0};

  // Compute Σ(v^k)
  for (int k = 0; k <= 2 * degree; k++) {
    float s = 0;
    for (int i = 0; i < N; i++) {
      s += pow(volt[i], k);
    }
    S[k] = s;
  }

  for (int r = 0; r < m; r++)
    for (int c = 0; c < m; c++)
      A[r][c] = S[r + c];

  for (int r = 0; r < m; r++) {
    float s = 0;
    for (int i = 0; i < N; i++)
      s += dist[i] * pow(volt[i], r);
    B[r] = s;
  }

  gaussian(m);
}

// ----------------------------------------
// MAIN PROGRAM
// ----------------------------------------
void setup()
{
  Serial.begin(1000000);
  delay(500);

  Serial.print("Enter Polynomial degree (1~5): ");
  while (!Serial.available());
  int degree = Serial.parseInt();
  degree = constrain(degree, 1, 5);

  Serial.print("Selected Polynomial degree: ");
  Serial.println(degree);

  Serial.println("\nPress Enter after moving ball.\n");

  for (int d = 0; d <= 30; d += 5) {
    Serial.print(d);
    Serial.println("cm → Press Enter.");

    while (!Serial.available());
    Serial.read();

    unsigned int filtered = ir_sensor_filtered(50, 0.5, 0);

    volt[idx] = filtered;
    dist[idx] = d * 10.0;  // mm
    idx++;
    N++;

    Serial.print("ADC = ");
    Serial.println(filtered);
  }

  polynomialRegression(degree);

  Serial.print("distance = ");

  for (int i = 0; i <= degree; i++) {
    Serial.print(X_coef[i], 6);
    if (i > 0) Serial.print(" * v^");
    if (i > 0) Serial.print(i);
    if (i != degree) Serial.print(" + ");
  }
  Serial.println(";");
  Serial.println("Use above equation in volt_to_distance()");
}

void loop() {
}
