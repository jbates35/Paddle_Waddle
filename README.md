# Paddle_Waddle
## Final project of Micro-controllers class, a game created for the MSP432 microcontroller

* Youtube video of use [here](https://www.youtube.com/watch?v=ZzPpJS_bg_o)

Main idea behind the game is that you use a paddle to collect balls flying down the screen. Every so many balls you collect, the game speeds up as to make it more challenging. When you lose the screen goes red. You can reset the game at any time with one of the buttons.

Main programming features of this was:
* Using ADC to collect the voltage from the Joystick and convert it to a binary value the program can understand.
* Using a pre-made file for the LCD that uses SPI to represent the game on the screen.
* Don't be fooled by the I2C module though, it is only there as that was the best way to implement a "random" value (It uses the temperature sensor to generate a "random" setof values at the start of the game)
* Timers and interrupt routines were used for the timing of the ball dropping and waiting on the button in case the user wanted to reset the game.