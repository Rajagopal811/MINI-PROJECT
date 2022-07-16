#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

#define RST_PIN 5 // Configurable,see typical pin layout above
#define SS_PIN 53 
const int buzzer=24;
const int led=27;
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(12, 11, 9, 8, 7, 6);
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {36, 37, 38, 39}; 
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
int rely=22;
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(rely, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led,OUTPUT);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("Read personal data on a MIFARE PICC:"));
}

void loop() {
  digitalWrite(rely, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Place your tag");
  delay(2000);
  //  R  F   I    D    CODE
     MFRC522::MIFARE_Key key;
     for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
     //some variables we need
     byte block;
     byte len;
     MFRC522::StatusCode status;
     //-------------------------------------------
     // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle 
     if ( ! mfrc522.PICC_IsNewCardPresent()) {
      //Serial.println("NO Card");
     return;
     }
     // Select one of the cards
     if ( ! mfrc522.PICC_ReadCardSerial()) {
     return;
     }
     Serial.println(F("**Card Detected:**"));

     mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));  //dump some details about the card
     //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));     //uncomment this to see all blocks in hex
     //-------------------------------------------
     Serial.print(F("Customer Name: "));
     byte buffer1[18];
     block = 4;
     len = 18;

     
    //------------------------------------------- GET FIRST NAME
     status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4,&key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
     if (status != MFRC522::STATUS_OK) {
     Serial.print(F("Authentication failed: "));
     Serial.println(mfrc522.GetStatusCodeName(status)); 
     return;
     }
     status = mfrc522.MIFARE_Read(block, buffer1, &len);
     if (status != MFRC522::STATUS_OK) {
     Serial.print(F("Reading failed: "));
     Serial.println(mfrc522.GetStatusCodeName(status)); return;
     }
     //PRINT FIRST NAME
     for (uint8_t i = 0; i < 16; i++){
     if (buffer1[i] != 32){
     Serial.write(buffer1[i]);
     } }
     Serial.print(" ");
     //---------------------------------------- GET LAST NAME
     byte buffer2[18];
     block = 1;
     status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1,
    &key, &(mfrc522.uid)); //line 834
     if (status != MFRC522::STATUS_OK) {
     Serial.print(F("Authentication failed: "));
     Serial.println(mfrc522.GetStatusCodeName(status)); 
     return;
     }
     status = mfrc522.MIFARE_Read(block, buffer2, &len);
     if (status != MFRC522::STATUS_OK) {
     Serial.print(F("Reading failed: "));
     Serial.println(mfrc522.GetStatusCodeName(status)); 
     return;
     }
     
     //PRINT LAST NAME
     for (uint8_t i = 0; i < 16; i++) {
     Serial.write(buffer2[i] );
     }
     // . . . . . . .GET. .. ACCOUNT . . . BALANCE...
     //. . . . .6 6 6 6 7 7 7 . . . . . .
     // . . . . . .
     Serial.println();
     Serial.print(F("Account Balance : "));
     byte buffer3[18];
     block = 8;
     len = 18;
     status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 8,&key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
     if (status != MFRC522::STATUS_OK) {
     Serial.print(F("Authentication failed: "));
     Serial.println(mfrc522.GetStatusCodeName(status)); 
     return;
     }
     status = mfrc522.MIFARE_Read(block, buffer3, &len);
     if (status != MFRC522::STATUS_OK) {
     Serial.print(F("Reading failed: "));
     Serial.println(mfrc522.GetStatusCodeName(status)); 
     return;
     }
     //PRINT BALANCE
     for (uint8_t i = 0; i < 16; i++){
     if (buffer3[i] != 32){
     Serial.write(buffer3[i]);
     } }
     char namec="";
     char fname[20]="";
     char lname[20]="";
     char chs="";
     for (uint8_t i = 0; i < 16; i++)
     {
     if (buffer1[i] != 32){
      chs=(char)buffer1[i];
      strncat(fname, &chs, 1);
      //Serial.print((char)buffer1[i]);
     }
     }
     chs="";
     //Serial.print(fname);
     //Serial.println();
     for (uint8_t i = 0; i < 16; i++)
     {
     if (buffer2[i] != 32){
      chs=(char)buffer2[i];
      strncat(lname, &chs, 1);
      //Serial.print((char)buffer2[i]);
     }
     }
     //Serial.print(lname);
     strcat(fname, " ");
     strcat(fname, lname);
     Serial.print(" . . . . . .. . ");
     Serial.println(fname);
     long amnt;
     amnt=strtol(buffer3, NULL, 0);
     
     Serial.println(amnt);
     
     Serial.print(F("\n**End Reading**\n"));
     delay(1000); //change value if you want to read cards faster
     mfrc522.PICC_HaltA();
     mfrc522.PCD_StopCrypto1();
  //  R   F  I    D   .   .  .  . .
  display_details(fname, amnt);            //DISPLAY CUSTOMER DETAILS

  lcd.setCursor(0,0);
  lcd.print("A FOR LITRE");
  delay(1500);
  lcd.setCursor(0,0);
  lcd.print("B FOR AMOUNT");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C FOR FULL");
  delay(1500);
  
  char keyi;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter key");
  delay(1000);
  lcd.clear();
  keyi=get_key();           //CHECK ENTERED OPTION
  long inpamnt;
  lcd.setCursor(0,1);
  if (keyi=='A'){
    Serial.println("A");
    lcd.print("A selected");
    inpamnt=set_litre();
    }
  else if (keyi=='B'){
    Serial.println("B");
    lcd.print("B selected");
    inpamnt=set_amount();
    }
  else if (keyi=='C'){
    Serial.println("C");
    lcd.print("C selected");
    //full_tank();
    } 
  else{
    Serial.println("Invalid");
    lcd.print("INVALID OPTION");
    }
   //. . . . . . .. . . . . . .
  double req_ty;
  lcd.clear();
  if(verify_balance){
    Serial.println("TRUE");
      // . .  . . . R .F .I .D. . .BALANCE . . . .. UPDATE . . . .. 
      //  . . .. . .. . . . . .. .. .. . . .

      // . . . . . WRITE .  .E . N . . D . . . . . . . . . . ..
    lcd.print("Setting. . ." );
    delay(1000);
    //. . ASSUME . . ..  1L . . is. . . . 250ml . . . . 
    inpamnt=inpamnt*10;
    req_ty=((double)inpamnt)/250;
    tone(buzzer, 400); // B U Z Z E R
    digitalWrite(rely, LOW);
    delay(req_ty*1000);
    noTone(buzzer);
    digitalWrite(rely, HIGH);
    tone(buzzer, 2000);
    digitalWrite(led, HIGH);
    delay(500);
    noTone(buzzer);
    digitalWrite(led, LOW);
    return;
    
  }
  digitalWrite(rely, LOW);
}
char get_key(){
  char keyinc;
  for(; ;){
  keyinc=myKeypad.getKey();
  if(keyinc){
    lcd.print(keyinc);
    Serial.println(keyinc);
    tone(buzzer, 500);
    delay(500);
    noTone(buzzer);
    //check_option(keyin);
    return keyinc;
    }
  }
}
void display_details(char crname[20], long balance){
  tone(buzzer, 1000);
  digitalWrite(led, HIGH);
  delay(500);
  noTone(buzzer);
  digitalWrite(led, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Customer Name"); //DISPLAY CUSTOMER NAME
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(crname);
  delay(1000);
  Serial.println(crname);
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Account Balance");          //DSIPLAY ACCOUNT BALANCE
  lcd.setCursor(0, 1);
  lcd.print(balance);
  Serial.println(balance);
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
}

long set_litre(){
  lcd.setCursor(0,0);
  lcd.print("Enter quantity"); 
  delay(2300);
  lcd.clear();
  
  char keylt;
  String inSt;
  int inint,amount;
  for(; ;){
    keylt=myKeypad.getKey();
    if(keylt){
      if(keylt!='D'){
        lcd.print(keylt);
        inSt += keylt;
      }
      else if(keylt=='D'){
        inint=inSt.toInt();
        inSt="";
        break;
      }
    }
  }
  Serial.print("Quantity : ");
  Serial.println(inint);
  amount=inint*100;
  Serial.println("Amount "+amount);
  return amount;
}

long set_amount()
{
  lcd.setCursor(0,0);
  lcd.print("Enter amount"); 
  delay(2000);
  lcd.clear();

  char keylt;
  String inSt;
  long inint;
  for(; ;){
    keylt=myKeypad.getKey();
    if(keylt){
      if(keylt!='D'){
        lcd.print(keylt);
        inSt += keylt;
      }
      else if(keylt=='D'){
        inint=inSt.toInt();
        inSt="";
        break;
      }
    }
  }
  Serial.print("Amount :");
  Serial.println(inint);
  tone(buzzer, 1500);
  delay(500);
  noTone(buzzer);
  return inint;
}
boolean verify_balance(long balance, long input_amount){
  if(input_amount<balance){
    return true;
  }
  else{
    return false;
  }
}

 void full_tank()
 {
  lcd.setCursor(0,0);
  lcd.print("Filling...");
 }
