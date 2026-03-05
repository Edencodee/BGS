#include "user_i2c.h"
#include <stdio.h>

#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
#define I2C_MASTER_ADDR   0x30

void I2C_UserInit(void)
{
	I2C_InitType i2c1Master;
	GPIO_InitType i2c1Gpio;

	RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

	GPIO_InitStruct(&i2c1Gpio);
	/*PA4 -- SCL; PA5 -- SDA*/
	i2c1Gpio.Pin        = GPIO_PIN_4 | GPIO_PIN_5;
	i2c1Gpio.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
	i2c1Gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
	i2c1Gpio.GPIO_Alternate = GPIO_AF7_I2C1;
	i2c1Gpio.GPIO_Pull = GPIO_No_Pull;
	GPIO_InitPeripheral(GPIOA, &i2c1Gpio);

	I2C_DeInit(I2C1);
	i2c1Master.BusMode     = I2C_BUSMODE_I2C;
	i2c1Master.FmDutyCycle = I2C_FMDUTYCYCLE_2; //if the spped greater than 400KHz, the FmDutyCycle mast be configured to I2C_FMDUTYCYCLE_2
	i2c1Master.OwnAddr1    = I2C_MASTER_ADDR;
	i2c1Master.AckEnable   = I2C_ACKEN;
	i2c1Master.AddrMode    = I2C_ADDR_MODE_7BIT;
	i2c1Master.ClkSpeed    = 400000;

	I2C_Init(I2C1, &i2c1Master);

	I2C_Enable(I2C1, ENABLE);
}

//#pragma arm section code = "RAMCODE"
int I2C_MasterSend(uint8_t dev, uint8_t* p, int size)
{
	/* 1. check busy */
	int timeout = I2CT_LONG_TIMEOUT;
	while (I2C_GetFlag(I2C1, I2C_FLAG_BUSY)) {
		if ((timeout--) == 0) {
			printf("send MASTER_BUSY 1\r\n");
			return -1;
		}
	}

	/* 2. gen start */
	I2C_GenerateStart(I2C1, ENABLE);

	timeout = I2CT_LONG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_MODE_FLAG)) { // EV5
		if ((timeout--) == 0) {
			printf("send MASTER_MODE 2\r\n");
			return -1;
		}
	}

	/* 3. send address */
	I2C_SendAddr7bit(I2C1, dev, I2C_DIRECTION_SEND);
	timeout = I2CT_LONG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_TXMODE_FLAG)) { // EV6
		if ((timeout--) == 0) {
			printf("send MASTER_TXMODE 3\r\n");
			return -1;
		}
	}

	/* 4. send data */
	while (size-- > 0) {
		I2C_SendData(I2C1, *p++);
		timeout = I2CT_LONG_TIMEOUT;
		while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDING)) { // EV8
			if ((timeout--) == 0) {
				printf("send MASTER_SENDING 4\r\n");
				return -1;
			}
		}
	}

	timeout = I2CT_LONG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDED)) { // EV8-2
		if ((timeout--) == 0) {
			printf("send MASTER_SENDED 4.1\r\n");
			return -1;
		}
	}

	/* 5. gen stop */
	I2C_GenerateStop(I2C1, ENABLE);

	/* 6. wait busy */
	timeout = I2CT_LONG_TIMEOUT;
	while (I2C_GetFlag(I2C1, I2C_FLAG_BUSY)) {
		if ((timeout--) == 0) {
			printf("send MASTER_BUSY 6\r\n");
			return -1;
		}
	}

	return 0;
}

int I2C_MasterRecv(uint8_t dev, uint8_t* p, int size)
{
	/* 1. check busy */
	int timeout = I2CT_LONG_TIMEOUT;
	while (I2C_GetFlag(I2C1, I2C_FLAG_BUSY)) {
		if ((timeout--) == 0) {
			printf("recv MASTER_BUSY 1\r\n");
			return -1;
		}
	}
	I2C_ConfigAck(I2C1, ENABLE);

	/* 2. gen start */
	I2C_GenerateStart(I2C1, ENABLE);

	timeout = I2CT_LONG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_MODE_FLAG)) { // EV5
		if ((timeout--) == 0) {
			printf("recv MASTER_MODE 2\r\n");
			return -1;
		}
	}

	/* 3. send addr */
	I2C_SendAddr7bit(I2C1, dev, I2C_DIRECTION_RECV);
	timeout = I2CT_LONG_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_RXMODE_FLAG)) { // EV6
		if ((timeout--) == 0) {
			printf("recv MASTER_RXMODE 3\r\n");
			return -1;
		}
	}
#if GM
	if (size == 1) {
		I2C_ConfigAck(I2C1, DISABLE);
		(void)(I2C1->STS1); // clear ADDR
		(void)(I2C1->STS2);
		/* 4.1. gen stop */
		I2C_GenerateStop(I2C1, ENABLE);

		timeout = I2CT_LONG_TIMEOUT;
		while (!I2C_GetFlag(I2C1, I2C_FLAG_RXDATNE)) {
			if ((timeout--) == 0) {
				printf("recv MASTER_RECVD 5_1\r\n");
				return -1;
			}
		}
		/* 5.1 get data */
		*p = I2C_RecvData(I2C1);
	} else if (size == 2) {
		I2C1->CTRL1 |= 0x0800; /// set ACKPOS
		(void)(I2C1->STS1);
		(void)(I2C1->STS2);
		I2C_ConfigAck(I2C1, DISABLE);

		timeout = I2CT_LONG_TIMEOUT;
		while (!I2C_GetFlag(I2C1, I2C_FLAG_BYTEF)) {
			if ((timeout--) == 0) {
				printf("recv MASTER_BYTEF 4_2\r\n");
				return -1;
			}
		}
		/* 4.2. gen stop */
		I2C_GenerateStop(I2C1, ENABLE);
		
		/* 5.2 get data */
		*p++ = I2C_RecvData(I2C1);
		*p = I2C_RecvData(I2C1);
	} else {
		I2C_ConfigAck(I2C1, ENABLE);
		(void)(I2C1->STS1);
		(void)(I2C1->STS2);
		
		while (size) {
			if(size == 3) {
				timeout = I2CT_LONG_TIMEOUT;
				while (!I2C_GetFlag(I2C1, I2C_FLAG_BYTEF)) {
					if ((timeout--) == 0) {
						printf("recv MASTER_BYTEF 4_3\r\n");
						return -1;
					}
				}
				/* 4.3 get data */
				I2C_ConfigAck(I2C1, DISABLE);
				*p++ = I2C_RecvData(I2C1);

				timeout = I2CT_LONG_TIMEOUT;
				while (!I2C_GetFlag(I2C1, I2C_FLAG_BYTEF)) {
					if ((timeout--) == 0) {
						printf("recv MASTER_BYTEF 5_3\r\n");
						return -1;
					}
				}
				/* 5.3. gen stop */
				I2C_GenerateStop(I2C1, ENABLE);

				/* 6.3 get data */
				*p++ = I2C_RecvData(I2C1);
				*p = I2C_RecvData(I2C1);
				
				break;
			}

			timeout = I2CT_LONG_TIMEOUT;
			while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_RECVD_FLAG)) { // EV7
				if ((timeout--) == 0) {
					printf("recv MASTER_RECVD 4_4\r\n");
					return -1;
				}
			}
			/* 4.4 get data */
			*p++ = I2C_RecvData(I2C1);
			size--;
		}
	}

	/* 7. wait busy */
	timeout = I2CT_LONG_TIMEOUT;
	while (I2C_GetFlag(I2C1, I2C_FLAG_BUSY)) {
		if ((timeout--) == 0) {
			printf("recv MASTER_BUSY 7\r\n");
			return -1;
		}
	}
#endif
	while(size > 0) {
		if(size == 1) {
			I2C_ConfigAck(I2C1, DISABLE);
		} else {
			I2C_ConfigAck(I2C1, ENABLE);
		}
		
		timeout = I2CT_LONG_TIMEOUT;
		while (!I2C_GetFlag(I2C1, I2C_FLAG_BYTEF)) {
			if ((timeout--) == 0) {
				printf("recv MASTER_BYTEF 4\r\n");
				return -1;
			}
		}
		*p++ = I2C_RecvData(I2C1);
		size--;
	}
	I2C_GenerateStop(I2C1, ENABLE);
	
	return 0;
}
//#pragma arm section
