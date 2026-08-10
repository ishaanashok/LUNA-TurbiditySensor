// --- Pin Configurations (Matched to your ESP32 Sheet) ---
const int emitterPin = 2;    // GPIO2 -> Controls the PN2222 transistor
const int receiverPin = 36;  // GPIO36 / ADC0 -> Reads the phototransistor voltage divider

// --- Sampling Configuration ---
const int numReadings = 1000;      // Fast internal cycles per macro-reading
const int numStandards = 3;        // 4 distinct NTU calibration liquids
const int entriesPerStandard = 6;  // 6 macro-entries taken per liquid standard

float rawSignals[numStandards];    // Cleaned X values for final regression
float targetNTUs[numStandards];    // Y values (Target NTUs)

// --- Active Calibration Coefficients (Chosen by the Auto-Selector) ---
int activeModel = 0;               // 0 = Linear, 1 = Quadratic Polynomial
float lin_a = 0.25, lin_b = 0.00;                 // Linear parameters (y = ax + b)
float poly_a = 0.00, poly_b = 0.25, poly_c = 0.00; // Quadratic parameters (y = ax^2 + bx + c)

// Helper function to capture a single "Net Signal" reading (LED On - LED Off)
float captureNetSignal() {
  long sumLEDOn = 0;
  long sumLEDOff = 0;

  // 1. Measure Ambient Background (LED Off)
  digitalWrite(emitterPin, LOW);
  delay(5); // 5ms settling time for 2MΩ line
  for (int i = 0; i < numReadings; i++) {
    sumLEDOff += analogRead(receiverPin);
  }

  // 2. Measure Total Light (LED On)
  digitalWrite(emitterPin, HIGH);
  delay(5); // 5ms rise time for 2MΩ line
  for (int i = 0; i < numReadings; i++) {
    sumLEDOn += analogRead(receiverPin);
  }
  digitalWrite(emitterPin, LOW); // Turn off LED immediately

  float avgLEDOff = (float)sumLEDOff / numReadings;
  float avgLEDOn = (float)sumLEDOn / numReadings;
  
  return (avgLEDOn - avgLEDOff);
}

// Helper function to pause execution and wait for user numeric input
float waitForUserNTU() {
  while (Serial.available() == 0) {
    delay(50);
  }
  float ntu = Serial.parseFloat();
  while (Serial.available() > 0) {
    Serial.read(); // Clear buffer
  }
  return ntu;
}

// Statistical IQR Filter: Sorts 6 entries, removes outliers, and averages the rest
float filterOutliersAndAverage(float samples[], int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (samples[j] > samples[j + 1]) {
        float temp = samples[j];
        samples[j] = samples[j + 1];
        samples[j + 1] = temp;
      }
    }
  }

  float q1 = samples[1]; // 25th percentile
  float q3 = samples[4]; // 75th percentile
  float iqr = q3 - q1;

  if (iqr < 0.1) {
    float sum = 0;
    for (int i = 0; i < n; i++) sum += samples[i];
    return sum / n;
  }

  float lowerBound = q1 - 1.5 * iqr;
  float upperBound = q3 + 1.5 * iqr;
  float sumClean = 0;
  int cleanCount = 0;

  for (int i = 0; i < n; i++) {
    if (samples[i] >= lowerBound && samples[i] <= upperBound) {
      sumClean += samples[i];
      cleanCount++;
    } else {
      Serial.print("  ⚠️ Statistical Outlier Detected & Removed: ");
      Serial.println(samples[i], 2);
    }
  }

  if (cleanCount == 0) return samples[2]; 
  return sumClean / cleanCount;
}

// Helper function to calculate 3x3 Determinant (used in Cramer's Rule)
float det3x3(float m[3][3]) {
  return m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1]) -
         m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
         m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
}

// Fits a 2nd-Order Polynomial (y = ax^2 + bx + c) and calculates R-squared
float fitQuadraticPolynomial(float x[], float y[], int n, float &a, float &b, float &c) {
  float sumX = 0, sumY = 0, sumX2 = 0, sumX3 = 0, sumX4 = 0, sumXY = 0, sumX2Y = 0;
  for (int i = 0; i < n; i++) {
    float x2 = x[i] * x[i];
    sumX += x[i];
    sumY += y[i];
    sumX2 += x2;
    sumX3 += x2 * x[i];
    sumX4 += x2 * x2;
    sumXY += x[i] * y[i];
    sumX2Y += x2 * y[i];
  }

  // Construct the matrices for Cramer's Rule
  float D[3][3] = {
    { sumX4, sumX3, sumX2 },
    { sumX3, sumX2, sumX  },
    { sumX2, sumX,  (float)n }
  };
  
  float Da[3][3] = {
    { sumX2Y, sumX3, sumX2 },
    { sumXY,  sumX2, sumX  },
    { sumY,   sumX,  (float)n }
  };

  float Db[3][3] = {
    { sumX4, sumX2Y, sumX2 },
    { sumX3, sumXY,  sumX  },
    { sumX2, sumY,   (float)n }
  };

  float Dc[3][3] = {
    { sumX4, sumX3, sumX2Y },
    { sumX3, sumX2, sumXY  },
    { sumX2, sumX,  sumY   }
  };

  float detD = det3x3(D);
  if (abs(detD) > 0.001) {
    a = det3x3(Da) / detD;
    b = det3x3(Db) / detD;
    c = det3x3(Dc) / detD;
  } else {
    a = 0.0; b = 0.25; c = 0.0; // Fallback
  }

  // Calculate R-squared for the polynomial curve
  float meanY = sumY / n;
  float ssTot = 0, ssRes = 0;
  for (int i = 0; i < n; i++) {
    ssTot += (y[i] - meanY) * (y[i] - meanY);
    float predictedY = a*x[i]*x[i] + b*x[i] + c;
    ssRes += (y[i] - predictedY) * (y[i] - predictedY);
  }
  return (ssTot > 0) ? (1.0 - (ssRes / ssTot)) : 0.0;
}

// Fits a Linear model (y = ax + b) and calculates R-squared
float fitLinearModel(float x[], float y[], int n, float &a, float &b) {
  float sumX = 0, sumY = 0;
  for (int i = 0; i < n; i++) {
    sumX += x[i];
    sumY += y[i];
  }
  float meanX = sumX / n;
  float meanY = sumY / n;

  float numerator = 0;
  float denominator = 0;
  for (int i = 0; i < n; i++) {
    numerator += (x[i] - meanX) * (y[i] - meanY);
    denominator += (x[i] - meanX) * (x[i] - meanX);
  }

  if (abs(denominator) > 0.001) {
    a = numerator / denominator;
    b = meanY - (a * meanX);
  } else {
    a = 0.25;
    b = 0.00;
  }

  // Calculate R-squared for the linear curve
  float ssTot = 0, ssRes = 0;
  for (int i = 0; i < n; i++) {
    ssTot += (y[i] - meanY) * (y[i] - meanY);
    float predictedY = a*x[i] + b;
    ssRes += (y[i] - predictedY) * (y[i] - predictedY);
  }
  return (ssTot > 0) ? (1.0 - (ssRes / ssTot)) : 0.0;
}

void setup() {
  pinMode(emitterPin, OUTPUT);
  digitalWrite(emitterPin, LOW);

  Serial.begin(115200);
  while (!Serial) { ; }

  Serial.println("\n==============================================");
  Serial.println("   ESP32 Bioretention Monitor - Multi-Curve   ");
  Serial.println("       Dynamic Model Selection Active         ");
  Serial.println("==============================================");

  // --- RUNTIME CALIBRATION SEQUENCE ---
  for (int stepNum = 0; stepNum < numStandards; stepNum++) {
    Serial.print("\n[CALIBRATION LIQUID STANDARD ");
    Serial.print(stepNum + 1);
    Serial.println(" of 4]");
    Serial.println("-> Place the probe into the standard bottle.");
    Serial.println("-> Type the target NTU value and press Enter...");

    targetNTUs[stepNum] = waitForUserNTU();
    
    Serial.println("Stabilizing liquid... Hold perfectly still...");
    delay(3000);
    
    Serial.println("Capturing 6 consecutive data entries...");
    float rawEntries[entriesPerStandard];
    
    for (int entryNum = 0; entryNum < entriesPerStandard; entryNum++) {
      rawEntries[entryNum] = captureNetSignal();
      Serial.print("  Entry "); 
      Serial.print(entryNum + 1); 
      Serial.print(": Raw Net Signal = "); 
      Serial.println(rawEntries[entryNum], 2);
      delay(2000); // 2-second spacing between macro-entries
    }

    rawSignals[stepNum] = filterOutliersAndAverage(rawEntries, entriesPerStandard);

    Serial.print(">> Final Cleaned Average Signal for ");
    Serial.print(targetNTUs[stepNum], 2);
    Serial.print(" NTU is: ");
    Serial.println(rawSignals[stepNum], 2);
    
    delay(3000); 
  }

  // --- DYNAMIC CURVE ANALYSIS ENGINE ---
  Serial.println("\nCalculating regression models...");
  
  float rSquared_linear = fitLinearModel(rawSignals, targetNTUs, numStandards, lin_a, lin_b);
  float rSquared_poly = fitQuadraticPolynomial(rawSignals, targetNTUs, numStandards, poly_a, poly_b, poly_c);

  Serial.println("\n==============================================");
  Serial.println("             REGRESSION EVALUATION            ");
  Serial.println("==============================================");
  Serial.print("1. Linear Model (y = ax + b):\n");
  Serial.print("   -> Formula: y = ("); Serial.print(lin_a, 6); Serial.print(")x + ("); Serial.print(lin_b, 4); Serial.println(")");
  Serial.print("   -> R-squared (fit): "); Serial.println(rSquared_linear, 6);
  
  Serial.print("\n2. Quadratic Polynomial (y = ax^2 + bx + c):\n");
  Serial.print("   -> Formula: y = ("); Serial.print(poly_a, 8); Serial.print(")x^2 + ("); Serial.print(poly_b, 6); Serial.print(")x + ("); Serial.print(poly_c, 4); Serial.println(")");
  Serial.print("   -> R-squared (fit): "); Serial.println(rSquared_poly, 6);
  Serial.println("----------------------------------------------");

  // Determine the best fitting mathematical model
  if (rSquared_poly > rSquared_linear && rSquared_poly <= 1.0) {
    activeModel = 1; // Set active model to Polynomial
    Serial.println(">>> SELECTED MODEL: 2nd-Order Polynomial Curve (Better Fit) <<<");
  } else {
    activeModel = 0; // Set active model to Linear
    Serial.println(">>> SELECTED MODEL: Linear Model (Better Fit) <<<");
  }
  Serial.println("==============================================\n");
  
  delay(5000);
}

void loop() {
  float netSignal = captureNetSignal();
  float cleanSignal = netSignal < 0 ? 0.00 : netSignal;
  float calculatedNTU = 0.00;

  // Execute the mathematical calculation based on the auto-selected model
  if (activeModel == 1) {
    // Quadratic: y = ax^2 + bx + c
    calculatedNTU = (poly_a * cleanSignal * cleanSignal) + (poly_b * cleanSignal) + poly_c;
  } else {
    // Linear: y = ax + b
    calculatedNTU = (lin_a * cleanSignal) + lin_b;
  }

  // Zero-bound check to prevent physically impossible negative NTU values
  if (calculatedNTU < 0) {
    calculatedNTU = 0.00;
  }

  // Print live data stream
  Serial.print("Net Signal: ");
  Serial.print(netSignal, 2);
  Serial.print("  |  Active Model: ");
  Serial.print(activeModel == 1 ? "Quadratic Poly" : "Linear");
  Serial.print("  |  Live Turbidity: ");
  Serial.print(calculatedNTU, 2);
  Serial.println(" NTU");

  delay(2000);
}