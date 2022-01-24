//  gamefunctions.h - File to keep game functions
//  Author: Jimmy Bates
//  Set: T
//  Date: December 2, 2020

#ifndef GAMEFUNCTIONS_H_
#define GAMEFUNCTIONS_H_

// LCD

#define LCD_MAX         127                 //  Max of LCD value (pixel)
#define LCD_MIN         0                   //  Min of LCD value (pixel)

// ADC

#define JS_MAX          16384               //  Max of JS ADC value
#define JS_MIN          0                   //  Min of JS ADC value

// Colours

#define BG_COLOR        CYAN                // Cyan background

#define BALL_INSIDE     WHITE               // White inside of ball
#define BALL_OUTLINE    BLACK               // Black outline of ball

#define PEDAL_INSIDE    RED                 // Colour of pedal inside
#define PEDAL_OUTLINE   BLACK               // Colour of pedal outline

// Dimensions and spacing

#define PEDAL_Y         0                   //  POSITION Y of the pedal, to catch the balls
#define PEDAL_WIDTH     12                  //  WIDTH of the pedal from center (so width is actually 2x this + 1)
#define PEDAL_HEIGHT    3                   //  HEIGHT of pedal from bottom

#define BALL_RADIUS     4                  //  Radius of ball

// Functional definitions

#define ERASE           1                   //  Erase pedal or ball
#define WRITE           0                   //  Write pedal or ball

#define DEBOUNCE        10                  //  Define debounce value
#define CLOCKRATE       3000                //  Define clockrate for msDelay (3E6/1000Hz)

/////////////////////////////////////////////////////
//  jsLcdConv - Outputs a value for the LCD screen from the ADC value of the Joystick
//  Arguments: adcValue - JS value from ADC
//  Return value: int for the JS-LDC conversion
/////////////////////////////////////////////////////
int jsLcdConv(int adcValue);


/////////////////////////////////////////////////////
//  writePedal - Void - writes the pedal on the screen based on the input x value from JS
//  Arguments:
//      pedalX - current JS value from ADC;
//      pedalX_Prev - previous JS value from ADC (to erase)
//  Return value: none allowed
/////////////////////////////////////////////////////
void writePedal(int pedalX, int pedalX_prev);


/////////////////////////////////////////////////////
//  writeBall - Void - writes the ball on the screen based on the input x and y values (or, erases)
//  Arguments:
//      ballX - x value of center of ball
//      ballY - y value of center of ball
//      erase - if set to ERASE, erase, if set to WRITE, write the ball
//  Return value: none allowed
/////////////////////////////////////////////////////
void writeBall(int ballX, int ballY, int ballErase);


/////////////////////////////////////////////////////
//  msDelay -- Function to generate delay
//  Arguments: ms - Number of milliseconds to delay
//  Return value: none
/////////////////////////////////////////////////////
void msDelay(unsigned int ms);


#endif /* GAMEFUNCTIONS_H_ */
