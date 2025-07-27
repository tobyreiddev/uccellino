#include <FastLED.h>

#define NUM_LEDS 35
#define DATA_PIN 3
#define CLOCK_PIN 2

CRGB leds[NUM_LEDS];

// Variables for color values
int redValue = 255;
int greenValue = 0;
int blueValue = 0;
CRGB currentColor = CRGB::Red;

// Serial communication variables
String inputString = "";
bool stringComplete = false;

void setup() {
  delay(3000);
  
  // Initialize FastLED
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
  
  // Initialize serial communication
  Serial.begin(9600);
  
  // Set initial color (red)
  setColor(0, 0, 0);
  
  Serial.println("Arduino FastLED Color Receiver Ready!");
  Serial.println("Send format: RGB,red,green,blue");
  Serial.println("Example: RGB,255,128,64");
}

void loop() {
  // Check for incoming serial data
  if (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
  
  // Process complete string
  if (stringComplete) {
    processColorCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
}

void processColorCommand(String command) {
  // Remove any whitespace
  command.trim();
  
  // Check if command starts with "RGB,"
  if (command.startsWith("RGB,")) {
    // Remove "RGB," prefix
    command = command.substring(4);
    
    // Parse RGB values
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    
    if (firstComma != -1 && secondComma != -1) {
      // Extract RGB values
      int r = command.substring(0, firstComma).toInt();
      int g = command.substring(firstComma + 1, secondComma).toInt();
      int b = command.substring(secondComma + 1).toInt();
      
      // Constrain values to valid range
      r = constrain(r, 0, 255);
      g = constrain(g, 0, 255);
      b = constrain(b, 0, 255);
      
      // Update color
      setColor(r, g, b);
      
      // Send confirmation
      Serial.print("Color set to RGB(");
      Serial.print(r);
      Serial.print(", ");
      Serial.print(g);
      Serial.print(", ");
      Serial.print(b);
      Serial.println(")");
    } else {
      Serial.println("Error: Invalid RGB format");
    }
  } else if (command.startsWith("BRIGHTNESS,")) {
    // Handle brightness command: BRIGHTNESS,0-255
    int brightness = command.substring(11).toInt();
    brightness = constrain(brightness, 0, 255);
    setBrightness(brightness);
    Serial.print("Brightness set to ");
    Serial.println(brightness);
  } else if (command.equals("RAINBOW")) {
    // Rainbow effect
    rainbow(3);
    Serial.println("Rainbow effect completed");
  } else if (command.equals("OFF")) {
    // Turn off all LEDs
    setColor(0, 0, 0);
    Serial.println("LEDs turned off");
  } else {
    Serial.println("Error: Unknown command");
    Serial.println("Available commands:");
    Serial.println("  RGB,red,green,blue (0-255)");
    Serial.println("  BRIGHTNESS,value (0-255)");
    Serial.println("  RAINBOW");
    Serial.println("  OFF");
  }
}

void setColor(int red, int green, int blue) {
  redValue = red;
  greenValue = green;
  blueValue = blue;
  currentColor = CRGB(red, green, blue);
  
  // Set all LEDs to the same color
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = currentColor;
  }
  
  FastLED.show();
}

void setBrightness(int brightness) {
  FastLED.setBrightness(brightness);
  FastLED.show();
}

// Smooth color transition
void fadeToColor(int targetR, int targetG, int targetB, int duration) {
  int steps = 50;
  int delayTime = duration / steps;
  
  for (int i = 0; i <= steps; i++) {
    int r = map(i, 0, steps, redValue, targetR);
    int g = map(i, 0, steps, greenValue, targetG);
    int b = map(i, 0, steps, blueValue, targetB);
    
    CRGB color = CRGB(r, g, b);
    for (int j = 0; j < NUM_LEDS; j++) {
      leds[j] = color;
    }
    FastLED.show();
    delay(delayTime);
  }
  
  // Update current values
  redValue = targetR;
  greenValue = targetG;
  blueValue = targetB;
  currentColor = CRGB(targetR, targetG, targetB);
}

// Rainbow effect for the entire strip
void rainbow(int cycles) {
  for (int cycle = 0; cycle < cycles; cycle++) {
    for (int hue = 0; hue < 256; hue++) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue, 255, 255);
      }
      FastLED.show();
      delay(20);
    }
  }
}

// Individual LED control (bonus feature)
void setLED(int ledIndex, int red, int green, int blue) {
  if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
    leds[ledIndex] = CRGB(red, green, blue);
    FastLED.show();
  }
}

// Utility functions for predefined colors
void setRed() { setColor(255, 0, 0); }
void setGreen() { setColor(0, 255, 0); }
void setBlue() { setColor(0, 0, 255); }
void setWhite() { setColor(255, 255, 255); }
void setOff() { setColor(0, 0, 0); }

// Fun pattern effects
void colorWipe(CRGB color, int wait) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    FastLED.show();
    delay(wait);
  }
}

void theaterChase(CRGB color, int wait) {
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 3; b++) {
      // Clear strip
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      // Set every third pixel
      for (int i = b; i < NUM_LEDS; i += 3) {
        leds[i] = color;
      }
      FastLED.show();
      delay(wait);
    }
  }
}