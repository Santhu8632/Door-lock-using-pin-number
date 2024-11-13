//Santhosh Kumar K R
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Define keypad layout
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
//byte rowPins[ROWS] = {9, 8, 7, 6};    // Connect to the row pins of the keypad
//byte colPins[COLS] = {5, 4, 3};       // Connect to the column pins of the keypad

byte rowPins[ROWS] = {8, 7, 6, 5};    // Connect to the row pins of the keypad
byte colPins[COLS] = {4, 3, 2};       // Connect to the column pins of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String password = "1234";  // Set your password here
String inputCode = "";     // To store the entered code
int accessCount = 0;       // Counter for successful access attempts

SoftwareSerial mySerial(10, 11);        // RX, TX for DFPlayer Mini
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);                 // Set serial baud rate for DFPlayer Mini
  pinMode(A0, OUTPUT);                  // Pin for the first action (e.g., lock)
  pinMode(A1, OUTPUT);                  // Pin for the second action (e.g., unlock)
  digitalWrite(A0, LOW);                // Start with pin A0 LOW (locked)
  digitalWrite(A1, LOW);                // Start with pin A1 LOW (unlocked)

  if (!myDFPlayer.begin(mySerial)) {    // Check if DFPlayer Mini is connected
    Serial.println("Unable to begin DFPlayer Mini.");
    while (true);
  }
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);                // Set volume to 20 (0-30 range)
}

void playLockSound() {
  myDFPlayer.play(3);  // Play the sound file "0001.mp3" for lock
}

void playUnlockSound() {
  myDFPlayer.play(1);  // Play the sound file "0002.mp3" for unlock
}

void WrongPassword() {
  myDFPlayer.play(2);  // Play the sound file "0002.mp3" for unlock
}

void loop() {
  char key = keypad.getKey(); // Read the key

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key == '*') {         // Reset the input if '*' is pressed
      inputCode = "";
      Serial.println("Input reset.");
    } else if (key == '#') {  // Check the code if '#' is pressed
      if (inputCode == password) {
        Serial.println("Access Granted");

        if (accessCount == 0) {
          digitalWrite(A0, HIGH);       // Activate lock action on pin A0
          playLockSound();              // Play lock sound
          delay(1000);
          digitalWrite(A0, LOW);        // Turn off pin A0 after delay
          accessCount++;                // Increment counter
        } else if (accessCount == 1) {
          digitalWrite(A1, HIGH);       // Activate unlock action on pin A1
          playUnlockSound();            // Play unlock sound
          delay(1000);
          digitalWrite(A1, LOW);        // Turn off pin A1 after delay
          accessCount = 0;              // Reset counter
        }
      } else {
        Serial.println("Access Denied");
        WrongPassword(); 
      }
      inputCode = "";  // Clear input after each attempt
    } else {
      inputCode += key; // Add pressed key to the input code
    }
  }
}
