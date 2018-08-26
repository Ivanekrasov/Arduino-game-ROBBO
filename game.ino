/*
 * Учебный проект РОББО клуб
 * Программа для игровой платформы на Arduino
 * made by ivanekrasov
*/

#define butCount 3
#define buzzPin 11 
#define ledCount 9
#define playersCount 2

#define player1But 2   
#define player2But 3
#define systemBut 7
#define player1LED 12
#define player2LED 13
#define redRGB 4
#define greenRGB 5
#define blueRGB 6
#define redLED 8
#define yelLED 9
#define greenLED 10
#define blueLED 14

int buttonPins[butCount] = {player1But, systemBut, player2But};
int ledPins[ledCount] = {player1LED, redRGB, greenRGB, blueRGB, redLED, yelLED, greenLED, player2LED, blueLED};
int gameNum = 1;//отладка, надо вернуть в 0
//int players = 2;
//bool but1, but2, sysBut, gameInProgress = false;
bool last = LOW;
bool getInColor = false;
int gameScore = 0;
int needColor;

bool finished_game;
volatile bool started, finished;
volatile bool playerWon[playersCount] = {false, false};
volatile bool playerFailed[playersCount] = {false, false};

void handlePlayer1Press();
void handlePlayer2Press();
void handlePlayerPress(int player);
void resetGameData();
void catchColor();
void catchMaxVal();
bool allPlayersFailed();
void pressBut();


bool button_state  = false;
bool button_long_state = false;
uint32_t ms_button = 0;

void setup()
{
  pinMode(buzzPin, OUTPUT);
  for (int i = 0; i < ledCount; i++) pinMode(ledPins[i], OUTPUT);
  for (int i = 0; i < butCount; i++) pinMode(buttonPins[i], INPUT_PULLUP);
  for (int player = 0; player < playersCount; ++player) {
    attachInterrupt(INT0, handlePlayer1Press, FALLING );
    attachInterrupt(INT1, handlePlayer2Press, FALLING );
  }
  Serial.begin(9600);
}

void handlePlayer1Press() { handlePlayerPress(0); }
void handlePlayer2Press() { handlePlayerPress(1); }

void loop()
{
  pressBut();
  resetGameData(); 
  catchColor();
  /*if(button_long_state){ //работает, не удалять
    Serial.print("Game number is ");
    Serial.println(gameNum);
    switch(gameNum){
      case(0):
        theQuickest();
        gameNum++;
        break;
      case(1):
        catchColor();
        gameNum++;
        break;
      case(2):
        catchMaxVal();
        gameNum++;
        break;
      case(3):
        gameNum = 0; 
        break;  
    }
    button_long_state = false;
    button_state = false;
  }*/

}

void theQuickest()
{
  button_long_state = false;
  button_state = false;
  finished_game = false;
  resetGameData();
  
  for (int i = 4; i < 7; i++)
  {
   digitalWrite(ledPins[i], HIGH);
   while (i != 6)
   {
    tone(buzzPin, 3000, 500); // 3 кГц, 250 мс
    delay(1000);
    digitalWrite(ledPins[i], LOW);
    break;
   }
   if (i == 6) 
   {
    tone(buzzPin, 3000, 1000);
   }
  }

  started = true;
  finished = allPlayersFailed(); // Если оба игрока сделали фальстарт — игру нужно завершить.
  
  while(true){
    
    pressBut();
    Serial.print(playerFailed[1],playerFailed[2]);
    Serial.print("\n");
    if (finished_game || finished){
        winMelody();
        delay(4000);
        theQuickest();
        delay(4000);
    }
    if (button_long_state) break;
  }
} 

bool debounce (bool last, int button){
  bool current = digitalRead(button);
  if (last != current){
    delay(5);
    current = digitalRead(button);
  }
  return current;
}

void winMelody()
{
  for (int i = 0; i < 15; i++)
  { 
    tone(buzzPin, 3000, 100);
    delay(200);
  } 
}

void catchColor()
{
  gameScore = 0;
  int flashColor;
  resetGameData();
  Serial.println(needColor);
  while (true){
    needColor = random(4, 8);
    if (needColor != 8 && needColor != 7 && needColor != 5){
      digitalWrite(ledPins[needColor], HIGH);
      break;
    }
    else if (needColor == 8) { analogWrite(ledPins[needColor], 1023); break; }
  }
  
  while (true){
    flashColor = random(4, 8);
    if (flashColor == 7) {
      digitalWrite(ledPins[1], HIGH);
      digitalWrite(ledPins[2], HIGH);
      digitalWrite(ledPins[3], HIGH);
      delay(500);
      digitalWrite(ledPins[1], LOW);
      digitalWrite(ledPins[2], LOW);
      digitalWrite(ledPins[3], LOW);
      delay(500);
    }
    else {
    digitalWrite(flashColor, HIGH);
    delay(500);
    digitalWrite(flashColor, LOW);
    delay(500);
    }
    if (gameScore == 3){
      winMelody();
      delay(4000);
      catchColor();
      delay(4000);
    }
    if (button_long_state) break;
  }
}

void catchMaxVal()
{
  Serial.println("catchMaxVal");
}

void breakaway(int playerWon, int playerLost)
{
  digitalWrite(playerWon, HIGH);
  tone(buzzPin, 3000, 1000);
  while (true)
  {
    digitalWrite(playerLost, HIGH);
    //winMelody();
    delay(1000);
    digitalWrite(playerLost, LOW);
    delay(1000);
    if (!debounce(last, buttonPins[1])) break;
    
  }
  digitalWrite(playerWon, LOW);
}


void resetGameData() 
{
  started  = false;
  finished = false;
  for (int i = 0; i < playersCount; i++) {
    playerWon[i]    = false;
    playerFailed[i] = false;
  }
  for (int i = 0; i < ledCount; i++){
    digitalWrite(ledPins[i], LOW);
  }
}

bool allPlayersFailed() 
{
  bool anyoneNotFailed = false;
  for (int i = 0; i < playersCount; i++) {
    if (!playerFailed[i]) anyoneNotFailed = true;
      
  }
  return !anyoneNotFailed;
}


void handlePlayerPress(int player) {
  if (gameNum == 0)
  {
    if (started && !finished && !playerFailed[player])
    {
      finished = true;
      playerWon[player] = true;
      digitalWrite(player ? ledPins[7] : ledPins[0], HIGH);
    } 
    else if (!started && !finished && !playerFailed[player]) 
    {
      finished_game = true;
      playerFailed[player] = true;
      digitalWrite(player ? ledPins[0] : ledPins[7], HIGH);
    }
  }
  else if (gameNum == 1)
  {
    if (digitalRead(ledPins[4]) && digitalRead(ledPins[1])){//красный свет
      if (gameScore == 0){
        digitalWrite(ledPins[6], HIGH);
      }
      else if (gameScore == 1){
        digitalWrite(ledPins[5], HIGH);
      }
      else if (gameScore == 2){
        analogWrite(ledPins[8], 1023);
      }
      gameScore++;
    }
    else if (digitalRead(ledPins[3]) && (analogRead(ledPins[8]) >= 1000)){//синий свет
      if (gameScore == 0){
        digitalWrite(ledPins[6], HIGH);
      }
      else if (gameScore == 1){
        digitalWrite(ledPins[5], HIGH);
      }
      else if (gameScore == 2){
        digitalWrite(ledPins[4], HIGH);
      }
      gameScore++;
    }
    else if (digitalRead(ledPins[2]) && digitalRead(ledPins[6])){//зеленый свет
      if (gameScore == 0){
        digitalWrite(ledPins[5], HIGH);
      }
      else if (gameScore == 1){
        digitalWrite(ledPins[4], HIGH);
      }
      else if (gameScore == 2){
        analogWrite(ledPins[8], 1023);
      }
      gameScore++;
    }
    else {
      if (needColor == 4){
        if (gameScore == 0){
        gameScore--;
      }
      else if (gameScore == 1){
        digitalWrite(ledPins[5], LOW);
        gameScore--;
      }
      else if (gameScore == 2){
        analogWrite(ledPins[8], 0);
        gameScore--;
      }
      
    }
    if (needColor == 6){
        if (gameScore == 0){
        gameScore--;
      }
      else if (gameScore == 1){
        digitalWrite(ledPins[5], LOW);
        gameScore--;
      }
      else if (gameScore == 2){
        digitalWrite(ledPins[4], 0);
        gameScore--;
      }
    }
      if (needColor == 8){
        if (gameScore == 0){
        gameScore--;
      }
      else if (gameScore == 1){
        digitalWrite(ledPins[6], LOW);
        gameScore--;
      }
      else if (gameScore == 2){
        digitalWrite(ledPins[5], 0);
        gameScore--;
      }      
    }
  }
  }
}

void pressBut(){
   uint32_t ms    = millis();
   bool pin_state = digitalRead(buttonPins[1]);
// Фиксируем нажатие кнопки   
   if( pin_state  == LOW && !button_state && ( ms - ms_button ) > 50 ){
      button_state      = true;
      button_long_state = false;  
      ms_button         = ms;
      delay(1000);
   }
// Фиксируем длинное нажатие кнопки   
   if( pin_state  == LOW && !button_long_state && ( ms - ms_button ) > 2000 ){
      button_long_state = true;
      Serial.println("Long press key");    
   }
// Фиксируем отпускание кнопки   
   if( pin_state == HIGH && button_state && ( ms - ms_button ) > 50  ){
      button_state      = false;   
      ms_button         = ms;
      if( !button_long_state )Serial.println("Press key");
   }
}


