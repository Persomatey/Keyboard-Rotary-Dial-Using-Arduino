#include <AsyncTimer.h>

// Rotary definitions 
#define fromRotaryA 7
#define fromRotaryB 8
#define fromRotaryS 6 
int switchState = HIGH; 

// Mosfet definitions 
#define mosfetControlU 4
#define mosfetControlD 3
#define mosfetControlS 2

// Declare variables 
int counter = 0; 
int aState; 
int aLastState; 
int upQueue = 0; 
int downQueue = 0; 
bool switchBool = false; 

// Declare async timers 
AsyncTimer up;
AsyncTimer down;
AsyncTimer sw; 

void setup()
{
  Serial.begin(1000000); 
  
  // Set the pins coming from the rotary tool as inputs 
  pinMode(fromRotaryA, INPUT); 
  pinMode(fromRotaryB, INPUT); 
  pinMode(fromRotaryS, INPUT_PULLUP);

  // Set the pins sent to the mosfet's gate as outputs 
  pinMode(mosfetControlU, OUTPUT); 
  pinMode(mosfetControlD, OUTPUT); 
  pinMode(mosfetControlS, OUTPUT); 

  // Close the mosfet gates initially
  digitalWrite(mosfetControlU, LOW); 
  digitalWrite(mosfetControlD, LOW); 
  digitalWrite(mosfetControlS, LOW); 

  // Set async functions 
  up.setInterval([]() { IncrementVolume(); }, 50);
  down.setInterval([]() { DecrementVolume(); }, 50);
  sw.setInterval([]() { ButtonRead(); }, 50);

  // Initial read of rotary dial's state 
  aLastState = digitalRead(fromRotaryA);
}

void loop()
{
  VolumeControl(); 

  // Handle the async functions 
  up.handle(); 
  down.handle(); 
  sw.handle(); 
}

void ButtonRead()
{
  // Read the current switch state 
  switchState = digitalRead(fromRotaryS); 

  // Using a bool to check once per state change because unpressed is always HIGH and pressed is always LOW 
  if (switchState == LOW && switchBool == false)
  {
    // Allow current throught the mosfet gate, then shortly after disallow it 
    Serial.println("Switch Pressed"); 
    digitalWrite(mosfetControlS, HIGH); 
    delay(25); 
    digitalWrite(mosfetControlS, LOW); 
    switchBool = true; 
  }
  
  if (switchState == HIGH && switchBool == true)
  {
    // Reset switchBool 
    Serial.println("Switch Released"); 
    switchBool = false; 
  }
}

void VolumeControl()
{
  // Read the current rotary state 
  aState = digitalRead(fromRotaryA); 

  // Check if the rotary state has changed recently 
  if (aState != aLastState && aState == 0)
  {
    // Compare the rotary state 
    if(digitalRead(fromRotaryB) != aState)
    {
      // Increment volume 
      counter++; 
      upQueue++; 
      downQueue = 0; 
    }
    else
    {
      // Decrement volume 
      counter--; 
      downQueue++; 
      upQueue = 0; 
    }

    Serial.print("Position:"); 
    Serial.println(counter); 
  }

  // Reset the last state 
  aLastState = aState; 
}

void IncrementVolume()
{
  // If there are still changes in the queue 
  if (upQueue > 0)
  {
    // Allow current throught the mosfet gate, then shortly after disallow it 
    digitalWrite(mosfetControlU, HIGH); 
    delay(25); 
    digitalWrite(mosfetControlU, LOW); 
    upQueue--; 
  }
  else if (upQueue < 0)
  {
    upQueue = 0; 
  }
}

void DecrementVolume()
{
  // If there are still changes in the queue 
  if (downQueue > 0)
  {
    // Allow current throught the mosfet gate, then shortly after disallow it 
    digitalWrite(mosfetControlD, HIGH); 
    delay(25); 
    digitalWrite(mosfetControlD, LOW); 
    downQueue--; 
  }
  else if (downQueue < 0)
  {
    downQueue = 0; 
  }
}
