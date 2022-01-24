// Filename: i2c.c
// Description: Module to communicate using the i2c bus
// Author(s): Robert Trost (Edited by: Jimmy Bates (November 17th 2020))
// Set: N/A
// Date: Mar 1, 2017 (created)

#include "msp.h"
#include "i2c.h"

#define SDA             BIT4        // Pin for SDA for I2C
#define SCL             BIT5        // Pin for SCL for I2C
#define CLOCK_DIVISOR   8           // set clock to ~400kHz (should be slightly less than 400kHz)

#define BITS_PER_BYTE   8           // 8 bits per tx/rx byte
#define LSB_MASK        0xFF        // mask for least significant byte

///////////////////////////////////////////////////////////////////////
// i2cInit - Initializes the UCB1 i2c interface
// Arguments: none
// Return Value: none
///////////////////////////////////////////////////////////////////////
void i2cInit(void) {
    // configure pins 6.4 and 6.5 for SDA and SCL for I2C
    P6->SEL0 |= (SDA | SCL);
    P6->SEL1 &= ~(SDA | SCL);

    // ensure in software reset (EUSCI_B_CTLW0_SWRST==1)
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    // set in master I2C synchronous mode with SMCLK source
    EUSCI_B1->CTLW0 |= (EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_MODE_3 | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_UCSSEL_2);

    // set divisor for ~400kHz i2c clock
    EUSCI_B1->BRW |= CLOCK_DIVISOR;

    // remove software reset
    EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

///////////////////////////////////////////////////////////////////////
// i2cWrite - Sends a 16-bit word to a slave on the i2c bus
// Arguments: slaveAddr - slave address of the device to receive the word
//            regAddr - 8-bit register address to which the data is sent
//            data - 16-bit data to be sent to slave device
// Return Value: none
///////////////////////////////////////////////////////////////////////
void i2cWrite(char slaveAddr, char regAddr, int data) {

    // wait until i2c completed any previous operations
    while (EUSCI_B1->STATW & UCBBUSY);

    // set slave address
    EUSCI_B1->I2CSA = slaveAddr;

    // send start condition in transmit mode
    EUSCI_B1->CTLW0 |= UCTXSTT | UCTR;

    // wait for space in TX buf
    while ((EUSCI_B1->IFG & UCTXIFG) == 0) ;

    // send register address
    EUSCI_B1->TXBUF = regAddr;

    // wait for space in TX buf
    while ((EUSCI_B1->IFG & UCTXIFG) == 0) ;

    // send MSB
    EUSCI_B1->TXBUF = data >> BITS_PER_BYTE;

    // wait for space in TX buf
    while ((EUSCI_B1->IFG & UCTXIFG) == 0) ;

    // send LSB
    EUSCI_B1->TXBUF = data & LSB_MASK;

    // wait for space in TX buf
    while ((EUSCI_B1->IFG & UCTXIFG) == 0) ;

    // send stop condition
    EUSCI_B1->CTLW0 |= UCTXSTP;

    // wait for stop to complete
    while (EUSCI_B1->CTLW0 & UCTXSTP);
}

///////////////////////////////////////////////////////////////////////
// i2cRead - Receives a 16-bit word from a slave on the i2c bus
//           Note: in order to read the function first transmits the address
//           of the register to be read from the slave device
// Arguments: slaveAddr - slave address of the device to receive the word
//            regAddr - 8-bit register address from which the data is read
// Return Value:  16-bit data received from slave device
///////////////////////////////////////////////////////////////////////
int i2cRead(char slaveAddr, char regAddr) {
    int readData; // data read from i2C

    // ensure i2c completed any previous operations
    while (EUSCI_B1->STATW & UCBBUSY);

    // set slave address
    EUSCI_B1->I2CSA = slaveAddr;

    // send start condition in transmit mode
    EUSCI_B1->CTLW0 |= UCTR | UCTXSTT;

    // wait for space in TX buf
    while ((EUSCI_B1->IFG & UCTXIFG) == 0) ;

    // send register address
    EUSCI_B1->TXBUF = regAddr;

    // wait for space in TX buf
    while ((EUSCI_B1->IFG & UCTXIFG) == 0) ;

    // send stop condition
    EUSCI_B1->CTLW0 |= UCTXSTP;

    // wait for stop to complete
    while (EUSCI_B1->CTLW0 & UCTXSTP);

    // set in receive mode and send start condition
    EUSCI_B1->CTLW0 = (EUSCI_B1->CTLW0 & ~UCTR) | UCTXSTT;

    // wait for receive data
    while ((EUSCI_B1->IFG & UCRXIFG) == 0) ;

    // get MSB
    readData = EUSCI_B1->RXBUF;
    readData = readData << BITS_PER_BYTE;

    // send stop condition
    EUSCI_B1->CTLW0 |= UCTXSTP;

    // wait for receive data
    while ((EUSCI_B1->IFG & UCRXIFG) == 0) ;

    // get LSB
    readData |= EUSCI_B1->RXBUF;

    // wait for stop to complete
    while (EUSCI_B1->CTLW0 & UCTXSTP);

    // return read data
    return readData;
}
