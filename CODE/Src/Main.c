#include<stdint.h>
#include<stm.h>
#include<string.h>

void delay(){
	for(uint32_t i =0;i<500000/2;i++);
}

uint8_t data[]="Hello\n";
#define SlaveAddr 0x68

//SDA D14 PB9
//SCL D10 PB6
I2C_Handle_t I2C1Handle;
void I2C1_GPIO_INIT(void){

	GPIO_HANDLE_t I2CPins;

	I2CPins.pGPIOx=GPIOB;
	I2CPins.GPIO_CONFIG_t.PinMode=GPIOMODE_AltFunction;
	I2CPins.GPIO_CONFIG_t.PinOPType=OUTPUT_OPENDRAIN;
	I2CPins.GPIO_CONFIG_t.PinPuPdControl=PULLUP;
	I2CPins.GPIO_CONFIG_t.PinAltFunMode=4;
	I2CPins.GPIO_CONFIG_t.PinSpeed=SPEED_HIGH ;

	//SDA PB9
	I2CPins.GPIO_CONFIG_t.PinNumber=9;
	GPIO_init(&I2CPins);

	//SCL PB6
	I2CPins.GPIO_CONFIG_t.PinNumber=6;
	GPIO_init(&I2CPins);

}

void I2C1_INIT(void){

	I2C1Handle.pI2Cx=I2C1_R;
	I2C1Handle.I2CConfig.AckControl=I2C_ACK_ENABLE;
	I2C1Handle.I2CConfig.DeviceAddress=0x61;
	I2C1Handle.I2CConfig.FM_DutyCycle=I2C_DUTYCYCLE_2;
	I2C1Handle.I2CConfig.SCLSpeed=I2C_SCL_SPEED_NORMAL;

	I2C_init(&I2C1Handle);
}


int main(void){
	//RCC GPIO
		uint32_t *RCC_AHB1 =(uint32_t*)0x40023830;
		*RCC_AHB1 &=~(1<<1);
		*RCC_AHB1 &=~(1<<0);
		*RCC_AHB1 &=~(1<<2);

		*RCC_AHB1 |=(1<<1);
		*RCC_AHB1 |=(1<<0);
		*RCC_AHB1 |=(1<<2);

		//RCC I2C
			uint32_t *RCC_APB1=(uint32_t*)0x40023840;
			*RCC_APB1 &=~(1<<21);
			*RCC_APB1 |=(1<<21);

		//Button
			GPIO_HANDLE_t GpioButton;
			 GpioButton.pGPIOx=GPIOC;
			 GpioButton.GPIO_CONFIG_t.PinNumber=13;
			 GpioButton.GPIO_CONFIG_t.PinMode=GPIOMODE_INPUT;
            GpioButton.GPIO_CONFIG_t.PinSpeed=SPEED_HIGH;
            GpioButton.GPIO_CONFIG_t.PinPuPdControl=NOPULL;
			GPIO_init(&GpioButton);

	I2C1_GPIO_INIT();
	I2C1_INIT();




	while(1){
    //Button Press
		 while(GPIO_INPUT_PINREAD(GPIOC,13)==1);
	//I2C DATA TRANSFER
	I2C_Master_DataSend(&I2C1Handle,data,strlen((char*)data),SlaveAddr);
	}



}

