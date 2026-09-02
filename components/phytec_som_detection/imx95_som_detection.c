// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#include <stdio.h>
#include <stdbool.h>
#include "imx95_som_detection.h"

#ifdef PHYTEC_IMX95_SOM_DETECTION

void phytec_imx95_set_dram_timings(enum phytec_imx95_ddr_eeprom_code opt) {
    switch (opt) {
        case PHYTEC_IMX95_DDR_1GB:
            printf("1GB RAM version not supported yet\n");
            break;
        case PHYTEC_IMX95_DDR_2GB:
            printf("2GB RAM version not supported yet\n");
            break;
        case PHYTEC_IMX95_DDR_4GB:
            printf("4GB RAM version not supported yet\n");
            break;
        case PHYTEC_IMX95_DDR_8GB:
            /* default; do nothing */
            break;
        case PHYTEC_IMX95_DDR_16GB:
            printf("16GB RAM version not supported yet\n");
            break;
        default:
            printf("Invalid DRAM EEPROM Option %u\n", opt);
            printf("Using default RAM Timings (8GB)\n");
            break;
    };
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

uint8_t __maybe_unused phytec_get_imx95_ddr_size(struct phytec_eeprom_data *data)
{
    char *opt;
    uint8_t ddr_id;

    if (!data || !data->valid)
        return PHYTEC_EEPROM_INVAL;

    if (!data->valid || data->payload.api_rev < PHYTEC_API_REV2) {
        printf("Unsupported API revision: %u\n", data->payload.api_rev);
        return PHYTEC_EEPROM_INVAL;
    }

    opt = phytec_get_opt(data);
    if (opt)
        ddr_id = PHYTEC_GET_OPTION(opt[PHYTEC_IMX95_RAM]);
    else
        ddr_id = PHYTEC_EEPROM_INVAL;

#ifdef DEBUG
    printf("ddr id: %u\n", ddr_id);
#endif /* DEBUG */
    return ddr_id;
}

#else

inline uint8_t __maybe_unused phytec_imx95_detect(struct phytec_eeprom_data *data)
{
    return 1;
}

#endif /* PHYTEC_IMX95_SOM_DETECTION */
