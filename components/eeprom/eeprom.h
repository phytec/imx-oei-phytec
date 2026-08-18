// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#ifndef EEPROM_H
#define EEPROM_H

/*!
 * @addtogroup eeprom
 * @{
 */

/*!
 * @file
 * @brief Header file containing eeprom functions.
 */

/* Includes */

#include <stdbool.h>
#include <stdint.h>
#include "fsl_lpi2c.h"

/* Definitions */


/*! EEPROM configuration structure. */
typedef struct {
    uint8_t addrSize;
} eeprom_data;

/*! EEPROM device info */
typedef struct {
    LPI2C_Type *i2cBase; /*!< I2C base address */
    uint8_t     devAddr; /*!< Device I2C address */
    eeprom_data data;    /*!< EEPROM chip info */
} EEPROM_Type;


/* Functions */

/*!
 * @name eeprom functions
 * @{
 */

/*!
 * Initialize EEPROM
 *
 * @param[in] dev device info
 * @param[in] eeprom EEPROM chip info
 *
 * @return Returns error code.
 */
status_t EEPROM_Init(EEPROM_Type *dev, eeprom_data eeprom);

/*!
 * read data from EEPROM
 *
 * @param[in] dev device info
 * @param[in] off register to start reading from
 * @param[in] count number of registers/bytes to read
 * @param[out] val Data read from the device registers
 *
 * @return Returns error code.
 */
status_t EEPROM_Read(EEPROM_Type const *dev, uint32_t const subAddr, uint8_t const bufSize,
                     uint8_t *buf);

/** @} */

#endif /* EEPROM_H */

/** @} */
