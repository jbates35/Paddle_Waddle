/************************************************************************
 *  MSP432  - ELEX 3305  - Lab 10                                       *
 *  Written by Jimmy Bates (SID A01035957, Set T)                       *
 *  December 2 2020                                                     *
 *                                                                      *
 * Final lab - This is a game that spawns balls from the top,           *
 * that you have to catch with a paddle (or pedal as I referred to      *
 * when thinking of what I'd call it at 2:00AM)                         *
 *                                                                      *
 * S1 will pause the game                                               *
 * S2 will restart the game                                             *
 *                                                                      *
 ***********************************************************************/


#include "msp.h"
#include "lcd.h"
#include "adc.h"
#include "i2c.h"
#include "gamefunctions.h"
#include <math.h>
#include <stdlib.h>

#define JS_X            BIT0                //  Joystick X input bit P6.0
#define JS_X_AIN        15                  //  Joystick X analog input 15

#define S1              BIT1                //  Define S1 button bit (P5.1)
#define S1_NVIC         PORT5_IRQn          //  Define S1 NVIC
#define S1_ACTIVE       !(P5->IN&S1)        //  Define S1 button active

#define S2              BIT5                //  Define S2 button bit (P3.5)
#define S2_NVIC         PORT3_IRQn          //  Define S2 NVIC
#define S2_ACTIVE       !(P3->IN&S2)        //  Define S2 button active

// Dimensions and measurements

#define BALL_Y_SPAWN        (129+BALL_RADIUS)       // The Y value the ball spawns
#define BALL_Y_BOTTOM       (LCD_MIN-BALL_RADIUS)   // The lowest Y value the ball drops
#define BALL_Y_DROP         7                       // Amount of pixels it drops per increment
#define PEDAL_Y_HIT         -4                      // Pixel as to which to see where ball is (x-wise) to see if hit or miss)

#define BALL_X_START_RANGE  (92-BALL_RADIUS)        // This gets multiplied by random
#define BALL_X_START_MIN    (18+BALL_RADIUS)        // Lowest value ballX can START

#define BALL_X_ANGLE        5                       // Different angles it can go
#define BALL_X_DIRECTIONS   2                       // 2 directions
#define BALL_X_LEFT         -1                      // Ball is going left

// States

#define GAME_ON         1                   //  Meaning game continue
#define GAME_OFF        0                   //  Meaning game end

#define START_LEVEL     0                   // Start level
#define END_LEVEL       4                   // End level

#define HITS_PER_LEVEL  5                  // Amount of hits before level decrements


// Timings

#define MS_PERIOD       3000                // Cycles per 1 ms

#define DROP_INTERVAL   40000               // Period for ball drop timer (6.25s)
#define DROP_SUBTRACT   8000                // Subtract this, multiplied by level, from period

// Global variables

int ballY;                  // Y value for center of ball
int ballX;                  // X value for center of ball
signed int ballX_AngleMax;  // int for keeping track of angle of ball
int ballX_AngleCount;       // counter that keeps track of if the ball should move
int ballX_Direction;        // Keeps track whether ball is angling right or left or straight
volatile int pedalX;        // Stores x value of Joystick, converted into LCD, for the center of the pedal
volatile int pedalX_prev;   // Stores old x value of Joystick, to compare and erase pedal if needed
int gameState;              // Determines whether game is still on
int hitsCounter;            // counter that keeps track of how many hits
int level;                  // counter that keeps track of level - starts at 0, ends at 4, increments every 10 hits
int timerCCR;               // value of timer that will get divided each level change


/////////////////////////////////////////////////////
//  initGame - Void - (re)initializes all game parameters
//  Arguments: none
//  Return value: none allowed
/////////////////////////////////////////////////////
void initGame();


/////////////////////////////////////////////////////
//  ballSpawn - Void - respawns the ball at the top of the screen
//  Arguments: none
//  Return value: none allowed
/////////////////////////////////////////////////////
void ballSpawn();


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    // Initalize LCD
    lcdInit();

    // Initialize i2C
    i2cInit();

    // Initalize optical sensor
    i2cWrite(OPT3001_SLAVE_ADDRESS, OPT3001_CONFIG_REG, OPT3001_CONFIG_100);

    // Initialize random
    srand(i2cRead(OPT3001_SLAVE_ADDRESS, OPT3001_RESULT_REG));

    // Set bits for joystick X (P6.0)
    P6->SEL1 |= JS_X;
    P6->SEL0 |= JS_X;

    // Setup S1 Button P5.1 (Active low, normally closed)
    P5->DIR &= ~S1;

    //Setup S1 Button Interrupt (Falling edge)
    P5->IE |= S1;
    P5->IES |= S1;

    // Setup S2 Button P3.5 (Active low, normally closed)
    P3->DIR &= ~S2;

    // Setup S2 Button Interrupt (Falling edge)
    P3->IE |= S2;
    P3->IES |= S2;

    // In ADC14CTL registers:
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; //Clear ADC14 Conversion bit
    ADC14->CTL0 |= ADC14_CTL0_ON;   //Turn ADC14 on
    ADC14->CTL0 |= ADC14_CTL0_SHP;  //Turn ADC14 Sample and hold pulse mode
    ADC14->CTL0 |= ADC14_CTL0_ENC;  //Set ADC14 Conversion bit to enable

    // Timer for dropping the ball, /16, up mode, with period of 160ms
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__8 | TIMER_A_CTL_MC__UP | TIMER_A_CTL_IE;
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__2;
    TIMER_A0->CCR[0] = DROP_INTERVAL-1;

    // Enable NVIC for interrupts
    NVIC_EnableIRQ(S1_NVIC);    // S1 Button NVIC
    NVIC_EnableIRQ(S2_NVIC);    // S2 Button NVIC
    NVIC_EnableIRQ(TA0_N_IRQn); // Timer_A0 NVIC

    // Enable global interrupt enable
    _enable_interrupts();

    // Initialize all game parameters
    initGame();


    // Loop forever
    while(1) {

        //Make sure game is on!
        if(gameState == GAME_ON) {

            adcInit(JS_X_AIN);                 // Select input, Joystick X-axis (AIN 15)
            pedalX = jsLcdConv(adcSample());   // Joystick X converted to LDC value

            //If there is a change in the x value of the joystick, write a new pedal value
            if(pedalX!=pedalX_prev) {

                writePedal(pedalX, pedalX_prev);    // Write new pedal on screen
                pedalX_prev = pedalX;               // Update the value of pedalX

            } // end if: new ADC value for pedalX
        } // end if: gameState = ON
    } // end while: Loop forever
} // end Main


////////////////////////////////////////////////////////////
//  Timer_A0 TAIFG ISR - Takes care of ball dropping - Also keeps score, ranges from 213ms at level 0 -> 56ms at level 4
//  Arguments: None allowed
//  Return values: None
////////////////////////////////////////////////////////////
void TA0_N_IRQHandler(void) {

    //clear Timer flag
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;

    //Make sure game is on!
    if(gameState == GAME_ON) {

        //Erase ball
        writeBall(ballX, ballY, ERASE);

        //Decrement BALL Y value by 5
        ballY -= BALL_Y_DROP;

        //Increment ball angle counter
        ballX_AngleCount++;

        //If ballX angle counter has reached its max count
        if(ballX_AngleCount>ballX_AngleMax) {
            ballX += ballX_Direction; // Increment ballX either to the left, nothing, or right
            ballX_AngleCount=0; // Reset angle counter
        } // end if: ballX angle counter reaches max

        //If ball reaches the height at which the pedal is ...
        if((ballY-BALL_RADIUS)==PEDAL_Y_HIT) {

            //To make things easier, combine the values of the ball and pedal width
            int ballRange = BALL_RADIUS + PEDAL_WIDTH;

            //Hit collision detector:
            //Check to see if ball is within the range of x values it should be
            if(pedalX<=(ballX+ballRange) && pedalX>=(ballX-ballRange)) {

                //Respawn ball at top of screen with diff parameters
                ballSpawn();

                //increment hits counter, if it reaches to 10, reset hits counter and increment level
                hitsCounter++;
                if(hitsCounter>=HITS_PER_LEVEL) {

                    hitsCounter=0; //reset hits counter
                    level++; // increment level

                    timerCCR = timerCCR * 10 / 14; // new timer value is divided by 1.4 from the previous level
                    TIMER_A0->CCR[0] = timerCCR-1; //update timer to reflect level change

                    //If level max is overcome, you have won the game - so clear LCD and turn game off
                    if(level>END_LEVEL) {
                        lcdClear(GREEN);
                        gameState=GAME_OFF;
                    } // end if: end game
                } // end if: hits Counter reset
            } // end if: hit collision detector
        } // end if:

        //If ball reaches bottom, game over
        if(ballY<=BALL_Y_BOTTOM) {

            //Clear screen and game, game has been lost
            lcdClear(RED);
            gameState=GAME_OFF;

        } // end if: ball reaches bottom

        //Write new ball
        writeBall(ballX, ballY, WRITE);

    } // End if: if game is on
} // end ISR for Timer_A0


/////////////////////////////////////////////////////
//  S1 Button IRS -- Pause/Restart
//  Arguments: None allowed
//  Return value: None
/////////////////////////////////////////////////////
void PORT5_IRQHandler(void) {

    //Debounce delay 10ms
    msDelay(DEBOUNCE);

    //Clear S1 Button IFG (P5.1)
    P5->IFG &= ~S1;

    //If S1 button is pressed, clear the screen
    if(S1_ACTIVE) {

        //If game is paused (GAME_OFF), start it back up - else, pause it
        if(gameState==GAME_OFF) {

            gameState=GAME_ON;

        } else {

            gameState=GAME_OFF;

        } // end if: gameState==GAME_OFF

    } // End if: S1_ACTIVE

} // End S1 IRS


/////////////////////////////////////////////////////
//  S2 Button IRS -- restart game
//  Arguments: None allowed
//  Return value: None
/////////////////////////////////////////////////////
void PORT3_IRQHandler(void) {

    //Debounce delay 10ms
    msDelay(DEBOUNCE);

    //Clear S2 Button IFG (P3.5)
    P3->IFG &= ~S2;

    //If S2 button is pressed, transmit the string "S2 pressed"
    if(S2_ACTIVE) {

        //Reinitialize game values
        initGame();

    } // End if: S2_ACTIVE

} // End S2 IRS


/////////////////////////////////////////////////////
//  initGame - Void - (re)initializes all game parameters
//  Arguments: none
//  Return value: none allowed
/////////////////////////////////////////////////////
void initGame() {

    lcdClear(BG_COLOR);                 // Set LCD to CYAN background
    timerCCR = DROP_INTERVAL-1;         // value of timer that will get divided each level change
    TIMER_A0->CCR[0] = DROP_INTERVAL-1; // Resets the timer/rate ball falls down
    hitsCounter=0;                      // counter that keeps track of how many hits
    level=0;                            // counter that keeps track of level - starts at 0, ends at 4, increments every 10 hits
    ballSpawn();                        // Initialize the ball spawn
    gameState = GAME_ON;                // turn state of game as on

    pedalX = jsLcdConv(adcSample());    // Joystick X converted to LDC value
    writePedal(pedalX, pedalX_prev);    // Write new pedal on screen
    pedalX_prev = pedalX;               // Update the value of pedalX

} // end initGame


/////////////////////////////////////////////////////
//  ballSpawn - Void - respawns the ball at the top of the screen
//  Arguments: none
//  Return value: none allowed
/////////////////////////////////////////////////////
void ballSpawn() {

    ballY = BALL_Y_SPAWN; // Put ball back at top
    ballX = BALL_X_START_MIN + rand() % BALL_X_START_RANGE; //Randomizes ball x starting location
    ballX_Direction = BALL_X_LEFT + BALL_X_DIRECTIONS * (rand() % BALL_X_DIRECTIONS); // Randomizes ballx direction (or the swing)
    ballX_AngleMax = rand() % BALL_X_ANGLE; // Randomizes the angle ballx takes
    ballX_AngleCount = 0; // reset angle counter

} // end ballSpawn
