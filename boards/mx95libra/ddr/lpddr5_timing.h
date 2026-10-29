/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2026 PHYTEC Messtechnik GmbH
 */

#ifndef __LPDDR5_TIMING_H__
#define __LPDDR5_TIMING_H__

#include "ddr.h"

/*
 * Materialize the default (8GB) DRAM timings into the runtime-writable
 * dram_timing instance (in .bss). Must be called once at startup, before
 * EEPROM variant detection.
 */
void ddr_timing_init(void);

/*
 * Copy the tables the 4GB variant modifies into the scratch window and point
 * dram_timing at the copies. Must run before any ddr_timing_patch*() call.
 */
void create_dram_timing_scratches(void);

/*
 * Apply a register diff table to a runtime timing table, overwriting the
 * last occurrence of every register found in it.
 */
void ddr_timing_patch_ddrc(const struct ddrc_cfg_param *diff, unsigned int dnum,
    struct ddrc_cfg_param *tbl, unsigned int num);
void ddr_timing_patch(const struct ddrphy_cfg_param *diff, unsigned int dnum,
    struct ddrphy_cfg_param *tbl, unsigned int num);

#endif /* __LPDDR5_TIMING_H__ */
