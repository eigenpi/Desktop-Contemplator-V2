// cristinel.ababei; 2016
// Project: "Desktop Contemplator V2.0"
//
// this is an Arduino project using an Arduino Mega board to show on a
// TFT display several different visual effects; user can select via 
// three TFT buttons one of three different types of patterns to 
// render for your enjoyment: 
// 1) fractals
// 2) plasmas 
// 3) cool geometry
//
// Hardware (HW) needed:
// --Mega 2560 R3 + Adaptor Shield + 3.2 TFT LCD Touch Panel
//   https://www.sainsmart.com/products/mega2560-r3-adaptor-shield-3-2-tft-lcd-touch-panel
//   If you cannot find the exact HW, you should be able to portion
//   this code to any Arduni MEga and TFT Display.
//
// Software (SW) needed:
// -- Arduino IDE
// -- UTFT Library
//    http://www.rinkydinkelectronics.com/library.php?id=51#google_vignette
// 
// NOTE: 
// please use a stylus to press buttons on the TFT display; if you use
// bare fingers, operation is glitchy; a pencil with eraser works great!
// 
// CREDITS:
// this code is an adaptation of the project "Desktop Contemplator for Nuts 
// and Volts", which was developed by Craig A. Lindley for SainSmart 3.2" 
// TFT LCD Display; 
// https://www.nutsvolts.com/magazine/article/december2012_Lindley


#include <UTFT.h>
#include <UTouch.h>

// timeout limit;
#define PATTERN_DISPLAY_DURATION_SECONDS 10
#define BUTTONS_DISPLAY_DURATION_SECONDS 5

#define NUMBER_OF_PATTERNS_FRACTAL 2 
#define NUMBER_OF_PATTERNS_PLASMA 1 
#define NUMBER_OF_PATTERNS_GEOMETRY 6 

enum PATTERNS_TYPE {FRACTAL, PLASMA, GEOMETRY};
PATTERNS_TYPE pattern_to_render_next = FRACTAL; // default;

// indices for all patterns; we'll just iterate thru the selected pattern 
// category and then repeat, unless user changes that via push-buttons;
int id_pattern_fractal = -1;
int id_pattern_plasma = -1;
int id_pattern_geometry = -1;

extern uint8_t SmallFont[];
int x=0, y=0; // coordinates used for TFT screen;


// create an instance of the LCD graphics object called "lcd";
// the LCD display uses the SSD1289 driver; that is  
// for Sainsmart Arduino Mega 2560 + LCD shield + LCD display;
UTFT lcd(SSD1289, 38, 39, 40, 41); 

// create object of TFT touch screen;
UTouch myTouch(6, 5, 4, 3, 2);

// chronometer mechanics;
boolean timeOutEnabled = false;
unsigned long timeOut;

///////////////////////////////////////////////////////////////////////////////
//
// System Setup
//
///////////////////////////////////////////////////////////////////////////////

void setup() 
{
  // (0) Setup serial interface for debugging
  // NOTE: Cannot use Serial and LCD at the same time
  // Serial.begin(115200);

  // (1) basic initializations;
  // Use the analog inputs to seed random number generator
  // All analog lines to input
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  // Read each input
  int v0 = analogRead(A0);
  int v1 = analogRead(A1);
  int v2 = analogRead(A2);
  int v3 = analogRead(A3);
  int v4 = analogRead(A4);
  int v5 = analogRead(A5);

  // Seed the generator
  long total = v0 + v1 + v2 + v3 + v4 + v5;
  randomSeed(total);

  // All analog lines to output for LCD display (on Uno)
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);

  // Setup the LCD display
  lcd.InitLCD();

  // (2) additional initializations for the TFT portion of things;
  lcd.clrScr();
  lcd.setFont(SmallFont);
  
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
}


///////////////////////////////////////////////////////////////////////////////
//
// Main Program Loop
//
///////////////////////////////////////////////////////////////////////////////

void loop() 
{
  
  while (true) {

    // (1) pick the next pattern from the selected category and render it;
    switch ( pattern_to_render_next) {
      case FRACTAL:
        run_next_pattern_fractal();
        break;
      case PLASMA:
        run_next_pattern_plasma();
        break;
      case GEOMETRY:
        run_next_pattern_geometry();
        break;
      default:
        break;
    }

    // (2) draw the selection buttons and wait for user to select a different pattern category;   
    draw_three_buttons();   
    // calculate future time to end waiting for user to select other pattern category;
    timeOut = millis() + (1000 * BUTTONS_DISPLAY_DURATION_SECONDS);  
    timeOutEnabled = true; // enable time outs
    while (true) {
      // is_any_button_pressed is true only if TFT area corresponding to
      // buttons s pressed; if user presses outside the buttons areas, it
      // does not count;
      is_any_button_pressed();
      
      // check for timeout;
      if (checkForTimeout()) break;
    }
  
  } // for ever loop;
}

///////////////////////////////////////////////////////////////////////////////
//
// Pattern Display Infrastructure
//
///////////////////////////////////////////////////////////////////////////////

// Check for pattern timeout
// This must be called by every display pattern 
boolean checkForTimeout() 
{
  boolean timeOutCondition = timeOutEnabled && (millis() > timeOut);
  if ( !timeOutCondition) {
    return false;
  }
  timeOutEnabled = false;
  return true;
}


void run_next_pattern_fractal()
{
  // First clear the screen
  lcd.clrScr();
  // Then a short delay
  delay(1000);
  
  // Select next pattern to run
  id_pattern_fractal = (id_pattern_fractal < NUMBER_OF_PATTERNS_FRACTAL-1) ? 
    (id_pattern_fractal+1) : (0);

  // Calculate future time to switch pattern
  timeOut = millis() + (1000 * PATTERN_DISPLAY_DURATION_SECONDS);
  // Enable time outs
  timeOutEnabled = true;

  // Start up the selected pattern
  switch( id_pattern_fractal) {
  case 0:
    mandelbrotPattern();
    break;
  case 1:
    juliaPattern();
    break;
  default:
    break;
  }
}

void run_next_pattern_plasma()
{
  // First clear the screen
  lcd.clrScr();
  // Then a short delay
  delay(1000);
  
  // Select next pattern to run
  id_pattern_plasma = (id_pattern_plasma < NUMBER_OF_PATTERNS_PLASMA-1) ? 
    (id_pattern_plasma+1) : (0);

  // Calculate future time to switch pattern
  timeOut = millis() + (1000 * PATTERN_DISPLAY_DURATION_SECONDS);
  // Enable time outs
  timeOutEnabled = true;

  // start up the selected pattern
  switch( id_pattern_plasma) {
  case 0:
    plasma1Pattern();
    break;
  default:
    break;
  }
}

void run_next_pattern_geometry()
{
  // First clear the screen
  lcd.clrScr();
  // Then a short delay
  delay(1000);
  
  // Select next pattern to run
  id_pattern_geometry = (id_pattern_geometry < NUMBER_OF_PATTERNS_GEOMETRY-1) ? 
    (id_pattern_geometry+1) : (0);

  // Calculate future time to switch pattern
  timeOut = millis() + (1000 * PATTERN_DISPLAY_DURATION_SECONDS);
  // Enable time outs
  timeOutEnabled = true;

  // Start up the selected pattern
  switch( id_pattern_geometry) {
  case 0:
    concentricSquaresPattern();
    break;
  case 1:
    concentricCirclesPattern();
    break;   
  case 2:
    offsetCirclePattern();
    break;    
  case 3:
    arrayOfCirclesPattern();
    break;
  case 4:
    webPattern();
    break;
  case 5:
    spirographPattern();
    break;
  default:
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Display and Font Definitions and Variables
//
///////////////////////////////////////////////////////////////////////////////

#define WIDTH_LANDSCAPE  320
#define MIN_X_LANDSCAPE    0
#define MAX_X_LANDSCAPE  319
#define XMID (WIDTH_LANDSCAPE / 2)
#define HEIGHT_LANDSCAPE 240
#define MIN_Y_LANDSCAPE    0
#define MAX_Y_LANDSCAPE  239
#define YMID (HEIGHT_LANDSCAPE / 2)

#define BIG_FONT_WIDTH    16
#define BIG_FONT_HEIGHT   16


///////////////////////////////////////////////////////////////////////////////
//
// Color Constants, Variables and Functions
//
///////////////////////////////////////////////////////////////////////////////

#define NUM_OF_COLOR_VALUES 256
#define MIN_COLOR_VALUE       0  
#define MAX_COLOR_VALUE     255

struct RGB {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Color space conversion
// Input arguments
// hue in degrees (0 - 360.0)
// saturation in percent (0.0 - 1.0)
// value in percent (0.0 - 1.0)
// Output arguments
// red, green blue (0.0 - 1.0)
void _HSVtoRGB(float hue, float saturation, float value, float *red, float *green, float *blue) 
{
  int i;
  float f, p, q, t;
  if (saturation == 0) {
    // achromatic (grey)
    *red = *green = *blue = value;
    return;
  }
  hue /= 60;			// sector 0 to 5
  i = floor(hue);
  f = hue - i;			// factorial part of h
  p = value * (1 - saturation);
  q = value * (1 - saturation * f);
  t = value * (1 - saturation * (1 - f));
  switch(i) {
  case 0:
    *red = value;
    *green = t;
    *blue = p;
    break;
  case 1:
    *red = q;
    *green = value;
    *blue = p;
    break;
  case 2:
    *red = p;
    *green = value;
    *blue = t;
    break;
  case 3:
    *red = p;
    *green = q;
    *blue = value;
    break;
  case 4:
    *red = t;
    *green = p;
    *blue = value;
    break;
  default:
    *red = value;
    *green = p;
    *blue = q;
    break;
  }
}

// Create a HSV color and return it to an RGB structure
void createColor(float hue, float saturation, float value, struct RGB *color) 
{
  float r, g, b;

  _HSVtoRGB(hue, saturation, value, &r, &g, &b);
  (*color).red   = (uint8_t) (r * MAX_COLOR_VALUE);
  (*color).green = (uint8_t) (g * MAX_COLOR_VALUE);
  (*color).blue =  (uint8_t) (b * MAX_COLOR_VALUE);
}

// Create an HSV color
void createHSVColor(int divisions, int index, float saturation, float value, struct RGB *color) 
{
  float hueAngle = (360.0 * index) / divisions;
  createColor(hueAngle, saturation, value, color);
}

// Create a fully saturated HSV color
void createHSVColor(int divisions, int index, struct RGB *color) 
{
  index %= divisions;
  createHSVColor(divisions, index, 1.0, 1.0, color);
}


///////////////////////////////////////////////////////////////////////////////
//
// Palette definitions and Functions 
//
///////////////////////////////////////////////////////////////////////////////

#define PALETTE_SIZE      256
#define NUM_OF_PALETTES     7
#define GRAYSCALE_PALETTE   0
#define SPECTRUM_PALETTE    1
#define SIN1_PALETTE        2
#define SIN2_PALETTE        3
#define SIN3_PALETTE        4
#define SIN4_PALETTE        5
#define RANDOM_PALETTE      6

// Create a palette structure for holding color information
struct RGB palette[PALETTE_SIZE];

// Generate a palette based upon parameter
void generatePalette(int paletteNumber) 
{
  // Create some factors for randomizing the generated palettes
  // This helps keep the display colors interesting
  float f1 = random(16, 128);
  float f2 = random(16, 128);
  float f3 = random(16, 128);

  switch(paletteNumber) {
  case GRAYSCALE_PALETTE:
    {
      // Grayscale palette
      // Sometimes the light colors at low index; other times at high index
      boolean direction = (random(2) == 1);
      if (direction) {
        for (int i = 0; i < PALETTE_SIZE; i++) {
          palette[i].red = palette[i].green = palette[i].blue = i;
        }
      }  
      else  {
        for (int i = 0; i < PALETTE_SIZE; i++) {
          palette[i].red = palette[i].green = palette[i].blue = 255 - i;
        }
      }
    }
    break;

  case SPECTRUM_PALETTE:
    {
      // Full spectrum palette at full saturation and value
      for (int i = 0; i < PALETTE_SIZE; i++) {
        createHSVColor(PALETTE_SIZE, i, &palette[i]);
      }
    }
    break;

  case SIN1_PALETTE:
    {
      // Use sin function to generate palette
      for(int i = 0; i < PALETTE_SIZE; i++) {
        palette[i].red   = 128.0 + 128 * sin(PI * i / f1);
        palette[i].green = 128.0 + 128 * sin(PI * i / f2);
        palette[i].blue  = 128.0 + 128 * sin(PI * i / f3);
      } 
    }
    break;

  case SIN2_PALETTE:
    {
      // Use sin function to generate palette - no blue
      for(int i = 0; i < 256; i++) {
        palette[i].red   = 128.0 + 128 * sin(PI * i / f1);
        palette[i].green = 128.0 + 128 * sin(PI * i / f2);
        palette[i].blue  = 0;
      }
    } 
    break;

  case SIN3_PALETTE:
    {
      // Use sin function to generate palette - no green
      for(int i = 0; i < 256; i++) {
        palette[i].red   = 128.0 + 128 * sin(PI * i / f1);
        palette[i].green = 0;
        palette[i].blue  = 128.0 + 128 * sin(PI * i / f2);
      } 
    }
    break;

  case SIN4_PALETTE:
    {
      // Use sin function to generate palette - no red
      for(int i = 0; i < PALETTE_SIZE; i++) {
        palette[i].red   = 0;
        palette[i].green = 128.0 + 128 * sin(PI * i / f1);
        palette[i].blue  = 128.0 + 128 * sin(PI * i / f2);
      } 
    }
    break;

  case RANDOM_PALETTE:
    {
      // Choose random color components
      for(int i = 0; i < PALETTE_SIZE; i++) {
        palette[i].red   = random(NUM_OF_COLOR_VALUES);
        palette[i].green = random(NUM_OF_COLOR_VALUES);
        palette[i].blue  = random(NUM_OF_COLOR_VALUES);
      } 
    }
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// FRACTAL patterns
//
///////////////////////////////////////////////////////////////////////////////

// The following should probably be 256 but this lengthens mandelbrot generation 
// time substantially
#define MANDELBROT_STEPS 128

int pointInMandelbrotSet(float z0Real, float z0Imag, float cReal, float cImag) 
{
  float x = z0Real;
  float y = z0Imag;
  float nx, ny;
  for (int i = 0; i < MANDELBROT_STEPS; i++) {

    // Calculate the real part of the sequence
    nx = (x * x) - (y * y) + cReal;
    // Calculate the imaginary part of the sequence
    ny = 2 * x * y + cImag;
    // Check magnitude at each step
    // We check if it's greater than 2
    // which means the point diverges towards infinity
    if (((nx * nx) + (ny * ny)) > 4) {
      return i;
    }
    x = nx;
    y = ny;
  }
  // Point in mandelbrot set
  return MANDELBROT_STEPS;
}

// Draw Mandelbrot set
// Drawing occurs on black background unless paintBackground is true
void drawMandelbrotSet(int paletteNumber, boolean paintBackground, float scale) 
{
  // Generate specified palette
  generatePalette(paletteNumber);

  if (paintBackground) {
    int colorIndex = random(PALETTE_SIZE);
    lcd.fillScr(palette[colorIndex].red, palette[colorIndex].green, palette[colorIndex].blue);
  }

  float xOfs = WIDTH_LANDSCAPE  / 2.0;
  float yOfs = HEIGHT_LANDSCAPE / 2.0;

  for (int x = 0; x < WIDTH_LANDSCAPE; x++) {
    float newReal = scale * ((float) (x - xOfs) / xOfs);

    for (int y = 0; y < HEIGHT_LANDSCAPE; y++) {
      float newImag = scale * ((float) (y - yOfs) / yOfs);

      int iterations = pointInMandelbrotSet(0, 0, newReal, newImag);
      if (iterations != MANDELBROT_STEPS) {
        // Map 128 iterations into 256 colors
        iterations *= 2;

        // Retrieve color from current palette
        lcd.setColor(palette[iterations].red, palette[iterations].green, palette[iterations].blue);
        lcd.drawPixel(x, y);
      }
    }
  }
}

// 0
void mandelbrotPattern() 
{
  float scale = 1.5;

  // Pick a random scale factor
  int scaleFactor = random(5);
  switch (scaleFactor) {
  case 0:
    scale = 1.5;
    break;
  case 1:
    scale = 1.25;
    break;
  case 2:
    scale = 1.1;
    break;
  case 3:
    scale = 2.0;
    break;
  case 4:
    scale = 2.5;
    break;
  }
  // Determine if mandelbrot set members are colored or black
  boolean paintBackground = (random(2) == 1);

  // Draw the set with specified scale and palette
  drawMandelbrotSet(random(NUM_OF_PALETTES), paintBackground, scale);

  // admire it for a sec!
  delay(1000); 
}


// Generate a random signed 10 digit floating point value;
// used for julia;
float generateRandomFloat() 
{
  char str[12];
  memset(str, 0, sizeof(str));
  int index = 0;

  boolean isPositive = (random(2) == 1);
  if (isPositive) {
    str[index] = ' ';
  }  
  else  {
    str[index] = '-';
  }
  index++;
  str[index++] = '.';

  for (int i = 0; i < 9; i++) {
    int offset = random(10);
    str[index++] = '0' + offset;
  }
  return atof(str); 
}

// Draw julia set
void drawJuliaSet(int paletteNumber, float cReal, float cImag) {
  // Generate specified palette
  generatePalette(paletteNumber);

  float xOfs = WIDTH_LANDSCAPE  / 2.0;
  float yOfs = HEIGHT_LANDSCAPE / 2.0;

  for (int x = 0; x < WIDTH_LANDSCAPE; x++) {
    float newReal = (((float) x - xOfs) / xOfs);

    for (int y = 0; y < HEIGHT_LANDSCAPE; y++) {
      float newImag = (((float) y - yOfs) / yOfs);

      int iterations = pointInMandelbrotSet(newReal, newImag, cReal, cImag);
      if (iterations != MANDELBROT_STEPS) {
        // Map 128 iterations into 256 colors
        iterations *= 2;

        // Retrieve color from current palette
        lcd.setColor(palette[iterations].red, palette[iterations].green, palette[iterations].blue);
        lcd.drawPixel(x, y);
      }
    }
  }
}

// 1
void juliaPattern() 
{
  // Generate a complex constant as seed for Julia set
  float cReal = generateRandomFloat();
  float cImag = generateRandomFloat();

  // Make sure this complex constant diverges at (0, 0)
  // This prevents some not so interesting Julia sets from being (slowly) rendered
  while (pointInMandelbrotSet(0, 0, cReal, cImag) == MANDELBROT_STEPS) {
    cReal = generateRandomFloat();
    cImag = generateRandomFloat();
  }

  // Draw the hopefully interesting Julia set
  drawJuliaSet(random(NUM_OF_PALETTES), cReal, cImag);

  // admire it for a sec!
  delay(1000);
}


///////////////////////////////////////////////////////////////////////////////
//
// PLASMA patterns
//
///////////////////////////////////////////////////////////////////////////////

#define NUM_OF_PLASMAS  4
#define PLASMA_TYPE_0   0
#define PLASMA_TYPE_1   1
#define PLASMA_TYPE_2   2
#define PLASMA_TYPE_3   3

void drawPlasma(int plasmaType, int paletteNumber) 
{
  int colorIndex;
  float value = 0;

  // Generate specified palette
  generatePalette(paletteNumber);

  // Generate some float factors to alter plasma
  float f1 = (float) random(2, 64);
  float f2 = (float) random(2, 64);
  float f3 = (float) random(2, 64);

  for (int x = 0; x < WIDTH_LANDSCAPE; x++) {
    for (int y = 0; y < HEIGHT_LANDSCAPE; y++) {

      // Determine plasma type
      switch (plasmaType) {

      case PLASMA_TYPE_0:
        {
          value = sin(sqrt((x - XMID) * (x - XMID) + (y - YMID) * (y - YMID)) / f1);
        }
        break;

      case PLASMA_TYPE_1:
        {
          value = (sin(x / f1) + sin(y / f1)) / 2.0;
        }
        break;

      case PLASMA_TYPE_2:
        {
          value = (sin(x / f1) + sin(y / f2) + sin((x + y) / f3)) / 3.0;
        }
        break;

      case PLASMA_TYPE_3:
        {
          value  = sin(x / f1);
          value += sin(y / f2);
          value += sin(sqrt(((x - XMID) * (x - XMID)) + ((y - YMID) * (y - YMID))) / f3);
          value /= 3.0;
        }
        break;
      }
      // Scale -1 ... +1 values to 0 ... 255
      value = (value * 128.0) + 128.0;
      colorIndex = ((int) value) % 256;
      lcd.setColor(palette[colorIndex].red, palette[colorIndex].green, palette[colorIndex].blue);
      lcd.drawPixel(x, y);
    }
  }
}

// 0
void plasma1Pattern() 
{
  while (true) {
    int plasmaType = random(NUM_OF_PLASMAS);
    int paletteNumber = random(NUM_OF_PALETTES);

    // Draw the specified plasma with the specified palette
    drawPlasma(plasmaType, paletteNumber);

    // Check for timeout
    if (checkForTimeout()) break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// GEOMETRY patterns
//
///////////////////////////////////////////////////////////////////////////////

// 0
void concentricCirclesPattern() 
{
  int radius;
  int radiusInc = 1;

  // Pick a random palette
  int paletteNumber = random(NUM_OF_PALETTES);
  generatePalette(paletteNumber);

  // Pick a random place to start within the palette
  int colorIndex = random(PALETTE_SIZE);

  while (true) {
    radius = 2;

    while ((radius < XMID) && (radius < YMID)) {

      lcd.setColor(palette[colorIndex].red, palette[colorIndex].green, palette[colorIndex].blue); 
      colorIndex++;
      colorIndex %= PALETTE_SIZE;

      lcd.drawCircle(XMID, YMID, radius);
      radius += radiusInc;

      delay(50);
    }
    // Check for timeout
    if (checkForTimeout()) break;
  }
}

// 1
void concentricSquaresPattern() 
{
  int colorIndex = random(PALETTE_SIZE);
  int x1, x2, y1, y2;
  int xInc = 3;
  int yInc = 2;

  int paletteNumber = random(NUM_OF_PALETTES);
  generatePalette(paletteNumber);

  while (true) {
    // Set initial coordinates
    x1 = XMID - 3;
    x2 = XMID + 3;
    y1 = YMID - 1;
    y2 = YMID + 1;

    while ((x1 >= 0) && (x2 < WIDTH_LANDSCAPE) &&
      (y1 >= 0) && (y2 < HEIGHT_LANDSCAPE)) {

      lcd.setColor(palette[colorIndex].red, palette[colorIndex].green, palette[colorIndex].blue); 
      colorIndex++;
      colorIndex %= PALETTE_SIZE;

      lcd.drawRect(x1, y1, x2, y2);
      x1 -= xInc;
      x2 += xInc;
      y1 -= yInc;
      y2 += yInc;

      delay(50);
    }
    // Check for timeout
    if (checkForTimeout()) break;
  }
}

// 2
void offsetCirclePattern() 
{
  struct RGB color;
  boolean even = (random(2) == 0);

  while (true) {
    int numberOfColors = random(NUM_OF_COLOR_VALUES);
    int colorIndex = random(numberOfColors);
    int radiusIncrement = random(4, 14);
    int delayTime = random(20, 100);

    for (int radius = 1; radius < YMID; radius += radiusIncrement) {
      // Create and set color
      createHSVColor(numberOfColors, colorIndex++, &color);
      lcd.setColor(color.red, color.green, color.blue);

      if (even) {
        lcd.drawCircle(radius , YMID, radius);
      }  
      else  {
        lcd.drawCircle(radius, radius, radius);
      }
      delay(delayTime);

      // Create and set color
      createHSVColor(numberOfColors, colorIndex++, &color);
      lcd.setColor(color.red, color.green, color.blue);
      if (even) {
        lcd.drawCircle(WIDTH_LANDSCAPE - radius , YMID, radius);
      }  
      else  {
        lcd.drawCircle(WIDTH_LANDSCAPE - radius, radius, radius);
      }
      delay(delayTime);

      // Create and set color
      createHSVColor(numberOfColors, colorIndex++, &color);
      lcd.setColor(color.red, color.green, color.blue);
      if (even) {
        lcd.drawCircle(XMID , radius, radius);
      }  
      else  {
        lcd.drawCircle(radius, HEIGHT_LANDSCAPE - radius - 1, radius);
      }
      delay(delayTime);

      // Create and set color
      createHSVColor(numberOfColors, colorIndex++, &color);
      lcd.setColor(color.red, color.green, color.blue);
      if (even) {
        lcd.drawCircle(XMID , HEIGHT_LANDSCAPE - radius - 1, radius);
      }  
      else  {
        lcd.drawCircle(WIDTH_LANDSCAPE - radius, HEIGHT_LANDSCAPE - radius - 1, radius);
      }
      delay(delayTime);
    }
    even = ! even;

    // Check for timeout
    if (checkForTimeout()) break;
  }
}

// 3
void arrayOfCirclesPattern() 
{
  // Fit three rows of four circles onto the display
  #define HBW    16
  #define VBW    15
  #define RADIUS 30

  while (true) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 4; col++) {

        // Create random color for the circle
        lcd.setColor(random(NUM_OF_COLOR_VALUES), random(NUM_OF_COLOR_VALUES), random(NUM_OF_COLOR_VALUES));

        // Calculate location of circle
        int x = (col + 1) * (HBW + RADIUS) + (col * RADIUS);
        int y = (row + 1) * (VBW + RADIUS) + (row * RADIUS);

        // Draw the filled circle
        lcd.drawCircle(x, y, RADIUS);
      }
    }
    // Check for termination
    if (checkForTimeout()) break;
  }
}

// 4
void webPattern() 
{
  #define ITERATIONS 16

  struct RGB color;
  int colorIndex = 0;

  while (true) {
    createHSVColor(ITERATIONS, colorIndex++, &color);
    lcd.setColor (color.red, color.green, color.blue);

    for (int y = 0; y < 240; y += 5) {
      lcd.drawLine(0, y, y * 1.33, 239);
    }    

    createHSVColor(ITERATIONS, colorIndex++, &color);
    lcd.setColor (color.red, color.green, color.blue);

    for (int y = 239; y >= 0; y -= 5) {
      lcd.drawLine(319, y, y * 1.33, 0);
    }

    createHSVColor(ITERATIONS, colorIndex++, &color);
    lcd.setColor (color.red, color.green, color.blue);

    for (int y = 239; y >= 0; y -= 5) {
      lcd.drawLine(0, y, 320 - (y * 1.33), 0);
    }

    createHSVColor(ITERATIONS, colorIndex++, &color);
    lcd.setColor (color.red, color.green, color.blue);

    for (int y = 0; y < 240; y += 5) {
      lcd.drawLine(319, y, 320 - (y * 1.33), 239);
    }

    // Check for termination
    if (checkForTimeout()) break;
  }
}

// Spirograph Functions
// 5
void spirographPattern() 
{
  while (true) {
    drawSpirograph();

    // Check for termination
    if (checkForTimeout()) break;
  }
}

// Greatest common divisor using Euclid's method
int gcd(int x, int y) 
{
  if (x % y == 0) {
    return y;
  }
  return gcd(y, x % y);
}

// Compute revolutions to complete spirograph pattern
// computed from radaii of fixed and moving circles
int revolutions(int R, int r) 
{
  return (int) (r / gcd(R, r)) * 7;
}

void drawSpirograph() 
{
  // Pick parameters randomly

  // Pick moving circle position: inside or outside
  boolean mcInside = (random(2) == 1);

  // Pick fcRadius and mcRadius accordingly
  int fcRadius, mcRadius;

  if (mcInside) {
    // Moving circle inside fixed circle
    fcRadius = random(100, HEIGHT_LANDSCAPE / 2);

    // Moving circle radius must be smaller than fixed circle radius
    mcRadius = random(46, fcRadius);
  }  
  else  {
    // Moving circle outside fixed circle
    fcRadius = random(40, 65);
    mcRadius = random(26, fcRadius);

    while ((120 + fcRadius + (2 * mcRadius)) >= HEIGHT_LANDSCAPE) {
      fcRadius = random(40, 65);
      mcRadius = random(26, fcRadius);
    }
  }

  // Pick moving circle offset
  int mcOffset = random(10, mcRadius);

  // Pick color mode: single, multi or random
  int colorMode = random(3);

  // Pick drawing mode: point or line
  boolean drawingModePoint = (random(2) == 1);

  // Calculate iterations to perform
  int iterations = revolutions(fcRadius, mcRadius);

  int redBits   = random(NUM_OF_COLOR_VALUES);
  int greenBits = random(NUM_OF_COLOR_VALUES);
  int blueBits  = random(NUM_OF_COLOR_VALUES);

  int x = 0;
  int y = 0;
  int prevx;
  int prevy;

  // Controls the step size; low values of r (e.g. 2) make jagged pattern, so s will be small
  // for low value of r; the effect is not directly proportinal, but sqrt (arbitrary)
  int exp = random(0, 5);
  int scale = pow(10, exp);
  float scaleFactor = 4.0 / scale;
  float s = scaleFactor * sqrt(mcRadius);

  int xOffset = WIDTH_LANDSCAPE  / 2;
  int yOffset = HEIGHT_LANDSCAPE / 2;

  float rSum  = fcRadius + mcRadius;
  float rDiff = fcRadius - mcRadius;

  // For multiColor, step to change for each color 
  int redStep = 1;
  int greenStep = 1;
  int blueStep = 1;

  for (float t = 0; t <= iterations; t += s) {

    prevx = x;
    prevy = y;

    // Determine color mode ?
    switch (colorMode) {
    case 0:
      // Single color mode. Use previous selected color every iteration
      break;

    case 1:
      {
        // Multi color mode
        // Randomly change one color in each iteration; makes more interesting patterns
        int whichColor = random(3); 

        switch (whichColor) {
        case 0:
          {
            if (redBits < 1) {
              redStep = 1;
            }
            if (redBits > 254) {
              redStep = -1;
            }
            redBits += redStep;
          }
          break;

        case 1:
          {
            if (greenBits < 1) {
              greenStep = 1;
            }
            if (greenBits > 254) {
              greenStep = -1;
            }
            greenBits += greenStep;
          }
          break;

        case 2:
          {
            if (blueBits < 1) {
              blueStep = 1;
            }
            if (blueBits > 254) {
              blueStep = -1;
            }
            blueBits += blueStep;
            break;
          }
        }
        break;
      }

    case 2:
      {
        // Random color mode 
        redBits   = random(NUM_OF_COLOR_VALUES);
        greenBits = random(NUM_OF_COLOR_VALUES);
        blueBits  = random(NUM_OF_COLOR_VALUES);
      }
      break;
    }
    // Select new drawing color
    lcd.setColor(redBits, greenBits, blueBits);

    if (mcInside) {
      // Moving circle is inside the fixed circle
      float exprResult = (rDiff * t) / (float) mcRadius;

      x = (int) ((rDiff * cos(t)) + (mcOffset * cos(exprResult)) + xOffset);
      y = (int) ((rDiff * sin(t)) - (mcOffset * sin(exprResult)) + yOffset);

    }  
    else  {

      // Moving circle is outside the fixed circle
      float exprResult = (rSum * t) / (float) mcRadius;
      x = (int) ((rSum * cos(t)) - (mcOffset * cos(exprResult)) + xOffset);
      y = (int) ((rSum * sin(t)) - (mcOffset * sin(exprResult)) + yOffset);
    }
    // Do drawing
    if (drawingModePoint) {
      // Drawing individual points
      lcd.drawPixel(x, y);
    }  
    else  {
      // Drawing lines
      if (t > 0) {
        lcd.drawLine(prevx, prevy, x, y);
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Functions related to TFT buttons
//
///////////////////////////////////////////////////////////////////////////////

void draw_three_buttons()
{
  // draw three buttons that will be drawn on the TFT display only
  // after a certain effect is done rendering; user can press any to
  // start iterating thru that type of effects;
  lcd.setColor(0, 0, 255);
  lcd.fillRoundRect (20,10, 90,30);
  lcd.fillRoundRect (120,10, 190,30);
  lcd.fillRoundRect (220,10, 290,30);
  lcd.setColor(255, 255, 255);
  lcd.drawRoundRect (20,10, 90,30);
  lcd.drawRoundRect (120,10, 190,30);
  lcd.drawRoundRect (220,10, 290,30);
  lcd.setBackColor(0, 0, 255);
  lcd.print("Fractals", 24, 14);
  lcd.print("Plasmas", 124, 14);
  lcd.print("Geometry", 224, 14);
}

boolean is_any_button_pressed()
{
  // check if any of the buttons is pressed; if yes, mark it with red outline
  // and record which one so that next effect displayed will be from the
  // approapriate pool of effects;
  
  if ( myTouch.dataAvailable()) {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY();

    // debug;
    lcd.printNumI(x, 160,80);
    lcd.printNumI(y, 160,100);
    
    if ((y>=10) && (y<=30)) { // row of buttons range;
      if ((x>=20) && (x<=90)) { // Button 1: Fractals
        highlight_red_button_pressed(20,10, 90,30);
        lcd.print("F", 160, 120);
        pattern_to_render_next = FRACTAL;
        return true;
      } else if ((x>=120) && (x<=190)) { // Button 2: Plasmas
        highlight_red_button_pressed(120,10, 190,30);
        lcd.print("P", 160, 120);
        pattern_to_render_next = PLASMA;
        return true;
      } else if ((x>=220) && (x<=290)) { // Button 3: Geometry
        highlight_red_button_pressed(220,10, 290,30);
        lcd.print("G", 160, 120);
        pattern_to_render_next = GEOMETRY;
        return true;
      } else {
        // will render the previous type of pattern;
        return false;
      }
    }  
  }
  return false;
}

void highlight_red_button_pressed(int x1,int y1, int x2,int y2)
{
  // draw a red frame while a button is touched; coordinates of any 
  // of the three possible buttons will be provided thru arguments;  
  lcd.setColor(255, 0, 0);
  lcd.drawRoundRect (x1,y1, x2,y2);
}
