/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#ifndef _PHYTEC_SOM_DETECTION_H
#define _PHYTEC_SOM_DETECTION_H

#include <stdbool.h>
#include <stdint.h>
#include "common.h"

#define PHYTEC_MAX_OPTIONS	17
#define PHYTEC_EEPROM_INVAL	0xff

#define PHYTEC_API2_DATA_LEN	32

#define PHYTEC_PRODUCT_NAME_PCX_LEN	7	// PCx-000
#define PHYTEC_PRODUCT_NAME_PFL_LEN	8	// PFL-x-00
#define PHYTEC_PRODUCT_NAME_KSP_LEN	8	// KSP-0000
#define PHYTEC_PRODUCT_NAME_MAX_LEN	PHYTEC_PRODUCT_NAME_KSP_LEN
#define PHYTEC_PART_NUMBER_PCX_LEN	11	// PCx-000-\w{1,17}.Ax
#define PHYTEC_PART_NUMBER_PFL_LEN	17	// PFL-x-00-xx000.Ax
#define PHYTEC_PART_NUMBER_KSP_LEN	11	// KSP-0000.Ax
#define PHYTEC_PART_NUMBER_STD_KSP_LEN	16	// PCx-000-KSx00.Ax
#define PHYTEC_PART_NUMBER_MAX_LEN	PHYTEC_PRODUCT_NAME_MAX_LEN + 21

enum {
    PHYTEC_API_REV0 = 0,
    PHYTEC_API_REV1,
    PHYTEC_API_REV2,
    PHYTEC_API_REV3,
};

enum phytec_som_type_str {
    SOM_TYPE_PCM = 0,
    SOM_TYPE_PCL,
    SOM_TYPE_KSM,
    SOM_TYPE_KSP,
    SOM_TYPE_PFL_G,
};

static const char * const phytec_som_type_str[] = {
    "PCM",
    "PCL",
    "KSM",
    "KSP",
    "PFL-G",
};

struct phytec_api2_data {
    uint8_t pcb_rev;		/* PCB revision of SoM */
    uint8_t pcb_sub_opt_rev;	/* PCB subrevision and opt revision */
    uint8_t som_type;		/* SoM type */
    uint8_t som_no;		/* SoM number */
    uint8_t ksp_no;		/* KSP information */
    char opt[PHYTEC_MAX_OPTIONS]; /* SoM options */
    char bom_rev[2];	/* BOM revision */
    uint8_t mac[6];		/* MAC address (optional) */
    uint8_t crc8;		/* checksum */
} __packed;

struct phytec_eeprom_payload {
    uint8_t api_rev;
    struct phytec_api2_data data_api2;
    struct phytec_api3_element *block_head;
} __packed;

struct phytec_eeprom_data {
    struct phytec_eeprom_payload payload;
    bool valid;
};

int phytec_eeprom_data_setup(struct phytec_eeprom_data *data, int bus_num, int addr);
int phytec_eeprom_data_init(struct phytec_eeprom_data *data, int bus_num, int addr);
void __maybe_unused phytec_print_som_info(struct phytec_eeprom_data *data);

char * __maybe_unused phytec_get_opt(struct phytec_eeprom_data *data);
uint8_t __maybe_unused phytec_get_rev(struct phytec_eeprom_data *data);
uint8_t __maybe_unused phytec_get_som_type(struct phytec_eeprom_data *data);

struct phytec_api3_element * __maybe_unused phytec_get_block_head(struct phytec_eeprom_data *data);

#endif /* _PHYTEC_SOM_DETECTION_H */
