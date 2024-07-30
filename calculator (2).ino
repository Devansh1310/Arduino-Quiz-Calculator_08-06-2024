#include <Wire.h>               // Library for I2C communication
#include <LiquidCrystal_I2C.h>  // Library for LCD with I2C interface
#include <Keypad.h>             // Library for keypad input

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16 columns and 2 rows

const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // A is for addition
  { '4', '5', '6', 'B' },  // B is for subtraction
  { '7', '8', '9', 'C' },  // C is for multiplication
  { '*', '0', '#', 'D' }   // * is delete, # is equals, D is division
};

byte rowPins[ROWS] = { 9, 8, 7, 6 };  // Connect to the row
byte colPins[COLS] = { 5, 4, 3, 2 };  // Connect to the column

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String inputString = "";  // To store user input
char lastOperator;        // To store the last operator pressed (A, B, C, or D)
float num1, num2;         // To store the numbers for calculation
int randhigh, randlow;
int operation;  // the random number for the quiz
const int quiz = 10;
const int greenLED = 12;
const int redLED = 13;
int buttonstate = 0;
const int buzzer = 11;  // Buzzer pin

void setup() {
  pinMode(quiz, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Initialize the LCD
  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight

  lcd.setCursor(0, 0);  // Set the cursor to the first column, first row
  lcd.print("Calculator");
  tone(buzzer, 100);
  delay(200);  // Play a sound when a key is pressed
  noTone(buzzer);  // Print "Calculator"   
  delay(1000);  // Wait for 2 seconds

  lcd.clear();                 // Clear the display
  lcd.setCursor(0, 0);         // Set the cursor to the first column, first row
  lcd.print("Enter number:");  // Print "Enter number:"
  delay(1000);                 // Wait for 1 second
  lcd.clear();                 // Clear the display

  // Seed the random number generator
  randomSeed(analogRead(A0));
}

void loop() {
  buttonstate = digitalRead(quiz);
  if (buttonstate == HIGH) {
    tone(buzzer, 1000);  // Play a sound when the quiz button is pressed
    delay(200);
    noTone(buzzer);
    generateQuiz();
  }

  // Get the key from the keypad
  char key = keypad.getKey();

  // If a key is pressed
  if (key) {
    tone(buzzer, 500);  // Play a sound when a key is pressed
    delay(100);
    noTone(buzzer);

    // If the key is a number (0-9)
    if (key >= '0' && key <= '9') {
      inputString += key;      // Add the key to the input string
      lcd.setCursor(0, 1);     // Move to the second line of the LCD
      lcd.print(inputString);  // Display the input string
    }
    // If the key is an operator (A, B, C, D)
    else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
      num1 = inputString.toFloat();  // Convert the input string to a float number
      lastOperator = key;            // Store the operator
      inputString = "";              // Clear the input string
      lcd.clear();                   // Clear the LCD
      lcd.setCursor(0, 0);           // Set the cursor to the first column, first row
      lcd.print("Enter number:");    // Prompt for the second number
    }
    // If the key is the delete key (*)
    else if (key == '*') {
      inputString = "";            // Clear the input string
      lcd.clear();                 // Clear the LCD
      lcd.setCursor(0, 0);         // Set the cursor to the first column, first row
      lcd.print("Enter number:");  // Prompt for a new number
    }
    // If the key is the equals key (#)
    else if (key == '#') {
      if (inputString != "") {
        num2 = inputString.toFloat();  // Convert the input string to a float number
      }
      float result;  // To store the result of the calculation

      // Perform the calculation based on the last operator
      switch (lastOperator) {
        case 'A':
          result = num1 + num2;  // Addition
          break;
        case 'B':
          result = num1 - num2;  // Subtraction
          break;
        case 'C':
          result = num1 * num2;  // Multiplication
          break;
        case 'D':
          if (num2 != 0) {
            result = num1 / num2;  // Division
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Err: Div by 0");  // Error message for division by zero
            delay(1500);                 // Wait for 1.5 seconds
            lcd.clear();
            inputString = "";
            lcd.setCursor(0, 0);
            lcd.print("Enter number:");
            return;  // Exit the function to avoid further processing
          }
          break;
      }

      // Display the result
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Result:");  // Print "Result:"
      lcd.setCursor(0, 1);
      lcd.print(result);  // Print the result
      delay(3000);        // Wait for 3 seconds

      // Clear everything and prompt for new input
      lcd.clear();
      inputString = "";  // Clear the input string
      lcd.setCursor(0, 0);
      lcd.print("Enter number:");  // Prompt for a new number
    }
  }
}

void generateQuiz() {
  // Generate random numbers between 1 and 20
  randlow = random(1, 21);
  randhigh = random(1, 21);

  // Generate a random operation (1-4)
  operation = random(1, 5);

  // Assign an operation symbol based on the random operation
  char opSymbol;
  int correctAnswer;  // Changed to int to avoid decimals

  switch (operation) {
    case 1:
      opSymbol = '+';
      correctAnswer = randlow + randhigh;
      break;
    case 2:
      opSymbol = '-';
      // Ensure no negative results
      if (randlow < randhigh) {
        int temp = randlow;
        randlow = randhigh;
        randhigh = temp;
      }
      correctAnswer = randlow - randhigh;
      break;
    case 3:
      opSymbol = '*';
      correctAnswer = randlow * randhigh;
      break;
    case 4:
      opSymbol = '/';
      // Ensure no division by zero and result is an integer
      while (randlow % randhigh != 0 || randhigh == 1) {
        randlow = random(1, 21);
        randhigh = random(1, 21);
      }
      correctAnswer = randlow / randhigh;
      break;
  }

  // Display the quiz on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(randlow);
  lcd.print(" ");
  lcd.print(opSymbol);
  lcd.print(" ");
  lcd.print(randhigh);
  lcd.print(" = ?");
  inputString = "";  // Clear any previous input

  // Wait for user to input the answer
  while (true) {
    char key = keypad.getKey();
    if (key) {
      tone(buzzer, 500);  // Play a sound when a key is pressed
      delay(100);
      noTone(buzzer);

      if (key >= '0' && key <= '9') {
        inputString += key;
        lcd.setCursor(0, 1);
        lcd.print(inputString);
      } else if (key == '#') {                 // User confirms their answer
        int userAnswer = inputString.toInt();  // Changed to int
        if (userAnswer == correctAnswer) {
          digitalWrite(greenLED, HIGH);
          tone(buzzer, 1000);
          delay(1000);
          noTone(buzzer);
          digitalWrite(greenLED, LOW);
        } else {
          digitalWrite(redLED, HIGH);
          tone(buzzer, 200);
          delay(1000);
          noTone(buzzer);
          digitalWrite(redLED, LOW);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter number:");
        break;                  // Exit the quiz loop and go back to the main loop
      } else if (key == '*') {  // Allow the user to clear their input
        inputString = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");  // Clear the second line
        lcd.setCursor(0, 1);
      }
    }
  }
}
