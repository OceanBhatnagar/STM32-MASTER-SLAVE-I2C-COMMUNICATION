
#include<i2c_driver.h>
#include<stdint.h>

static void I2C_START(I2C_REGDEF_t *pI2Cx);
static void I2C_EXECUTE_BASE_ADDRESS(I2C_REGDEF_t *pI2Cx,uint8_t SlaveAddr);
static void ClearADDRFlag(I2C_REGDEF_t *pI2Cx);
static void I2C_STOP(I2C_REGDEF_t *pI2Cx);

static void I2C_START(I2C_REGDEF_t *pI2Cx){
	pI2Cx->CR1 |=(1<<8);
}
static void I2C_STOP(I2C_REGDEF_t *pI2Cx){
	pI2Cx->CR1 |=(1<<9);
}
static void I2C_EXECUTE_BASE_ADDRESS(I2C_REGDEF_t *pI2Cx,uint8_t SlaveAddr){
	SlaveAddr=SlaveAddr <<1;
	SlaveAddr &=~(1);  //SlaveAddr=SLAVE ADDRESS+READ AND WRITE
	pI2Cx->DR=SlaveAddr;
}

static void ClearADDRFlag(I2C_REGDEF_t *pI2Cx){
	uint32_t dummyRead=pI2Cx->SR1;
     dummyRead=pI2Cx->SR2;
	(void)dummyRead;
}

void I2C_PeripheralControl(I2C_REGDEF_t *pI2Cx,uint8_t EnorDi){
	if(EnorDi==1){
		pI2Cx->CR1 |=(1<<0);
	}
	else{
		pI2Cx->CR1 &=~(1<<0);
	}
}



uint32_t RCC_GetPCLK1Value=16000000;
void I2C_init(I2C_Handle_t *I2CHandle){
	//Enable Acking
	uint32_t temp=0;
	temp |=(I2CHandle->I2CConfig.AckControl <<10);
	I2CHandle->pI2Cx->CR1=temp;

	//FREQ FIELD OF CR2
	temp=0;
	temp=16;  //1000000U
	I2CHandle->pI2Cx->CR1=(temp & 0x3F);

	//SLAVE ADDRESS
	temp=0;
	temp |=(I2CHandle->I2CConfig.DeviceAddress<<1);
	temp |=(1<<14);
	I2CHandle->pI2Cx->OAR1=temp;

	//SERIAL CLOCK SPEED
	uint16_t ccr=0;
	temp=0;
	if(I2CHandle->I2CConfig.SCLSpeed<=I2C_SCL_SPEED_NORMAL){
		//Standard Mode
		ccr=RCC_GetPCLK1Value/(2*I2CHandle->I2CConfig.SCLSpeed);
		temp |=(ccr & 0xFFF);
	}
	else{
		//FAST MODE
		temp |=(1<<15);
		temp |=(I2CHandle->I2CConfig.FM_DutyCycle << 14);
		if(I2CHandle->I2CConfig.FM_DutyCycle==I2C_DUTYCYCLE_2)
		{
			ccr=RCC_GetPCLK1Value/(3*I2CHandle->I2CConfig.SCLSpeed);
		}
		else{
			ccr=RCC_GetPCLK1Value/(25*I2CHandle->I2CConfig.SCLSpeed);
		}
		temp |=(ccr & 0xFFF);
	}
	I2CHandle->pI2Cx->CCR=temp;

	//T_RISE CONFIGURATION

	if(I2CHandle->I2CConfig.SCLSpeed<=I2C_SCL_SPEED_NORMAL){
			//STANDARD MODE
			temp=(RCC_GetPCLK1Value/1000000U)+1;

		}
	else{
		//FAST MODE
		temp=((RCC_GetPCLK1Value*300)/1000000000U)+1;
	}
	I2CHandle->pI2Cx->TRISE=(temp & 0x3F);

}


void I2C_Master_DataSend(I2C_Handle_t *I2CHandle,uint8_t *pTxBuffer,uint32_t Len,uint8_t SlaveAddr){
	//GENERATE START CONDITION
	 I2C_START(I2CHandle->pI2Cx);

	 //CONFIRM START GENERATION
	 while(! I2C_GETFLAG(I2CHandle->pI2Cx, I2C_SB_FLAG));

	 //SEND THE ADDRESS OF SLAVE WITH W=0
	 I2C_EXECUTE_BASE_ADDRESS(I2CHandle->pI2Cx,SlaveAddr);

	 //CONFIRM ADDRESS PHASE
	 while(! I2C_GETFLAG(I2CHandle->pI2Cx, I2C_ADDR_FLAG));

	 //CLEAR ADDR FLAG
	 ClearADDRFlag(I2CHandle->pI2Cx);

	 //SEND DATA UNTIL LEN=0

	 while(Len>0){
		 while(!I2C_GETFLAG(I2CHandle->pI2Cx,I2C_TXE_FLAG));
		 I2CHandle->pI2Cx->DR=*pTxBuffer;
		 pTxBuffer++;
		 Len--;
	 }

	 //Wait For TXE=1 and BTF=1

	 while(!I2C_GETFLAG(I2CHandle->pI2Cx,I2C_TXE_FLAG));
	 while(!I2C_GETFLAG(I2CHandle->pI2Cx,I2C_BTF_FLAG));

	 //STOP CONDITION
	 I2C_STOP(I2CHandle->pI2Cx);

}

uint8_t I2C_GETFLAG(I2C_REGDEF_t *pI2Cx,uint32_t FlagName){
	if(pI2Cx->SR1 & FlagName){
		return FLAG_SET;
	}
	return FLAG_RESET;
}




















//*****************************************************************************************************************

void I2C_IRQCONFIG(uint8_t IRQNO,uint8_t EnableDisable){
if(EnableDisable==1){
		if(IRQNO <=31){
			*NVIC_ISER0 |=(1<<IRQNO);
		}
		else if(IRQNO>31 && IRQNO<64){
			*NVIC_ISER1 |=(1<<(IRQNO%32));
		}
		else if(IRQNO>=64 && IRQNO<96){
			*NVIC_ISER2 |=(1<<(IRQNO%64));
		}
	}
	else{
		if(IRQNO <=31){
					*NVIC_ICER0 |=(1<<IRQNO);
				}
				else if(IRQNO>31 && IRQNO<64){
					*NVIC_ICER1 |=(1<<(IRQNO%32));
				}
				else if(IRQNO>=64 && IRQNO<96){
					*NVIC_ICER2 |=(1<<(IRQNO%64));
				}
			}
	}

void I2C_IRQPriority(uint32_t IRQNumber,uint8_t Priority){
	uint8_t irp=IRQNumber/4;
		uint8_t irp_section=IRQNumber%4;
		uint8_t Shift=(8*irp_section)+4;

		*(NVIC_Priority +(irp*4))|=(Priority<<Shift);
}
