/*
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>

#include "clock.h"
#include "eeprom.h"
#include "oei.h"
#include "board.h"
#include "eeprom.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "fsl_ccm.h"
#include "fsl_clock.h"
#ifdef PHYTEC_SOM_DETECTION
#include "imx95_som_detection.h"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Factory EEPROM I2C device addresses */
#define BOARD_M24C32_DEV_ADDR  0x51U

/* Debug UART base pointer list */
static LPUART_Type *const s_uartBases[] = LPUART_BASE_PTRS;
/* LPI2C base pointer list */
static LPI2C_Type *const s_i2cBases[] = LPI2C_BASE_PTRS;

/* Debug UART clock list */
static uint32_t const s_uartClks[] =
{
    0U,
    CLOCK_ROOT_LPUART1,
    CLOCK_ROOT_LPUART2,
    CLOCK_ROOT_LPUART3,
    CLOCK_ROOT_LPUART4,
    CLOCK_ROOT_LPUART5,
    CLOCK_ROOT_LPUART6,
    CLOCK_ROOT_LPUART7,
    CLOCK_ROOT_LPUART8
};

/* Debug UART configuration info */
static board_uart_config_t const s_uartConfig =
{
    .base = s_uartBases[BOARD_DEBUG_UART_INSTANCE],
    .clockId = s_uartClks[BOARD_DEBUG_UART_INSTANCE],
    .baud = BOARD_DEBUG_UART_BAUDRATE,
    .inst = BOARD_DEBUG_UART_INSTANCE
};

static EEPROM_Type som_eeprom = {};

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t BOARD_EepromRead(uint32_t const off, size_t const count, uint8_t data[static count]) {
    return EEPROM_Read(&som_eeprom, off, count, data);
}

/*--------------------------------------------------------------------------*/
/* Return the debug UART info                                               */
/*--------------------------------------------------------------------------*/
const board_uart_config_t *BOARD_GetDebugUart(void)
{
    return &s_uartConfig;
}

/*--------------------------------------------------------------------------*/
/* Initialize debug console                                                 */
/*--------------------------------------------------------------------------*/
void BOARD_InitDebugConsole(void)
{
    if (s_uartConfig.base != NULL)
    {
#if 0
        uint64_t rate = CCM_RootGetRate(s_uartConfig.clockId);
#else
        uint64_t rate = 24000000;
#endif

        /* Configure debug UART */
        lpuart_config_t lpuart_config;
        LPUART_GetDefaultConfig(&lpuart_config);
        lpuart_config.baudRate_Bps = s_uartConfig.baud;
        lpuart_config.rxFifoWatermark = ((uint8_t)
            FSL_FEATURE_LPUART_FIFO_SIZEn(s_uartConfig.base)) - 1U;
        lpuart_config.txFifoWatermark = ((uint8_t)
            FSL_FEATURE_LPUART_FIFO_SIZEn(s_uartConfig.base)) - 1U;
        lpuart_config.enableTx = true;
        lpuart_config.enableRx = true;
        (void) LPUART_Init(s_uartConfig.base, &lpuart_config,
            (uint32_t) rate & 0xFFFFFFFFU);
    }
}

/*--------------------------------------------------------------------------*/
/* Initialize board                                                         */
/*--------------------------------------------------------------------------*/
void BOARD_InitHardware(void)
{
    Clock_Init();
#if defined(CONSOLE)
    BOARD_InitPins();
    BOARD_InitDebugConsole();
#endif
    BOARD_InitSerialBus();

    som_eeprom.i2cBase = s_i2cBases[BOARD_I2C_INSTANCE];
    som_eeprom.devAddr = BOARD_M24C32_DEV_ADDR;
    status_t err = EEPROM_Init(&som_eeprom, M24C32);
    if (err != kStatus_Success) {
        printf("EEPROM Init failed\n");
    }

#ifdef PHYTEC_SOM_DETECTION
    struct phytec_eeprom_data data = {};
	int ret = phytec_eeprom_data_setup(&data, 0, 0x51);
	if (!ret) {
		ret = phytec_imx95_detect(&data);
		if (!ret)
			phytec_print_som_info(&data);
	}
#endif

}

/*--------------------------------------------------------------------------*/
/* Initialize serial bus for external devices                               */
/*--------------------------------------------------------------------------*/
void BOARD_InitSerialBus(void) {
    LPI2C_Type *base = s_i2cBases[BOARD_I2C_INSTANCE];
    lpi2c_master_config_t lpi2cConfig = { };
    static uint32_t const s_i2cClks[] = {
        0U,
        CLOCK_ROOT_LPI2C1,
        CLOCK_ROOT_LPI2C2
    };
    uint32_t clockId = s_i2cClks[BOARD_I2C_INSTANCE];

    /* bug, reduce width from 64 to 32 */
    uint64_t rate = CCM_RootGetRate(clockId);

    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);

    lpi2cConfig.baudRate_Hz = BOARD_I2C_BAUDRATE;
    lpi2cConfig.enableDoze = false;

    LPI2C_MasterInit(base, &lpi2cConfig, rate);
}
