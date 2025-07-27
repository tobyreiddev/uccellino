#include <FastLED.h>

// Hardware configuration
#define NUM_LEDS 35
#define DATA_PIN 3
#define CLOCK_PIN 2
#define MAX_BRIGHTNESS 255
#define DEFAULT_BRIGHTNESS 100

// LED array
CRGB leds[NUM_LEDS];

// State management
struct LEDState {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t brightness = DEFAULT_BRIGHTNESS;
  bool isOn = false;
} currentState;

// Serial communication
String inputBuffer = "";
const uint8_t MAX_BUFFER_SIZE = 64;

// Effect types and parameters
enum EffectType {
  EFFECT_NONE,
  EFFECT_RAINBOW_CYCLE,
  EFFECT_BREATHING,
  EFFECT_FIRE,
  EFFECT_WAVE,
  EFFECT_TWINKLE_CONTINUOUS,
  EFFECT_CHASE_CONTINUOUS
};

struct EffectState {
  EffectType currentEffect = EFFECT_NONE;
  bool running = false;
  uint32_t lastUpdate = 0;
  uint16_t speed = 50;
  uint8_t intensity = 255;
  
  // Effect-specific variables
  uint8_t hue = 0;
  uint8_t brightness = 128;
  uint16_t step = 0;
  bool direction = true;
  uint8_t position = 0;
} effectState;

void setup() {
  // Startup delay for power stabilization
  delay(1000);
  
  // Initialize FastLED with error checking
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000); // Power management
  
  // Clear all LEDs
  FastLED.clear();
  FastLED.show();
  
  // Initialize serial with higher baud rate
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait for serial or timeout
  
  // Reserve memory for input buffer
  inputBuffer.reserve(MAX_BUFFER_SIZE);
  
  // Startup sequence
  startupSequence();
  
  // Print help information
  printHelp();
}

void loop() {
  handleSerialInput();
  
  // Handle running effects in a non-blocking way
  if (effectState.running) {
    uint32_t currentTime = millis();
    
    // Only update effect if enough time has passed
    if (currentTime - effectState.lastUpdate >= effectState.speed) {
      updateCurrentEffect();
      effectState.lastUpdate = currentTime;
    }
  }
  
  // Small delay to prevent overwhelming the processor
  delay(1);
}

void handleSerialInput() {
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = "";
      }
    } else if (inChar >= 32 && inChar <= 126) { // Printable characters only
      if (inputBuffer.length() < MAX_BUFFER_SIZE - 1) {
        inputBuffer += inChar;
      } else {
        Serial.println(F("Error: Command too long"));
        inputBuffer = "";
      }
    }
  }
}

void processCommand(String command) {
  command.trim();
  command.toUpperCase(); // Case insensitive commands
  
  if (command.startsWith("RGB,")) {
    handleRGBCommand(command);
  } else if (command.startsWith("HSV,")) {
    handleHSVCommand(command);
  } else if (command.startsWith("BRIGHTNESS,") || command.startsWith("B,")) {
    handleBrightnessCommand(command);
  } else if (command.startsWith("LED,")) {
    handleIndividualLEDCommand(command);
  } else if (command.startsWith("FADE,")) {
    handleFadeCommand(command);
  } else if (command.startsWith("SPEED,")) {
    handleSpeedCommand(command);
  } else if (command.startsWith("INTENSITY,")) {
    handleIntensityCommand(command);
  } else if (command == "RAINBOW") {
    runRainbowEffect();
  } else if (command == "CHASE") {
    runChaseEffect();
  } else if (command == "WIPE") {
    runWipeEffect();
  } else if (command == "TWINKLE") {
    runTwinkleEffect();
  } else if (command == "CRAINBOW") {
    startContinuousEffect(EFFECT_RAINBOW_CYCLE);
  } else if (command == "CBREATHING") {
    startContinuousEffect(EFFECT_BREATHING);
  } else if (command == "CFIRE") {
    startContinuousEffect(EFFECT_FIRE);
  } else if (command == "CWAVE") {
    startContinuousEffect(EFFECT_WAVE);
  } else if (command == "CTWINKLE") {
    startContinuousEffect(EFFECT_TWINKLE_CONTINUOUS);
  } else if (command == "CCHASE") {
    startContinuousEffect(EFFECT_CHASE_CONTINUOUS);
  } else if (command == "STOP") {
    stopContinuousEffect();
  } else if (command == "ON") {
    turnOn();
  } else if (command == "OFF") {
    turnOff();
  } else if (command == "STATUS") {
    printStatus();
  } else if (command == "HELP") {
    printHelp();
  } else if (command == "RESET") {
    resetToDefaults();
  } else {
    Serial.println(F("Error: Unknown command. Type 'HELP' for available commands."));
  }
}

void handleRGBCommand(const String& command) {
  // Parse RGB,r,g,b
  int values[3];
  if (parseIntValues(command.substring(4), values, 3)) {
    for (int i = 0; i < 3; i++) {
      values[i] = constrain(values[i], 0, 255);
    }
    
    setAllLEDs(values[0], values[1], values[2]);
    Serial.print(F("RGB set to ("));
    Serial.print(values[0]); Serial.print(F(", "));
    Serial.print(values[1]); Serial.print(F(", "));
    Serial.print(values[2]); Serial.println(F(")"));
  } else {
    Serial.println(F("Error: RGB format should be RGB,r,g,b (0-255)"));
  }
}

void handleHSVCommand(const String& command) {
  // Parse HSV,h,s,v
  int values[3];
  if (parseIntValues(command.substring(4), values, 3)) {
    uint8_t h = constrain(values[0], 0, 359) * 255 / 359; // Convert to 0-255
    uint8_t s = constrain(values[1], 0, 100) * 255 / 100;  // Convert percentage
    uint8_t v = constrain(values[2], 0, 100) * 255 / 100;  // Convert percentage
    
    CRGB color = CHSV(h, s, v);
    setAllLEDs(color.r, color.g, color.b);
    Serial.print(F("HSV set to ("));
    Serial.print(values[0]); Serial.print(F("°, "));
    Serial.print(values[1]); Serial.print(F("%, "));
    Serial.print(values[2]); Serial.println(F("%)"));
  } else {
    Serial.println(F("Error: HSV format should be HSV,h,s,v (h:0-359, s&v:0-100)"));
  }
}

void handleBrightnessCommand(const String& command) {
  int commaIndex = command.indexOf(',');
  if (commaIndex != -1) {
    int brightness = command.substring(commaIndex + 1).toInt();
    brightness = constrain(brightness, 0, MAX_BRIGHTNESS);
    setBrightness(brightness);
    Serial.print(F("Brightness set to "));
    Serial.println(brightness);
  } else {
    Serial.println(F("Error: Brightness format should be BRIGHTNESS,value (0-255)"));
  }
}

void handleIndividualLEDCommand(const String& command) {
  // Parse LED,index,r,g,b
  int values[4];
  if (parseIntValues(command.substring(4), values, 4)) {
    int index = constrain(values[0], 0, NUM_LEDS - 1);
    int r = constrain(values[1], 0, 255);
    int g = constrain(values[2], 0, 255);  
    int b = constrain(values[3], 0, 255);
    
    leds[index] = CRGB(r, g, b);
    FastLED.show();
    Serial.print(F("LED "));
    Serial.print(index);
    Serial.print(F(" set to RGB("));
    Serial.print(r); Serial.print(F(", "));
    Serial.print(g); Serial.print(F(", "));
    Serial.print(b); Serial.println(F(")"));
  } else {
    Serial.println(F("Error: LED format should be LED,index,r,g,b"));
  }
}

void handleFadeCommand(const String& command) {
  // Parse FADE,r,g,b,duration
  int values[4];
  if (parseIntValues(command.substring(5), values, 4)) {
    int r = constrain(values[0], 0, 255);
    int g = constrain(values[1], 0, 255);
    int b = constrain(values[2], 0, 255);
    int duration = constrain(values[3], 100, 10000);
    
    fadeToColor(r, g, b, duration);
    Serial.println(F("Fade effect completed"));
  } else {
    Serial.println(F("Error: FADE format should be FADE,r,g,b,duration_ms"));
  }
}

void handleSpeedCommand(const String& command) {
  int commaIndex = command.indexOf(',');
  if (commaIndex != -1) {
    int speed = command.substring(commaIndex + 1).toInt();
    effectState.speed = constrain(speed, 1, 1000);
    Serial.print(F("Effect speed set to "));
    Serial.println(effectState.speed);
  } else {
    Serial.println(F("Error: SPEED format should be SPEED,value (1-1000)"));
  }
}

void handleIntensityCommand(const String& command) {
  int commaIndex = command.indexOf(',');
  if (commaIndex != -1) {
    int intensity = command.substring(commaIndex + 1).toInt();
    effectState.intensity = constrain(intensity, 0, 255);
    Serial.print(F("Effect intensity set to "));
    Serial.println(effectState.intensity);
  } else {
    Serial.println(F("Error: INTENSITY format should be INTENSITY,value (0-255)"));
  }
}

void updateCurrentEffect() {
  switch (effectState.currentEffect) {
    case EFFECT_RAINBOW_CYCLE:
      updateRainbowCycle();
      break;
      
    case EFFECT_BREATHING:
      updateBreathing();
      break;
      
    case EFFECT_FIRE:
      updateFire();
      break;
      
    case EFFECT_WAVE:
      updateWave();
      break;
      
    case EFFECT_TWINKLE_CONTINUOUS:
      updateTwinkleContinuous();
      break;
      
    case EFFECT_CHASE_CONTINUOUS:
      updateChaseContinuous();
      break;
      
    default:
      effectState.running = false;
      break;
  }
}

void updateRainbowCycle() {
  fill_solid(leds, NUM_LEDS, CHSV(effectState.hue, 255, effectState.intensity));
  FastLED.show();
  
  effectState.hue++;
  if (effectState.hue >= 255) {
    effectState.hue = 0;
  }
}

void updateBreathing() {
  if (effectState.direction) {
    effectState.brightness += 3;
    if (effectState.brightness >= effectState.intensity) {
      effectState.brightness = effectState.intensity;
      effectState.direction = false;
    }
  } else {
    effectState.brightness -= 3;
    if (effectState.brightness <= 30) {
      effectState.brightness = 30;
      effectState.direction = true;
    }
  }
  
  CRGB color = CRGB(currentState.red, currentState.green, currentState.blue);
  if (!currentState.isOn) color = CRGB::White;
  
  color.fadeToBlackBy(255 - effectState.brightness);
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

void updateFire() {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t red = random(150, effectState.intensity);
    uint8_t green = random(0, red / 3);
    uint8_t blue = 0;
    
    // Add random flickering
    if (random(10) < 2) {
      red = red / 3;
      green = green / 3;
    }
    
    leds[i] = CRGB(red, green, blue);
  }
  FastLED.show();
}

void updateWave() {
  for (int i = 0; i < NUM_LEDS; i++) {
    float angle = (effectState.step + i * 15) * 0.1;
    uint8_t brightness = (sin(angle) + 1) * (effectState.intensity / 2);
    
    CRGB color = CRGB(currentState.red, currentState.green, currentState.blue);
    if (!currentState.isOn) color = CRGB::Blue;
    
    color.fadeToBlackBy(255 - brightness);
    leds[i] = color;
  }
  
  effectState.step++;
  if (effectState.step >= 628) {
    effectState.step = 0;
  }
  
  FastLED.show();
}

void updateTwinkleContinuous() {
  // Fade all LEDs slightly
  fadeToBlackBy(leds, NUM_LEDS, 20);
  
  // Add new twinkles randomly
  if (random(10) < 3) {
    int pos = random(NUM_LEDS);
    CRGB twinkleColor = CRGB(currentState.red, currentState.green, currentState.blue);
    if (!currentState.isOn) twinkleColor = CRGB::White;
    
    twinkleColor.fadeToBlackBy(255 - effectState.intensity);
    leds[pos] = twinkleColor;
  }
  
  FastLED.show();
}

void updateChaseContinuous() {
  FastLED.clear();
  
  CRGB chaseColor = CRGB(currentState.red, currentState.green, currentState.blue);
  if (!currentState.isOn) chaseColor = CRGB::Red;
  chaseColor.fadeToBlackBy(255 - effectState.intensity);
  
  // Create chase pattern with 3 consecutive LEDs
  for (int i = 0; i < 3; i++) {
    int pos = (effectState.position + i) % NUM_LEDS;
    leds[pos] = chaseColor;
  }
  
  effectState.position++;
  if (effectState.position >= NUM_LEDS) {
    effectState.position = 0;
  }
  
  FastLED.show();
}

void startContinuousEffect(EffectType effect) {
  effectState.currentEffect = effect;
  effectState.running = true;
  effectState.lastUpdate = millis();
  effectState.step = 0;
  effectState.hue = 0;
  effectState.brightness = 128;
  effectState.direction = true;
  effectState.position = 0;
  
  Serial.print(F("Started continuous effect: "));
  switch (effect) {
    case EFFECT_RAINBOW_CYCLE:
      Serial.println(F("Rainbow Cycle"));
      break;
    case EFFECT_BREATHING:
      Serial.println(F("Breathing"));
      break;
    case EFFECT_FIRE:
      Serial.println(F("Fire"));
      break;
    case EFFECT_WAVE:
      Serial.println(F("Wave"));
      break;
    case EFFECT_TWINKLE_CONTINUOUS:
      Serial.println(F("Continuous Twinkle"));
      break;
    case EFFECT_CHASE_CONTINUOUS:
      Serial.println(F("Continuous Chase"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }
}

void stopContinuousEffect() {
  effectState.running = false;
  effectState.currentEffect = EFFECT_NONE;
  Serial.println(F("Stopped continuous effect"));
}

bool parseIntValues(const String& str, int* values, int count) {
  int startIndex = 0;
  int valueIndex = 0;
  
  for (int i = 0; i <= str.length() && valueIndex < count; i++) {
    if (i == str.length() || str.charAt(i) == ',') {
      if (i > startIndex) {
        values[valueIndex] = str.substring(startIndex, i).toInt();
        valueIndex++;
      }
      startIndex = i + 1;
    }
  }
  
  return valueIndex == count;
}

void setAllLEDs(uint8_t red, uint8_t green, uint8_t blue) {
  currentState.red = red;
  currentState.green = green;
  currentState.blue = blue;
  currentState.isOn = (red > 0 || green > 0 || blue > 0);
  
  CRGB color = CRGB(red, green, blue);
  fill_solid(leds, NUM_LEDS, color); // More efficient than loop
  FastLED.show();
}

void setBrightness(uint8_t brightness) {
  currentState.brightness = brightness;
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void fadeToColor(uint8_t targetR, uint8_t targetG, uint8_t targetB, uint16_t duration) {
  const uint8_t steps = 100;
  const uint16_t delayTime = duration / steps;
  
  uint8_t startR = currentState.red;
  uint8_t startG = currentState.green;
  uint8_t startB = currentState.blue;
  
  for (uint8_t i = 0; i <= steps; i++) {
    uint8_t r = map(i, 0, steps, startR, targetR);
    uint8_t g = map(i, 0, steps, startG, targetG);
    uint8_t b = map(i, 0, steps, startB, targetB);
    
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(delayTime);
  }
  
  currentState.red = targetR;
  currentState.green = targetG;
  currentState.blue = targetB;
  currentState.isOn = (targetR > 0 || targetG > 0 || targetB > 0);
}

void runRainbowEffect() {
  Serial.println(F("Running rainbow effect..."));
  
  // Stop any continuous effect first
  bool wasRunning = effectState.running;
  if (wasRunning) stopContinuousEffect();
  
  for (uint16_t cycle = 0; cycle < 3; cycle++) {
    for (uint8_t hue = 0; hue < 255; hue++) {
      fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
      FastLED.show();
      delay(effectState.speed);
      
      // Check for serial interrupt
      if (Serial.available()) {
        Serial.println(F("Effect interrupted"));
        return;
      }
    }
  }
  
  Serial.println(F("Rainbow effect completed"));
}

void runChaseEffect() {
  Serial.println(F("Running chase effect..."));
  
  // Stop any continuous effect first
  bool wasRunning = effectState.running;
  if (wasRunning) stopContinuousEffect();
  
  CRGB chaseColor = CRGB(currentState.red, currentState.green, currentState.blue);
  if (!currentState.isOn) chaseColor = CRGB::White;
  
  for (uint8_t cycle = 0; cycle < 5; cycle++) {
    for (int pos = 0; pos < NUM_LEDS; pos++) {
      FastLED.clear();
      
      // Create chase pattern
      for (int i = 0; i < 3; i++) {
        int ledPos = (pos + i) % NUM_LEDS;
        leds[ledPos] = chaseColor;
      }
      
      FastLED.show();
      delay(effectState.speed);
      
      if (Serial.available()) {
        Serial.println(F("Effect interrupted"));
        return;
      }
    }
  }
  
  Serial.println(F("Chase effect completed"));
}

void runWipeEffect() {
  Serial.println(F("Running wipe effect..."));
  
  // Stop any continuous effect first
  bool wasRunning = effectState.running;
  if (wasRunning) stopContinuousEffect();
  
  CRGB wipeColor = CRGB(currentState.red, currentState.green, currentState.blue);
  if (!currentState.isOn) wipeColor = CRGB::Red;
  
  // Wipe on
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = wipeColor;
    FastLED.show();
    delay(effectState.speed);
    
    if (Serial.available()) {
      Serial.println(F("Effect interrupted"));
      return;
    }
  }
  
  delay(500);
  
  // Wipe off
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(effectState.speed);
    
    if (Serial.available()) {
      Serial.println(F("Effect interrupted"));
      return;
    }
  }
  
  Serial.println(F("Wipe effect completed"));
}

void runTwinkleEffect() {
  Serial.println(F("Running twinkle effect..."));
  
  // Stop any continuous effect first
  bool wasRunning = effectState.running;  
  if (wasRunning) stopContinuousEffect();
  
  CRGB twinkleColor = CRGB(currentState.red, currentState.green, currentState.blue);
  if (!currentState.isOn) twinkleColor = CRGB::White;
  
  for (uint16_t i = 0; i < 200; i++) {
    // Random LED selection
    int randomLED = random(NUM_LEDS);
    
    // Random brightness
    uint8_t brightness = random(50, 255);
    leds[randomLED] = twinkleColor;
    leds[randomLED].fadeToBlackBy(255 - brightness);
    
    // Fade all LEDs slightly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    
    FastLED.show();
    delay(effectState.speed);
    
    if (Serial.available()) {
      Serial.println(F("Effect interrupted"));
      FastLED.clear();
      FastLED.show();
      return;
    }
  }
  
  FastLED.clear();
  FastLED.show();
  Serial.println(F("Twinkle effect completed"));
}

void turnOn() {
  if (currentState.red == 0 && currentState.green == 0 && currentState.blue == 0) {
    setAllLEDs(255, 255, 255); // Default to white
  } else {
    setAllLEDs(currentState.red, currentState.green, currentState.blue);
  }
  Serial.println(F("LEDs turned on"));
}

void turnOff() {
  FastLED.clear();
  FastLED.show();
  currentState.isOn = false;
  Serial.println(F("LEDs turned off"));
}

void resetToDefaults() {
  // Reset LED state
  currentState.red = 0;
  currentState.green = 0;
  currentState.blue = 0;
  currentState.brightness = DEFAULT_BRIGHTNESS;
  currentState.isOn = false;
  
  // Reset effect state
  effectState.currentEffect = EFFECT_NONE;
  effectState.running = false;
  effectState.lastUpdate = 0;
  effectState.speed = 50;
  effectState.intensity = 255;
  effectState.hue = 0;
  effectState.brightness = 128;
  effectState.step = 0;
  effectState.direction = true;
  effectState.position = 0;
  
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  Serial.println(F("Reset to defaults"));
}

void startupSequence() {
  // Brief startup animation
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(20);
  }
  delay(500);
  FastLED.clear();
  FastLED.show();
}

void printStatus() {
  Serial.println(F("=== LED Strip Status ==="));
  Serial.print(F("LEDs: ")); Serial.println(NUM_LEDS);
  Serial.print(F("State: ")); Serial.println(currentState.isOn ? F("ON") : F("OFF"));
  Serial.print(F("RGB: (")); 
  Serial.print(currentState.red); Serial.print(F(", "));
  Serial.print(currentState.green); Serial.print(F(", "));
  Serial.print(currentState.blue); Serial.println(F(")"));
  Serial.print(F("Brightness: ")); Serial.println(currentState.brightness);
  Serial.print(F("Effect Speed: ")); Serial.println(effectState.speed);
  Serial.print(F("Effect Intensity: ")); Serial.println(effectState.intensity);
  Serial.print(F("Continuous Effect: ")); 
  if (effectState.running) {
    switch (effectState.currentEffect) {
      case EFFECT_RAINBOW_CYCLE: Serial.println(F("Rainbow Cycle")); break;
      case EFFECT_BREATHING: Serial.println(F("Breathing")); break;
      case EFFECT_FIRE: Serial.println(F("Fire")); break;
      case EFFECT_WAVE: Serial.println(F("Wave")); break;
      case EFFECT_TWINKLE_CONTINUOUS: Serial.println(F("Continuous Twinkle")); break;
      case EFFECT_CHASE_CONTINUOUS: Serial.println(F("Continuous Chase")); break;
      default: Serial.println(F("Unknown")); break;
    }
  } else {
    Serial.println(F("None"));
  }
  Serial.print(F("Memory: ")); Serial.print(freeRam()); Serial.println(F(" bytes"));
  Serial.println(F("====================="));
}

void printHelp() {
  Serial.println(F("\n=== FastLED Controller Commands ==="));
  Serial.println(F("Basic Colors:"));
  Serial.println(F("RGB,r,g,b          - Set RGB color (0-255)"));
  Serial.println(F("HSV,h,s,v          - Set HSV color (h:0-359, s&v:0-100)"));
  Serial.println(F("BRIGHTNESS,val     - Set brightness (0-255)"));
  Serial.println(F("B,val              - Short brightness command"));
  Serial.println(F("LED,index,r,g,b    - Set individual LED"));
  Serial.println(F("FADE,r,g,b,ms      - Fade to color over time"));
  Serial.println(F(""));
  Serial.println(F("Effect Settings:"));
  Serial.println(F("SPEED,val          - Set effect speed (1-1000)"));
  Serial.println(F("INTENSITY,val      - Set effect intensity (0-255)"));
  Serial.println(F(""));
  Serial.println(F("One-shot Effects:"));
  Serial.println(F("RAINBOW            - Rainbow cycle effect"));
  Serial.println(F("CHASE              - Chase effect"));
  Serial.println(F("WIPE               - Color wipe effect"));
  Serial.println(F("TWINKLE            - Twinkle effect"));
  Serial.println(F(""));
  Serial.println(F("Continuous Effects:"));
  Serial.println(F("CRAINBOW           - Continuous rainbow cycle"));
  Serial.println(F("CBREATHING         - Continuous breathing effect"));
  Serial.println(F("CFIRE              - Continuous fire effect"));
  Serial.println(F("CWAVE              - Continuous wave effect"));
  Serial.println(F("CTWINKLE           - Continuous twinkle"));
  Serial.println(F("CCHASE             - Continuous chase"));
  Serial.println(F("STOP               - Stop continuous effects"));
  Serial.println(F(""));
  Serial.println(F("Control:"));
  Serial.println(F("ON                 - Turn on LEDs"));
  Serial.println(F("OFF                - Turn off LEDs"));
  Serial.println(F("STATUS             - Show current status"));
  Serial.println(F("RESET              - Reset to defaults"));
  Serial.println(F("HELP               - Show this help"));
  Serial.println(F(""));
  Serial.println(F("Note: Continuous effects run in background"));
  Serial.println(F("      and can be controlled while running!"));
  Serial.println(F("===================================\n"));
}

int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}