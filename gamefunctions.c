//  gamefunctions.h - File to keep game functions
//  Author: Jimmy Bates
//  Set: T
//  Date: December 2, 2020

#include "msp.h"
#include "gamefunctions.h"
#include "lcd.h"
#include "adc.h"
#include <math.h>


/////////////////////////////////////////////////////
//  jsLcdConv - Outputs a value for the LCD screen from the ADC value of the Joystick
//  Arguments: adcValue - JS value from ADC (Either inverted or non-inverted)
//  Return value: int for the JS-LDC conversion
/////////////////////////////////////////////////////
int jsLcdConv(int adcValue) {

    // Adjust boundaries of ADC so we don't get LCD values below 0 or above 127
    if(adcValue<JS_MIN) {
        adcValue=JS_MIN;
    } else if(adcValue>JS_MAX) {
        adcValue=JS_MAX;
    } // end boundaries if-else

    // Return Equation of JS to ADC equation, round so it doesn't truncate
    return round((adcValue - JS_MIN) * (LCD_MAX - LCD_MIN) / (JS_MAX - JS_MIN));

} // end jsLcdConv


/////////////////////////////////////////////////////
//  writePedal - Void - writes the pedal on the screen based on the input x value from JS
//  Arguments:
//      pedalX - current JS value from ADC;
//      pedalX_Prev - previous JS value from ADC (to erase)
//  Return value: none allowed
/////////////////////////////////////////////////////
void writePedal(int pedalX, int pedalX_prev) {

    int xIndex; // Keep track of X value of LCD writing of pedal (pedalX-10 - pedalX+10)
    int yIndex; // Keep track of Y value of LCD writing of pedal (LCD_MIN - LCD_MIN+4)

    int pedalColour; // Holds value of what colour pixel should be written for pedal (if border, black, else red)

    // Write Y array of pedal
    for(yIndex=PEDAL_Y; yIndex<(PEDAL_Y+PEDAL_HEIGHT); yIndex++) {

        // Erase previous line
        for(xIndex=(pedalX_prev-PEDAL_WIDTH); xIndex<=(pedalX_prev+PEDAL_WIDTH); xIndex++) {
            lcdSetPixel(xIndex, yIndex, BG_COLOR); // Write to LCD the pixel of BG colour to previous pedal
        } // end pedalX_prev for

        // Write X array of pedal
        for(xIndex=(pedalX-PEDAL_WIDTH); xIndex<=(pedalX+PEDAL_WIDTH); xIndex++) {


           // IF: pixel is not on border, red
           if(xIndex>(pedalX-PEDAL_WIDTH+1) && xIndex<(pedalX+PEDAL_WIDTH-1) && yIndex<(PEDAL_Y+PEDAL_HEIGHT-1))
               pedalColour = PEDAL_INSIDE;
           // Otherwise, write the outline which is black
           else
               pedalColour = PEDAL_OUTLINE;

           lcdSetPixel(xIndex, yIndex, pedalColour); // Write to LCD the pixel of pedal pixel

        } // end X array for current pedal for
    } // end Y array pedal for
} // end writePedal


/////////////////////////////////////////////////////
//  writeBall - Void - writes the ball on the screen based on the input x and y values (or, erases)
//  Arguments:
//      ballX - x value of center of ball
//      ballY - y value of center of ball
//      erase - if set to ERASE, erase, if set to WRITE, write the ball
//  Return value: none allowed
/////////////////////////////////////////////////////
void writeBall(int ballX, int ballY, int ballErase) {

    signed int ballColour, ballY_Max, xIndex, yIndex; // Declare variables for ball parameters

    // X array for Ball
    for(xIndex=(0-BALL_RADIUS); xIndex<=(BALL_RADIUS); xIndex++) {

        // Calculate value for Y MIN and Y MAX (Y MIN just equals negative Y MAX)
        ballY_Max=sqrt(( BALL_RADIUS * BALL_RADIUS ) - ( xIndex * xIndex ));

        // Y array for Ball
        for(yIndex=(0-ballY_Max); yIndex<=ballY_Max; yIndex++) {

            // If function is set to erase, write BG colour to erase
            if(ballErase==ERASE)
                ballColour = BG_COLOR;
            //Else if y index or x index is not on 1px border, ball is white
            else if( (yIndex<(ballY_Max-1)) && (yIndex>(0-ballY_Max+1)) && (xIndex<(BALL_RADIUS-1)) && (xIndex>(0-BALL_RADIUS+1)) && ballErase==WRITE)
                ballColour = BALL_INSIDE;
            //Else, 2px ball outline which is black
            else
                ballColour = BALL_OUTLINE;

            // Write ball pixels
            lcdSetPixel( (ballX+xIndex) , (ballY+yIndex), ballColour);

        } // end for: Y array for Ball

    } // end for: X array for Ball

}


/////////////////////////////////////////////////////
//  msDelay -- Function to generate delay
//  Arguments: ms - Number of milliseconds to delay
//  Return value: none
/////////////////////////////////////////////////////
void msDelay(unsigned int ms) {
    //Delay CLOCKRATE which is 1 millisecond of cycles until ms reaches 0
    while (ms>0) {
        __delay_cycles(CLOCKRATE);
        ms--;
    }
} // End msDelay
