/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#ifndef _PHYTEC_IMX95_SOM_DETECTION_H
#define _PHYTEC_IMX95_SOM_DETECTION_H

#include "fsl_common.h"
#include "phytec_som_detection.h"

#define PHYTEC_PHYFLEX_IMX95	2

enum phytec_imx95_ddr_eeprom_code {
    PHYTEC_IMX95_DDR_1GB = 0,
    PHYTEC_IMX95_DDR_2GB,
    PHYTEC_IMX95_DDR_4GB,
    PHYTEC_IMX95_DDR_8GB,
    PHYTEC_IMX95_DDR_16GB,
};

enum phytec_imx95_kit_options {
    PHYTEC_IMX95_RAM = 5,
};

/*!
 * Adjust dram timing values
 *
 * @param[in] opt DRAM option representing the DRAM size.
 */
void phytec_imx95_set_dram_timings(enum phytec_imx95_ddr_eeprom_code opt);

/*
 * Parse RAM size from EEPROM data.
 *
 * @param[in] data valid EEPROM data read from an EEPROM
 *
 * @return Returns 0xff on failure, otherwise RAM option.
 */
uint8_t __maybe_unused phytec_get_imx95_ddr_size(struct phytec_eeprom_data *data);

uint8_t __maybe_unused phytec_imx95_detect(struct phytec_eeprom_data *data);

#endif /* _PHYTEC_IMX95_SOM_DETECTION_H */
