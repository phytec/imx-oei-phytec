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

/*!
 * Adjust dram timing values
 *
 * @param[in] opt DRAM option representing the DRAM size.
 *
 * @return Returns error code.
 */
status_t phytec_imx95_set_dram_timings(enum phytec_imx95_ddr_eeprom_code opt);

uint8_t __maybe_unused phytec_imx95_detect(struct phytec_eeprom_data *data);

#endif /* _PHYTEC_IMX95_SOM_DETECTION_H */
