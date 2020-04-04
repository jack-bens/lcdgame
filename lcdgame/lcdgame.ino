// Simple LCD screen + 1-button obstacle dodging game
// 4/4/20

#include <LiquidCrystal.h>

// CHANGE THESE VARIABLES TO MODIFY DIFFICULTY:
///////////////////////////////////////////////
// Time in ms for each step (Determines game pace)
const int STEP = 500;
// Number of obstacles 
const int OBS = 15;
// Minimum steps between each obstacle spawn
const int SPAWNRATE = 2;
///////////////////////////////////////////////

// Custom characters
byte player[8] = {
  0b00001,
  0b10011,
  0b01010,
  0b11100,
  0b11100,
  0b01010,
  0b10011,
  0b00001
};
byte obstacle[8] = {
  0b11100,
  0b10110,
  0b10011,
  0b11110,
  0b11110,
  0b10011,
  0b10110,
  0b11100
};
byte collision[8] = {
  0b10001,
  0b01010,
  0b10101,
  0b01010,
  0b01010,
  0b10101,
  0b01010,
  0b10001
};

// Time for delay at start
const int WAIT = 3000;
// Initialize player position
int pos = 1;
// Grid of obstacle positions on screen
int obstaclePos[OBS][2];
// Initialize step timer and spawn delay timer
int stepTimer = 0;
int spawnTimer = 0;
// Initialize variable to keep track of previous spawn
int prevY = 0;

bool running = true;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 7;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int buttonState;
int lastButtonState = LOW;

void setup() {
  // Create custom characters
  lcd.createChar(0, player);
  lcd.createChar(1, obstacle);
  lcd.createChar(2, collision);
  
  pinMode(buttonPin, INPUT);
  lcd.begin(16, 2);

  // Initialize obstacle positions
  for (int i = 0; i < OBS; i++) {
    for (int j = 0; j < 2; j++) {
      obstaclePos[i][j] = 16;
    }
  }

  lcd.setCursor(0,0);
  lcd.print("Press button to");
  lcd.setCursor(0,1);
  lcd.print("dodge obstacles!");
  delay(WAIT);
  lcd.clear();
}

void loop() {
  // On button press, change player position
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH && running) {
        lcd.setCursor(15,0);
        if (pos == 0) {
          pos = 1;
          lcd.write(" ");
          lcd.setCursor(15,1);
          lcd.write((uint8_t)0);
        }
        else {
          pos = 0;
          lcd.write((uint8_t)0);
          lcd.setCursor(15,1);
          lcd.write(" ");
        }
      }
    }
  }
  lastButtonState = reading;

  unsigned long currentTime = millis();

  // On step, update and print obstacle positions
  if (running && currentTime - stepTimer >= STEP) {
    // Clear screen
    lcd.clear();
    
    // Print player character
    lcd.setCursor(15, pos);
    lcd.write((uint8_t)0);

    //if (moveTimer >= SPEED) {
    for (int i = 0; i < OBS; i++) {
      if (obstaclePos[i][0] >= 16) {
        if (spawnTimer >= SPAWNRATE) {
          // RNG decides whether to spawn a new obstacle
          int sp = rand() % 3;
          if (sp > 0) {
            obstaclePos[i][0] = 0;
            // Random y position (bias based on previous spawn)
            int newY = (rand() % 3) % 2;
            if (prevY == 0) {
              obstaclePos[i][1] = (newY + 1) % 2;
            }
            else {
              obstaclePos[i][1] = newY;
            }
            prevY = obstaclePos[i][1];
            
          }
          spawnTimer = 0;
        }
      }
      else {
        obstaclePos[i][0]++;
      }
      // Check for player collision with obstacle
      if (obstaclePos[i][0] == 15 && obstaclePos[i][1] == pos) {
        lcd.setCursor(obstaclePos[i][0], obstaclePos[i][1]);
        lcd.write((uint8_t)2);
        running = false;
        break;
      }
      else if (obstaclePos[i][0] < 16) {
        lcd.setCursor(obstaclePos[i][0], obstaclePos[i][1]);
        lcd.write((uint8_t)1);
      }
    }
    // Increment spawn delay timer
    spawnTimer++;
    stepTimer = currentTime;
  }
  else if (!running) {
    delay(1000);
    // After collision, press F to pay respects
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("F");
    delay(10000);
  }
}
