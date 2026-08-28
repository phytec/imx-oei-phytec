// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#include <stddef.h>
#include <stdio.h>
#include "board.h"
#include "crc.h"
#include "phytec_som_detection.h"

#ifdef PHYTEC_SOM_DETECTION

int phytec_eeprom_data_setup(struct phytec_eeprom_data *data, int bus_num, int addr)
{
    int ret;
    if (!data)
        return -EINVAL;

    ret = phytec_eeprom_data_init(data, bus_num, addr);
    if (ret)
        printf("EEPROM data init failed\n");

    return ret;
}

int phytec_eeprom_read(uint8_t *data, int __maybe_unused bus_num, int __maybe_unused addr,
        int size, int offset)
{
    return BOARD_EepromRead(offset, size, data);
}

int phytec_eeprom_data_init_v2(struct phytec_eeprom_data *data)
{
    unsigned int crc;

    if (!data)
        return -1;

    crc = CRC_Crc8((const uint8_t *)&data->payload, PHYTEC_API2_DATA_LEN, CRC8_POLY, 0, false);
#ifdef DEBUG
    printf("crc: %x\n", crc);
#endif /* DEBUG */

    if (crc) {
        printf("CRC mismatch. EEPROM data is not usable.\n");
        return -EINVAL;
    }

    return 0;
}

int phytec_eeprom_data_init(struct phytec_eeprom_data *data,
        int bus_num, int addr)
{
    int ret, i;
    uint8_t *ptr;

    if (!data)
        return -EINVAL;

    ret = phytec_eeprom_read((uint8_t *)data, bus_num, addr,
            PHYTEC_API2_DATA_LEN, 0);
    if (ret)
        goto err;
    data->payload.block_head = NULL;

    if (data->payload.api_rev == 0xff) {
        printf("EEPROM is not flashed. Prototype?\n");
        ret = -EINVAL;
        goto err;
    }

    ptr = (uint8_t *)data;
    for (i = 0; i < PHYTEC_API2_DATA_LEN; ++i)
        if (ptr[i] != 0x0)
            break;

    if (i == PHYTEC_API2_DATA_LEN) {
        printf("EEPROM data is all zero. Erased?\n");
        ret = -EINVAL;
        goto err;
    }

    if (data->payload.api_rev >= PHYTEC_API_REV2) {
        ret = phytec_eeprom_data_init_v2(data);
        if (ret)
            goto err;
    }

    data->valid = true;
    return 0;
err:
    data->valid = false;
    return ret;
}

static int phytec_get_product_name(struct phytec_eeprom_data *data, char *product)
{
    struct phytec_api2_data *api2;
    unsigned int ksp_no, som_type;
    int len;

    if (!data || !data->valid || data->payload.api_rev < PHYTEC_API_REV2)
        return -EINVAL;

    api2 = &data->payload.data_api2;
    som_type = api2->som_type;

    switch (som_type) {
        case SOM_TYPE_KSM:
        case SOM_TYPE_KSP:
            ksp_no = (api2->ksp_no << 8) | api2->som_no;
            len = snprintf(product, PHYTEC_PRODUCT_NAME_MAX_LEN + 1, "%s-%04u",
                    phytec_som_type_str[som_type], ksp_no);
            if (len != PHYTEC_PRODUCT_NAME_KSP_LEN)
                return -EINVAL;
            break;
        case SOM_TYPE_PFL_G_PT:
        case SOM_TYPE_PFL_G_SP:
        case SOM_TYPE_PFL_G_KP:
        case SOM_TYPE_PFL_G_KM:
            len = snprintf(product, PHYTEC_PRODUCT_NAME_MAX_LEN + 1, "%s-%02u",
                           phytec_som_type_str[som_type], api2->som_no);
            if (len != PHYTEC_PRODUCT_NAME_PFL_LEN)
                return -EINVAL;
            break;
        case SOM_TYPE_PCM:
        case SOM_TYPE_PCL:
        case SOM_TYPE_PCM_KSP:
        case SOM_TYPE_PCL_KSP:
        case SOM_TYPE_PCM_KSM:
        case SOM_TYPE_PCL_KSM:
            len = snprintf(product, PHYTEC_PRODUCT_NAME_MAX_LEN + 1, "%s-%03u",
                           phytec_som_type_str[som_type], api2->som_no);
            if (len != PHYTEC_PRODUCT_NAME_PCX_LEN)
                return -EINVAL;
            break;
        default:
            printf("Invalid SoM type %u\n", api2->som_type);
            return -EINVAL;
    };

    return 0;
}

static int phytec_get_part_number(struct phytec_eeprom_data *data, char *part)
{
    char product_name[PHYTEC_PRODUCT_NAME_MAX_LEN + 1] = {'\0'};
    struct phytec_api2_data *api2;
    int res, len;

    if (!data || !data->valid || data->payload.api_rev < PHYTEC_API_REV2)
        return -EINVAL;

    api2 = &data->payload.data_api2;
    unsigned const som_type = api2->som_type;

    res = phytec_get_product_name(data, product_name);
    if (res)
        return res;

    char *variant = {};
    switch (som_type) {
        case SOM_TYPE_PCM:
        case SOM_TYPE_PCL:
            len = snprintf(part, PHYTEC_PART_NUMBER_MAX_LEN + 1, "%s-%s.%s",
                           product_name, api2->opt, api2->bom_rev);
            if (len < PHYTEC_PART_NUMBER_PCX_LEN)
                return -EINVAL;
            return 0;
        case SOM_TYPE_KSM:
        case SOM_TYPE_KSP:
            len = snprintf(part, PHYTEC_PART_NUMBER_MAX_LEN + 1, "%s.%s",
                           product_name, api2->bom_rev);
            if (len != PHYTEC_PART_NUMBER_KSP_LEN)
                return -EINVAL;
            return 0;
        case SOM_TYPE_PFL_G_PT:
            variant = "PT";
            break;
        case SOM_TYPE_PFL_G_SP:
            variant = "SP";
            break;
        case SOM_TYPE_PFL_G_KP:
            variant = "KP";
            break;
        case SOM_TYPE_PFL_G_KM:
            variant = "KM";
            break;
        case SOM_TYPE_PCL_KSM:
        case SOM_TYPE_PCL_KSP:
        case SOM_TYPE_PCM_KSM:
        case SOM_TYPE_PCM_KSP:
            len = snprintf(part, PHYTEC_PART_NUMBER_MAX_LEN + 1, "%s-%s%02u.%s",
                           product_name, phytec_som_type_str[som_type],
                           api2->ksp_no, api2->bom_rev);
            if (len < PHYTEC_PART_NUMBER_STD_KSP_LEN)
                return -EINVAL;
            return 0;
        default:
            printf("Invalid SOM type: %i", som_type);
    };

    /* Handle PFL_G */
    len = snprintf(part, PHYTEC_PART_NUMBER_MAX_LEN + 1,
            "%s-%s%03u.%s", product_name, variant,
            api2->ksp_no, api2->bom_rev);
    if (len != PHYTEC_PART_NUMBER_PFL_LEN)
        return -EINVAL;
    return 0;
}

void __maybe_unused phytec_print_som_info(struct phytec_eeprom_data *data)
{
    char part_number[PHYTEC_PART_NUMBER_MAX_LEN + 1] = {'\0'};
    struct phytec_api2_data *api2;
    char pcb_sub_rev;
    int res;

    if (!data || !data->valid || data->payload.api_rev < PHYTEC_API_REV2)
        return;

    api2 = &data->payload.data_api2;

    /* Calculate PCB subrevision */
    pcb_sub_rev = api2->pcb_sub_opt_rev & 0x0f;
    pcb_sub_rev = pcb_sub_rev ? ((pcb_sub_rev - 1) + 'a') : ' ';

    res = phytec_get_part_number(data, part_number);
    if (res)
        return;

    printf("SOM: %s\n", part_number);
    printf("PCB Rev.: %u%c\n", api2->pcb_rev, pcb_sub_rev);
    if (api2->som_type > 1)
        printf("Options: %s\n", api2->opt);
}

char * __maybe_unused phytec_get_opt(struct phytec_eeprom_data *data)
{
    char *opt;

    if (!data || !data->valid)
        return NULL;

    opt = data->payload.data_api2.opt;

    return opt;
}

uint8_t __maybe_unused phytec_get_rev(struct phytec_eeprom_data *data)
{
    struct phytec_api2_data *api2;

    if (!data || !data->valid || data->payload.api_rev < PHYTEC_API_REV2)
        return PHYTEC_EEPROM_INVAL;

    api2 = &data->payload.data_api2;

    return api2->pcb_rev;
}

uint8_t __maybe_unused phytec_get_som_type(struct phytec_eeprom_data *data)
{
    if (!data || !data->valid || data->payload.api_rev < PHYTEC_API_REV2)
        return PHYTEC_EEPROM_INVAL;

    return data->payload.data_api2.som_type;
}

struct phytec_api3_element * __maybe_unused phytec_get_block_head(struct phytec_eeprom_data *data)
{
    if (!data || !data->valid)
        return NULL;

    return data->payload.block_head;
}

#else

inline int phytec_eeprom_data_setup(struct phytec_eeprom_data *data,
        int bus_num, int addr)
{
    return PHYTEC_EEPROM_INVAL;
}

inline int phytec_eeprom_data_init(struct phytec_eeprom_data *data,
        int bus_num, int addr)
{
    return PHYTEC_EEPROM_INVAL;
}

inline void __maybe_unused phytec_print_som_info(struct phytec_eeprom_data *data)
{
}

inline char *__maybe_unused phytec_get_opt(struct phytec_eeprom_data *data)
{
    return NULL;
}

uint8_t __maybe_unused phytec_get_rev(struct phytec_eeprom_data *data)
{
    return PHYTEC_EEPROM_INVAL;
}

uint8_t __maybe_unused phytec_get_som_type(struct phytec_eeprom_data *data)
{
    return PHYTEC_EEPROM_INVAL;
}

    inline struct phytec_api3_element * __maybe_unused
phytec_get_block_head(struct phytec_eeprom_data *data)
{
    return NULL;
}

#endif /* PHYTEC_SOM_DETECTION */
