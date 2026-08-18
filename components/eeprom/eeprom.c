// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#include "eeprom.h"
#include "fsl_common.h"

status_t EEPROM_Init(EEPROM_Type *dev, eeprom_data eeprom) {
    dev->data = eeprom;

    /* Check that device is ready */
    uint8_t testByte = {};
    status_t err = EEPROM_Read(dev, 0, 1, &testByte);

    if (err != kStatus_Success) {
        return err;
    }

    return kStatus_Success;
}

status_t EEPROM_Read(EEPROM_Type const *dev, uint32_t const subAddr, uint8_t const bufSize,
                     uint8_t *buf) {
    lpi2c_master_transfer_t xfer = {
        .flags = kLPI2C_TransferDefaultFlag,
        .slaveAddress = dev->devAddr,
        .direction = kLPI2C_Read,
        .subaddress = subAddr,
        .subaddressSize = dev->data.addrSize,
        .data = buf,
        .dataSize = bufSize,
    };
    return LPI2C_MasterTransferBlocking(dev->i2cBase, &xfer);
}
