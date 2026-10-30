/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2026 PHYTEC Messtechnik GmbH
 */

#ifndef __LPDDR5_TIMING_H__
#define __LPDDR5_TIMING_H__

/*
 * Materialize the default (8GB) DRAM timings into the runtime-writable
 * dram_timing instance (in .bss). Must be called once at startup, before
 * EEPROM variant detection.
 */
void ddr_timing_init(void);

void set_dram_timings_4gb(void);

#endif /* __LPDDR5_TIMING_H__ */
