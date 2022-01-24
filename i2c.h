// Filename: i2c.h
// Description: Module to communicate using the i2c bus
// Author(s): Robert Trost (Edited by: Jimmy Bates (November 17th 2020))
// Set: N/A
// Date: Mar 1, 2017 (created)

#ifndef I2C_H_
#define I2C_H_

// slave addresses for sensors connected to the i2c bus
#define OPT3001_SLAVE_ADDRESS   0x44
#define TMP006_SLAVE_ADDRESS    0x40

// TMP006 Register Addresses
#define TMP006_TEMPERATURE_REG  0x01
#define TMP006_DEVICE_ID_REG    0xFF    // expected DEVICE ID will return 0x0067

// OPT3001 Register Addresses
#define OPT3001_RESULT_REG      0x00
#define OPT3001_CONFIG_REG      0x01
#define OPT3001_DEVICE_ID_REG   0x7F    // Expected DEVICE ID will return 0x3001

#define OPT3001_CONFIG_100      0xC410  // OPT3001 Configuration value for 100ms continuous sampling

///////////////////////////////////////////////////////////////////////
// i2cInit - Initializes the UCB1 i2c interface
// Arguments: none
// Return Value: none
///////////////////////////////////////////////////////////////////////
void i2cInit(void);

///////////////////////////////////////////////////////////////////////
// i2cWrite - Sends a 16-bit word to a slave on the i2c bus
// Arguments: slaveAddr - slave address of the device to receive the word
//            regAddr - 8-bit register address to which the data is sent
//            data - 16-bit data to be sent to slave device
// Return Value: none
///////////////////////////////////////////////////////////////////////
void i2cWrite(char slaveAddr, char regAddr, int data);

///////////////////////////////////////////////////////////////////////
// i2cRead - Receives a 16-bit word from a slave on the i2c bus
// Arguments: slaveAddr - slave address of the device to receive the word
//            regAddr - 8-bit register address from which the data is read
// Return Value:  16-bit data received from slave device
///////////////////////////////////////////////////////////////////////
int i2cRead(char slaveAddr, char regAddr);

#endif /* I2C_H_ */
