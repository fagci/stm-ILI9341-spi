#include <stm32f10x_dma.h>
#include <stm32f10x.h>
#include "stm32f10x.h"
#include "spi_init.h"

#define ARRAYSIZE 800

uint32_t rc_buf[ARRAYSIZE];
uint32_t tx_buf[ARRAYSIZE];

SPI_InitTypeDef SPI_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

void RCC_Configuration(void);

void GPIO_Configuration(void);

void SPI_DMA_Write(uint16_t addr, uint16_t data) {
    tx_buf[addr] = data;
}

void DMA_init_master(void) {

    // RX
    DMA_DeInit(DMA1_Channel2); //Set DMA registers to default values
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &SPI1->DR; //Address of peripheral the DMA must map to
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) rc_buf; //Variable to which received data will be stored
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = ARRAYSIZE; //Buffer size
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel2, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);


    // TX
    DMA_DeInit(DMA1_Channel3); //Set DMA registers to default values
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &SPI1->DR; //Address of peripheral the DMA must map to
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) tx_buf; //Variable from which data will be transmitted
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = ARRAYSIZE; //Buffer size
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel3, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
}

void SPI_init_master(void) {
    RCC_Configuration();
    GPIO_Configuration();
    DMA_init_master();

//    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
//    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
//    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;

    SPI_Init(SPI_MASTER, &SPI_InitStructure);
    SPI_SSOutputCmd(SPI_MASTER, ENABLE);
    SPI_Cmd(SPI_MASTER, ENABLE);

    SPI_I2S_DMACmd(SPI_MASTER, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
}

void RCC_Configuration(void) {
    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_APB2PeriphClockCmd(SPI_MASTER_GPIO_CLK | SPI_MASTER_CLK, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void GPIO_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = SPI_MASTER_PIN_NSS | SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_MASTER_GPIO, &GPIO_InitStructure);
}
