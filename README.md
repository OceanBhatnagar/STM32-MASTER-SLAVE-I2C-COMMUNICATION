# I2C Communication Between STM32 and Arduino  

This repository contains the source code and documentation for **I2C Communication Between STM32 and Arduino**,
where an STM32F410RB microcontroller acts as the I2C master, transmitting data to an Arduino slave upon a button press. 
The project focuses on low-level peripheral driver development and signal validation using a logic analyzer.  

## Features  
- **Custom GPIO and I2C Drivers**: Designed and developed from scratch, configuring low-level registers for precise control over I/O and serial communication.  
- **I2C Communication**: Implemented and tested communication between STM32 (master) and Arduino (slave), integrating a logic level shifter for voltage compatibility.  
- **Button-Controlled Data Transmission**: Data is sent over I2C when a button is pressed on the STM32 board.  
- **Signal Analysis**: Verified and analyzed I2C signal integrity using a logic analyzer, ensuring correct timing, data transmission, and synchronization.  

## Components Used  
- **STM32 Nucleo Board** (STM32F410RB).  
- **Arduino Board**.  
- **Logic Level Shifter** (3.3V to 5V conversion).  
- **Breadboard** and **Jumper Wires**.  

## Tools Used  
- **STM32CubeIDE**: For writing, compiling, and uploading the code.  
- **Logic Analyzer**: To capture and analyze I2C signals.  

## Project Structure  
- **Source Code**: C code for GPIO, I2C driver, and button-controlled data transmission.  
- **Project Photograph**: Details on implementation and setup.  
