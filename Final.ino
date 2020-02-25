// Main arduino file
char gridValue = 1 ;  // Grid Value
char character = 'S'; // Command Char
boolean godMode = false; // Toggle fastest path speeed


void setup(){
  setUpDora();
  setupMotorEncoder();
  setupPID();
// Leaderboard crushing
// By someone in SCSE
// $$$$$$$$\ $$\       $$\                 $$\                 $$$$$$$\                                     
// \__$$  __|$$ |      \__|                \__|                $$  __$$\                                    
//    $$ |   $$$$$$$\  $$\  $$$$$$$\       $$\  $$$$$$$\       $$ |  $$ | $$$$$$\   $$$$$$\  $$$$$$\        
//    $$ |   $$  __$$\ $$ |$$  _____|      $$ |$$  _____|      $$ |  $$ |$$  __$$\ $$  __$$\ \____$$\       
//    $$ |   $$ |  $$ |$$ |\$$$$$$\        $$ |\$$$$$$\        $$ |  $$ |$$ /  $$ |$$ |  \__|$$$$$$$ |      
//    $$ |   $$ |  $$ |$$ | \____$$\       $$ | \____$$\       $$ |  $$ |$$ |  $$ |$$ |     $$  __$$ |      
//    $$ |   $$ |  $$ |$$ |$$$$$$$  |      $$ |$$$$$$$  |      $$$$$$$  |\$$$$$$  |$$ |     \$$$$$$$ |      
//    \__|   \__|  \__|\__|\_______/       \__|\_______/       \_______/  \______/ \__|      \_______|      
                                                                                                         
}                                                            
void loop(){
    while (Serial.available() > 0){ // read the incoming byte:
      if(Serial.peek() >= 65 && Serial.peek() < 91){   // For normal commands
        character = Serial.read();
      }
      else if (Serial.peek() >= 48 && Serial.peek() < 58){   // For 0-9 , append char 'A' and int will be the Gridvalue
        character = 'A';
        gridValue = Serial.parseInt();
      }

    Serial.print("I received: ");   // Code to check what is the bot recieving
    Serial.println(character);
    Serial.print(" GridValue: ");
    Serial.println((int)gridValue);
    
    // Switch cases for bot movement
    switch(character){
      case 'A': moveForward((int)gridValue);                 
              break;
      case 'L': turnLeft(90);
              break;
      case 'R': turnRight(90);
              break;
      case 'C': calibrate();
              break;        
      case 'O': readAllSensors();
              break;  
      case 'D': calibrate();
              break;   
      case 'F': godMode = true;
                Serial.println("God Mode Activated!");
              break;
      case 'N': godMode = false; 
                Serial.println("God Mode Deactivated!");
              break;
      case 'S': Serial.println("EX_START RECEIVED");
              break;
       default: 
              break;
    }
    
    // Send Grid data after every movemnet
    sendGridData();
  
    /*Serial.print("Sending to pi :  ");   // Code to check what is the bot sending
    Serial.print(character);
    Serial.println()
    */
    Serial.read();


    
  }
}
// Function for set up
void setUpDora(){
  Serial.begin(115200);
  Serial.setTimeout(50);   // Limit the serial port reading time to reduce delay
  Serial.println("Unleashing Dora ...... Press S to start");
  while(Serial.read() != 'S'){ }
  Serial.println("Dora Activated....");  // Bot wont start before 'S' is pressed
  delay(1000);

}
