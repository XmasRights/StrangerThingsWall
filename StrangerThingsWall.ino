/* LedStripGradient: Example Arduino sketch that shows
 * how to control an Addressable RGB LED Strip from Pololu.
 *
 * To use this, you will need to plug an Addressable RGB LED
 * strip from Pololu into pin 12.  After uploading the sketch,
 * you should see a pattern on the LED strip that fades from
 * green to pink and also moves along the strip.
 */

#include <PololuLedStrip.h>

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

const int BUTTON_PIN = 2;
const int STATE_COUNT = 5;
const int LOOP_DELAY = 20;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 26
rgb_color colors[LED_COUNT];

int animationPause = 0;
int state = 0;
int sequence = 0;
int buttonState = 0;

void setup()
{
  pinMode(2, INPUT);
}

void loop()
{
  checkButton();

  if (animationPause <= 0) { animate(); }
  else { animationPause = animationPause - LOOP_DELAY; }
  
  delay(LOOP_DELAY);  
}

void animate()
{
  for (uint16_t pos = 0; pos < LED_COUNT; pos++)
  {
    int tPos = shouldTranslatePosition(state) == 1 ? translatedPosition(pos) : pos;
    colors[tPos] = enable(sequence, pos, state);
  }

  // Write the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);

  animationPause = sequenceDelay(sequence, state);
  updateSequence(state);
}

int translatedPosition(int pos)
{
  switch (pos)
  {
    case 0: return 18;
    case 1: return 19;
    case 2: return 20;
    case 3: return 21;
    case 4: return 22;
    case 5: return 23;
    case 6: return 24;
    case 7: return 25;
    case 8: return 17;
    case 9: return 16;
    case 10: return 15;
    case 11: return 14;
    case 12: return 13;
    case 13: return 12;
    case 14: return 11;
    case 15: return 10;
    case 16: return 9;
    case 17: return 0;
    case 18: return 1;
    case 19: return 2;
    case 20: return 3;
    case 21: return 4;
    case 22: return 5;
    case 23: return 6;
    case 24: return 7;
    case 25: return 8;
  }

  return 0;
  
}

//////////////////// BUTTON HELPERS ////////////////////
void checkButton()
{
    int newState = digitalRead(BUTTON_PIN);

    if (buttonState != newState)
    {
       buttonState = newState;

       if (buttonState == HIGH)
       {
          incrementState();
       }
    }
}

void incrementState()
{
  if (state + 1 >= STATE_COUNT) { state = 0; }
  else { state = state + 1; }
  
  reset();
}

void reset()
{
  sequence = 0;
  animationPause = 0;
}


//////////////////// STRUCT CALLS ////////////////////

Pololu::rgb_color enable(int seq, int pos, int ste)
{
  switch (ste) 
  {
    // MERRY CHRISTMAS
    case 0:  
    {
      char text[] = "M E R R Y   C H R I S T M A S   ";
      char letter = text[seq];
      int  light  = charToInt(letter);
  
      if (pos == light)
      {
        return colour(pos, seq, ste);
      }
      return dimmed(colour(pos, seq, ste), 40);
    }

    // FLICKER
    case 1:
    { 
      if (seq % 2 == 0) return rgb_color(0, 0, 0);
      return flicker() ? colour(pos, seq, ste) : rgb_color(0, 0, 0);
    }

    // FESTIVE
    case 2:  return colour(pos, seq, ste); 

    // RAINBOW
    case 3: return colour(pos, seq, ste);

    // PEAK:
    case 4:
    { 
      char text[] = "P E A K   ";
      char letter = text[seq];
      int  light  = charToInt(letter);
      
      if (pos == light)
      {
        return rgb_color(255, 255, 255);
      }
      return dimmed(rgb_color(90, 15, 255), 10);
    }
    
    
    case 10: return colour(pos, seq, ste); // ALL ON
    case 11: return seq == pos ? colour(pos, seq, ste) : rgb_color(0, 0, 0); // SEQUENCE TEST
  }
  
  return rgb_color(0, 0, 0);
}

Pololu::rgb_color colour(int pos, int seq, int ste)
{
  switch (ste) {
    case 0:
    case 1:
    case 10: return strangerThingsColour(pos);

    case 2:
    {
      Pololu::rgb_color lights[] = { rgb_color(0, 255, 0), rgb_color(255, 0, 0), rgb_color(0, 0, 255), rgb_color(255, 255, 0) }; 
      return lights[ (pos + seq) % 4 ];
    }

    case 3: 
    {
      int h = 255 * (((LED_COUNT - pos) + seq) % LED_COUNT) / LED_COUNT;
      return hsvToRgb(h, 255, 255);
    }

    case 11: return rgb_color(255, 0, 0);
  } 
  return rgb_color(255, 255, 255);
}

int shouldTranslatePosition(int ste)
{
  switch (ste)
  {
    case 3: return 0;
  }
  return 1;
}

int sequenceCount(int ste)
{
  switch (ste) {
    case 0: return 32; //"M E R R Y   C H R I S T M A S   "
    case 1:  return 2;  // Flicker
    case 2:  return 4;  // Festive
    case 3:  return LED_COUNT;  // Rainbow
    case 4:  return 10;
    case 10: return 1;  // All On
    case 11: return 26; // Sequence Test
  }
}

int sequenceDelay(int seq, int ste)
{
  switch (ste) {
    case 0:  return (seq % 2 == 0) ? 800 : 200;
    case 1:  return random(20, 200);
    case 2:  return 800;
    case 3:  return 1;
    case 4:  return (seq % 2 == 0) ? 800 : 200;
    case 10: return 1000;
    case 11: return 500;
  }
}

//////////////////// HELPERS ////////////////////

void updateSequence(int ste)
{
  sequence = sequence + 1;
  if (sequence >= sequenceCount(ste)) { sequence = 0; }
}

int charToInt(char character)
{
  return character - 65;
}

Pololu::rgb_color dimmed(Pololu::rgb_color colour, int decrease)
{
  return Pololu::rgb_color(colour.red / decrease, colour.green / decrease, colour.blue / decrease);
}

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return rgb_color(r, g, b);
}

//////////////////// EXTENSION ////////////////////
Pololu::rgb_color strangerThingsColour(int pos)
{
  Pololu::rgb_color lights[] = 
  { rgb_color(245, 255, 246), // A
    rgb_color(0, 0, 255),     // B
    rgb_color(0, 255, 100),   // C
    rgb_color(230, 156, 217), // D
    rgb_color(125, 0, 255),   // E
    rgb_color(51, 255, 0),    // F
    rgb_color(0, 153, 0),     // G
    rgb_color(255, 0, 127),   // H
    rgb_color(255, 0, 127),   // I
    rgb_color(153, 255, 255), // J
    rgb_color(102, 0, 204),   // K
    rgb_color(255, 102, 204), // L
    rgb_color(51, 255, 0),    // M
    rgb_color(153, 255, 255), // N
    rgb_color(0, 255, 100),   // O
    rgb_color(255, 102, 204), // P
    rgb_color(0, 102, 102),   // Q
    rgb_color(255, 102, 204), // R
    rgb_color(245, 255, 246), // S
    rgb_color(51, 255, 0),    // T
    rgb_color(125, 0, 255),   // U
    rgb_color(153, 255, 255), // V
    rgb_color(125, 0, 255),   // W
    rgb_color(51, 255, 0),    // X
    rgb_color(153, 255, 255), // Y
    rgb_color(0, 153, 0)      // Z
  };

  return lights[pos];
}

bool flicker()
{
  return (random(0, 100) < 70);
}


