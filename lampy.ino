#include <FastLED.h>

#define MATRIX_DATA_PIN 6
#define BUTTON_LED_PIN  3
#define BUTTON_PIN_IN   2

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60
#define CHIPSET     WS2811
#define NUM_LEDS    64
#define COLOR_ORDER GRB

const int OFF_STATE = 0;
const int WHITE_STATE = 1;
const int RED_STATE = 2;
const int FUCSIA_STATE = 3;
const int CYCLONE_STATE = 4;
const int FIRE_STATE = 5;

int firstState = OFF_STATE;
int lastState = FIRE_STATE; 

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
int stateCount = 0;

void setup()
{
    pinMode(BUTTON_PIN_IN, INPUT_PULLUP);
    pinMode(BUTTON_LED_PIN, OUTPUT);

    delay(3000); // sanity delay
    FastLED.addLeds<CHIPSET, MATRIX_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_IN), onPressed, RISING);
    
    gPal = HeatColors_p;
}

void onPressed()
{
    stateCount++;
    if (stateCount > lastState) 
    {
        stateCount = firstState;
    }
}

void loop()
{
    switch (stateCount)
    {
        case OFF_STATE:
            digitalWrite(BUTTON_LED_PIN, LOW);
            FastLED.clear();  // clear all pixel data
            FastLED.show();
            break;
        case WHITE_STATE:
            digitalWrite(BUTTON_LED_PIN, HIGH);
            for(int i = 0; i< NUM_LEDS; i++)
            {
                leds[i] = CRGB::White; 
            }

            FastLED.show(); 
            break;
        case RED_STATE:
            digitalWrite(BUTTON_LED_PIN, HIGH);
            for(int i = 0; i< NUM_LEDS; i++)
            {
                leds[i] = CRGB::Red; 
            }
            FastLED.show(); 
            delay(10);
            break;
        case FUCSIA_STATE:
            digitalWrite(BUTTON_LED_PIN, HIGH);
            for(int i = 0; i< NUM_LEDS; i++)
            {
                leds[i] = CRGB::Fuchsia; 
            }
            delay(10);
            break;
        case CYCLONE_STATE:
            digitalWrite(BUTTON_LED_PIN, HIGH);
            Cyclone(); // run simulation frame
            FastLED.show(); // display this frame
            delay(10);
            break;
        case FIRE_STATE:
            digitalWrite(BUTTON_LED_PIN, HIGH);
            Fire2012(); // run simulation frame
            FastLED.show(); // display this frame
            FastLED.delay(1000 / FRAMES_PER_SECOND);
            break;
    }

}

// v- The code below from the FastLED Example Lib  -v
// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY

// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// Four different static color palettes are provided here, plus one dynamic one.
// 
// The three static ones are: 
//   1. the FastLED built-in HeatColors_p -- this is the default, and it looks
//      pretty much exactly like the original Fire2012.
//
//  To use any of the other palettes below, just "uncomment" the corresponding code.
//
//   2. a gradient from black to red to yellow to white, which is
//      visually similar to the HeatColors_p, and helps to illustrate
//      what the 'heat colors' palette is actually doing,
//   3. a similar gradient, but in blue colors rather than red ones,
//      i.e. from black to blue to aqua to white, which results in
//      an "icy blue" fire effect,
//   4. a simplified three-step gradient, from black to red to white, just to show
//      that these gradients need not have four components; two or
//      three are possible, too, even if they don't look quite as nice for fire.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120
void Fire2012()
{
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];
    bool gReverseDirection = false;

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) 
    {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) 
    {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) 
    {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) 
    {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }
void Cyclone()
{
    static uint8_t hue = 0;
    // First slide the led in one direction
    for(int i = 0; i < NUM_LEDS; i++) 
    {
      // Set the i'th led to red 
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show(); 
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(10);
    }

    // Now go in the other direction.  
    for(int i = (NUM_LEDS)-1; i >= 0; i--) 
    {
      // Set the i'th led to red 
      leds[i] = CHSV(hue++, 255, 255);
      // Show the leds
      FastLED.show();
      // now that we've shown the leds, reset the i'th led to black
      // leds[i] = CRGB::Black;
      fadeall();
      // Wait a little bit before we loop around and do it again
      delay(10);
    }
}