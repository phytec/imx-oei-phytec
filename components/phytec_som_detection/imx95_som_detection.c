// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#include <stdio.h>
#include <stdbool.h>
#include "imx95_som_detection.h"

#ifdef PHYTEC_IMX95_SOM_DETECTION

status_t phytec_imx95_set_dram_timings(enum phytec_imx95_ddr_eeprom_code opt) {
    switch (opt) {
        case PHYTEC_IMX95_DDR_1GB:
            __fallthrough;
        case PHYTEC_IMX95_DDR_2GB:
            __fallthrough;
        case PHYTEC_IMX95_DDR_4GB:
            /* 1,2,4 GB Unsupported */
            return kStatus_InvalidArgument;
        case PHYTEC_IMX95_DDR_8GB:
            /* default; do nothing */
            break;
        case PHYTEC_IMX95_DDR_16GB:
            __fallthrough;
        default:
            /* 16 GB and any other option value unsupported */
            return kStatus_Fail;
            break;
    };
    return kStatus_Success;
}

/* Check if the SoM is actually one of the following products:
 * - i.MX95
 *
 * Returns 0 in case it's a known SoM. Otherwise, returns 1.
 */
uint8_t __maybe_unused phytec_imx95_detect(struct phytec_eeprom_data *data)
{
    uint8_t som;

    /* Early API revisions are not supported */
    if (!data || !data->valid) {
        return 1;
    }

    if (data->payload.api_rev < PHYTEC_API_REV2) {
        printf("Unsupported API revision: %u\n", data->payload.api_rev);
        return 1;
    };

    som = data->payload.data_api2.som_no;
#ifdef DEBUG
    printf("som id: %u\n", som);
#endif /* DEBUG */

    if (som == PHYTEC_PHYFLEX_IMX95)
        return 0;

    printf("SoM ID does not match. Wrong EEPROM data?\n");
    return 1;
}

#else

inline uint8_t __maybe_unused phytec_imx95_detect(struct phytec_eeprom_data *data)
{
    return 1;
}

status_t phytec_imx95_set_dram_timings(enum phytec_imx95_ddr_eeprom_code opt) {
    return kStatus_Fail;
}

#endif /* PHYTEC_IMX95_SOM_DETECTION */
