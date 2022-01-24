// Filename: lcd.h
// Description: Module to control the color 128x128 TFT LCD display on Educational BoosterPack MKII
// Author(s): Robert Trost, edited by Jimmy Bates (dec 2 2020)
// Set: N/A
// Date: Feb 20, 2016 (created)

#ifndef LCD_H_
#define LCD_H_

// dimensions of the LCD
#define LCD_MAX_X 127  // x coordinate 0 to 127
#define LCD_MAX_Y 127  // y coordinate 0 to 127

// A few basic colour options to output to the LCD
// the LCD is configured in 16 bit colour mode (64K possible colours)
// bits 0 to 4 control blue intensity
// bits 5 to 10 control green instensisty
// bits 11 to 15 control red intensity
#define BLACK    (0x0000)
#define WHITE    (0xFFFF)
#define RED      (0xF800)
#define GREEN    (0x07E0)
#define BLUE     (0x001F)
#define YELLOW   (RED | GREEN)
#define CYAN     (GREEN | BLUE)
#define MAGENTA  (RED | BLUE)

///////////////////////////////////////////////////////////////////////
// lcdClear - Clears the entire LCD display
// Arguments: colour - A 16 bit value to represent the RGB colour to be
//                     clear all pixels to
// Return Value: none
///////////////////////////////////////////////////////////////////////
void lcdClear(int colour);

///////////////////////////////////////////////////////////////////////
// lcdInit - Initializes ports, SPI settings and LCD.
//           This function assumes CPU clock is less than 15MHz.
// Arguments: none
// Return Value: none
///////////////////////////////////////////////////////////////////////
void lcdInit();

///////////////////////////////////////////////////////////////////////
// lcdSetPixel - Sets the pixel colour at the specified display location.
//               The bottom left corner of the LCD is (0,0)
// Arguments: x - The horizontal location of the pixel
//            y - The vertical location of the pixel
//            colour - A 16 bit value to represent the RGB colour to be
//                     output at the pixel location
// Return Value: none
///////////////////////////////////////////////////////////////////////
void lcdSetPixel(int x, int y, int colour);

#endif /* LCD_H_ */
