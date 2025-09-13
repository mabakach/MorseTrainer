#include <LiquidCrystal_I2C.h>

#include <Wire.h>

const int DISPLAY_LENGTH = 20;
// initialize the liquid crystal library
// the first parameter is  the I2C address
// the second parameter is how many rows are on your screen
// the  third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27, DISPLAY_LENGTH, 4);

const int DASH_MIN_PRESSING_TIME_MS = 200;
const int CHAR_END_TIMEOUT_MS = 1000;
const int WORD_END_TIMEOUT_MS = 4000;

const int morseButtonPin = 3;
const int resetButtonPin = 2;
const int ledPin = 13;

int morseButtonState = 0;
int lastMorseButtonState = 0;
long millisLastMorseButtonPressStarted = -1;
long millisLastMorseButtonPressEnded = -1;

char currentMorseString[DISPLAY_LENGTH + 1];
char lastMorseString[DISPLAY_LENGTH + 1];
char currentDecodedString[DISPLAY_LENGTH + 1];
char clearString[DISPLAY_LENGTH + 1];

int resetButtonState = 0;

void (*resetFunc)(void) = 0;

void displayData();

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  currentMorseString[0] = '\0';
  lastMorseString[0] = '\0';
  currentDecodedString[0] = '\0';
  for (int i = 0; i < DISPLAY_LENGTH; i++){
    clearString[i] = ' ';
  }
  clearString[DISPLAY_LENGTH] = '\0';

  Serial.println("init LCD");
  // initialize lcd screen
  lcd.init();
  // turn on the backlight
  lcd.backlight();
  Serial.println("init LCD finished");

  // init buttons
  pinMode(morseButtonPin, INPUT);
  pinMode(resetButtonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  displayData();
}
void readButtonState() {
  morseButtonState = digitalRead(morseButtonPin);
  resetButtonState = digitalRead(resetButtonPin);
}

void displayData() {
  // tell the screen to write on line 0
  lcd.setCursor(0, 0);
  lcd.print("Morse Code:");
  // move to line 1
  lcd.setCursor(0, 1);
  // print the current morse text in . and -
  lcd.printstr(currentMorseString);
  // overwrite all characters behind currentMorseString
  lcd.printstr(clearString);
  // move to line 2
  lcd.setCursor(0, 2);
  lcd.print("Message: ");
  // move to line 3
  lcd.setCursor(0, 3);
  // print the current decoded string as readable text.
  lcd.printstr(currentDecodedString);
}

void updateLed() {
  if (morseButtonState == HIGH) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void updatePressingTimes() {
  if (morseButtonState != lastMorseButtonState) {
    if (morseButtonState == HIGH) {
      millisLastMorseButtonPressStarted = millis();
    } else {
      millisLastMorseButtonPressEnded = millis();
    }
    lastMorseButtonState = morseButtonState;
  }
}

void addCharToArray(char *arr, char c) {
  Serial.print("Add char to array: ");
  Serial.println(c);
  int length = strlen(arr);
  if (length < DISPLAY_LENGTH) {
    arr[length] = c;
    arr[length + 1] = '\0';
  } else {
    for (int i = 1; i < DISPLAY_LENGTH; i++) {
      arr[i - 1] = arr[i];
    }
    arr[DISPLAY_LENGTH - 1] = c;
    arr[DISPLAY_LENGTH] = '\0';
  }
}

void updateMorseString() {
  Serial.println("updateMorseString");
  if (morseButtonState == LOW) {
    // break detected
    if (millisLastMorseButtonPressStarted > 0 && millisLastMorseButtonPressEnded - millisLastMorseButtonPressStarted > DASH_MIN_PRESSING_TIME_MS) {
      // hypen
      addCharToArray(currentMorseString, '-');
    } else if (millisLastMorseButtonPressStarted > 0) {
      // dot
      addCharToArray(currentMorseString, '.');
    }
    millisLastMorseButtonPressStarted = -1;
  }
}

void decodeAndClearMorseString() {
  if (millis() - millisLastMorseButtonPressEnded > CHAR_END_TIMEOUT_MS && strcmp(currentMorseString, "") != 0) {
    // character is finished
    if (strcmp(currentMorseString, ".-") == 0) {
      addCharToArray(currentDecodedString, 'A');
    } else if (strcmp(currentMorseString, ".-.-") == 0) {
      addCharToArray(currentDecodedString, 'Ä');
    } else if (strcmp(currentMorseString, "-...") == 0) {
      addCharToArray(currentDecodedString, 'B');
    } else if (strcmp(currentMorseString, "-.-.") == 0) {
      addCharToArray(currentDecodedString, 'C');
    } else if (strcmp(currentMorseString, "-..") == 0) {
      addCharToArray(currentDecodedString, 'D');
    } else if (strcmp(currentMorseString, ".") == 0) {
      addCharToArray(currentDecodedString, 'E');
    } else if (strcmp(currentMorseString, "..-.") == 0) {
      addCharToArray(currentDecodedString, 'F');
    } else if (strcmp(currentMorseString, "--.") == 0) {
      addCharToArray(currentDecodedString, 'G');
    } else if (strcmp(currentMorseString, "....") == 0) {
      addCharToArray(currentDecodedString, 'H');
    } else if (strcmp(currentMorseString, "..") == 0) {
      addCharToArray(currentDecodedString, 'I');
    } else if (strcmp(currentMorseString, ".---") == 0) {
      addCharToArray(currentDecodedString, 'J');
    } else if (strcmp(currentMorseString, "-.-") == 0) {
      addCharToArray(currentDecodedString, 'K');
    } else if (strcmp(currentMorseString, ".-..") == 0) {
      addCharToArray(currentDecodedString, 'L');
    } else if (strcmp(currentMorseString, "--") == 0) {
      addCharToArray(currentDecodedString, 'M');
    } else if (strcmp(currentMorseString, "-.") == 0) {
      addCharToArray(currentDecodedString, 'N');
    } else if (strcmp(currentMorseString, "---") == 0) {
      addCharToArray(currentDecodedString, 'O');
    } else if (strcmp(currentMorseString, "---.") == 0) {
      addCharToArray(currentDecodedString, 'Ö');
    } else if (strcmp(currentMorseString, ".--.") == 0) {
      addCharToArray(currentDecodedString, 'P');
    } else if (strcmp(currentMorseString, "--.-") == 0) {
      addCharToArray(currentDecodedString, 'Q');
    } else if (strcmp(currentMorseString, ".-.") == 0) {
      addCharToArray(currentDecodedString, 'R');
    } else if (strcmp(currentMorseString, "...") == 0) {
      addCharToArray(currentDecodedString, 'S');
    } else if (strcmp(currentMorseString, "-") == 0) {
      addCharToArray(currentDecodedString, 'T');
    } else if (strcmp(currentMorseString, "..-") == 0) {
      addCharToArray(currentDecodedString, 'U');
    } else if (strcmp(currentMorseString, "..--") == 0) {
      addCharToArray(currentDecodedString, 'Ü');
    } else if (strcmp(currentMorseString, "...-") == 0) {
      addCharToArray(currentDecodedString, 'V');
    } else if (strcmp(currentMorseString, ".--") == 0) {
      addCharToArray(currentDecodedString, 'W');
    } else if (strcmp(currentMorseString, "-..-") == 0) {
      addCharToArray(currentDecodedString, 'X');
    } else if (strcmp(currentMorseString, "-.--") == 0) {
      addCharToArray(currentDecodedString, 'Y');
    } else if (strcmp(currentMorseString, "--..") == 0) {
      addCharToArray(currentDecodedString, 'Z');
    } else if (strcmp(currentMorseString, "-----") == 0) {
      addCharToArray(currentDecodedString, '0');
    } else if (strcmp(currentMorseString, ".----") == 0) {
      addCharToArray(currentDecodedString, '1');
    } else if (strcmp(currentMorseString, "..---") == 0) {
      addCharToArray(currentDecodedString, '2');
    } else if (strcmp(currentMorseString, "...--") == 0) {
      addCharToArray(currentDecodedString, '3');
    } else if (strcmp(currentMorseString, "....-") == 0) {
      addCharToArray(currentDecodedString, '4');
    } else if (strcmp(currentMorseString, ".....") == 0) {
      addCharToArray(currentDecodedString, '5');
    } else if (strcmp(currentMorseString, "-....") == 0) {
      addCharToArray(currentDecodedString, '6');
    } else if (strcmp(currentMorseString, "--...") == 0) {
      addCharToArray(currentDecodedString, '7');
    } else if (strcmp(currentMorseString, "---..") == 0) {
      addCharToArray(currentDecodedString, '8');
    } else if (strcmp(currentMorseString, "----.") == 0) {
      addCharToArray(currentDecodedString, '9');
    } else if (strcmp(currentMorseString, "--..--") == 0) {
      addCharToArray(currentDecodedString, ',');
    } else if (strcmp(currentMorseString, "-.-.--") == 0) {
      addCharToArray(currentDecodedString, '!');
    } else if (strcmp(currentMorseString, ".-.-.-") == 0) {
      addCharToArray(currentDecodedString, '.');
    }

    currentMorseString[0] = '\0';
  }
  if (millisLastMorseButtonPressEnded > 0 && millis() - millisLastMorseButtonPressEnded > WORD_END_TIMEOUT_MS) {
    // word finished
    addCharToArray(currentDecodedString, '_');
    millisLastMorseButtonPressEnded = -1;
  }
}

void loop() {
  for (int i = 0; i < 100; i++) {
    readButtonState();
    updateLed();
    updatePressingTimes();
    updateMorseString();
    decodeAndClearMorseString();
    if (resetButtonState == HIGH) {
      resetFunc();
    }
    if (strcmp(currentMorseString, lastMorseString) != 0){
      displayData();
      strncpy(lastMorseString, currentMorseString, DISPLAY_LENGTH);
    }
  }
  displayData();
}
