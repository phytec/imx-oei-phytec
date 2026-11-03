/*
 * Copyright 2026 PHYTEC Messtechnik GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ddr.h"
#include <stdio.h>
#include "lpddr5_timing_8gb.h"

/*
 * Runtime modifiable timing instance.
 */
struct dram_timing_info dram_timing;

/*
 * Scratch window for the non 8GB RAM variants. It holds writable copies of the
 * tables the other variants need to modify and is addressed only through
 * runtime pointers.
 */
#define DDR_TIMING_SCRATCH 0x4aa28000
#define DDR_TIMING_SCRATCH_END 0x4aa2ff00

/* size of the 4-byte aligned objects at the start of the window */
#define DDR_TIMING_ALIGNED_SIZE \
    (sizeof(ddr_dram_fsp_msg) + sizeof(ddr_ddrc_cfg) + \
     sizeof(ddr_dram_fsp0_ddrc_cfg) + sizeof(ddr_dram_fsp_cfg))

/* the aligned objects must keep the following packed arrays word-aligned */
typedef char ddr_timing_aligned_size_ok[
    (DDR_TIMING_ALIGNED_SIZE % 4) == 0 ? 1 : -1];

/*
 * The msgh copy has one spare entry: the 16GB variant inserts a register that
 * is not in the 8GB table.
 */
#define DDR_TIMING_MSGH_CAP (ARRAY_SIZE(ddr_phy_msgh_fsp0_cfg) + 1)
#define DDR_TIMING_MSGH_BYTES \
    (sizeof(struct ddrphy_cfg_param) * DDR_TIMING_MSGH_CAP)

#define DDR_TIMING_SCRATCH_SIZE \
    (DDR_TIMING_ALIGNED_SIZE + sizeof(ddr_ddrphy_cfg) + \
     sizeof(ddr_phy_pie) + sizeof(ddr_phy_fsp0_cfg) + \
     DDR_TIMING_MSGH_BYTES + sizeof(ddr_phy_pie_fsp0_cfg))

/* compile-time check: the copies must fit the window below the QB hole */
typedef char ddr_timing_scratch_fits_window[
    DDR_TIMING_SCRATCH + DDR_TIMING_SCRATCH_SIZE <=
    DDR_TIMING_SCRATCH_END ? 1 : -1];

/*
 * Fixed base pointers for the six tables that need to be modified during
 * runtime inside the scratch window.
 */
#define DDR_TIMING_MSG_RT \
    ((struct dram_fsp_msg *)DDR_TIMING_SCRATCH)
#define DDR_TIMING_DDRC_RT \
    ((struct ddrc_cfg_param *)(DDR_TIMING_SCRATCH + sizeof(ddr_dram_fsp_msg)))
#define DDR_TIMING_FSP0_DDRC_RT \
    ((struct ddrc_cfg_param *)(DDR_TIMING_SCRATCH + sizeof(ddr_dram_fsp_msg) + \
        sizeof(ddr_ddrc_cfg)))
#define DDR_TIMING_FSPCFG_RT \
    ((struct dram_fsp_cfg *)(DDR_TIMING_SCRATCH + sizeof(ddr_dram_fsp_msg) + \
        sizeof(ddr_ddrc_cfg) + sizeof(ddr_dram_fsp0_ddrc_cfg)))
#define DDR_TIMING_CFG_RT \
    ((struct ddrphy_cfg_param *)(DDR_TIMING_SCRATCH + DDR_TIMING_ALIGNED_SIZE))
#define DDR_TIMING_PIE_RT \
    ((struct ddrphy_cfg_param *)(DDR_TIMING_SCRATCH + DDR_TIMING_ALIGNED_SIZE + \
        sizeof(ddr_ddrphy_cfg)))
#define DDR_TIMING_FSP0_RT \
    ((struct ddrphy_cfg_param *)(DDR_TIMING_SCRATCH + DDR_TIMING_ALIGNED_SIZE + \
        sizeof(ddr_ddrphy_cfg) + sizeof(ddr_phy_pie)))
#define DDR_TIMING_MSGH_RT \
    ((struct ddrphy_cfg_param *)(DDR_TIMING_SCRATCH + DDR_TIMING_ALIGNED_SIZE + \
        sizeof(ddr_ddrphy_cfg) + sizeof(ddr_phy_pie) + sizeof(ddr_phy_fsp0_cfg)))
#define DDR_TIMING_PIE_FSP0_RT \
    ((struct ddrphy_cfg_param *)(DDR_TIMING_SCRATCH + DDR_TIMING_ALIGNED_SIZE + \
        sizeof(ddr_ddrphy_cfg) + sizeof(ddr_phy_pie) + sizeof(ddr_phy_fsp0_cfg) + \
        DDR_TIMING_MSGH_BYTES))

/*
 * Copy n bytes without any library call. There is no memcpy() in the OEI
 * environment, and GCC pattern-matches plain copy loops into a memcpy()
 * call (which then fails to link) - volatile accesses prevent that.
 */
static void ddr_timing_copy(void *dst, const void *src, unsigned int n)
{
    volatile unsigned char *d = (volatile unsigned char *)dst;
    const volatile unsigned char *s = (const volatile unsigned char *)src;

    while (n--) {
        *d++ = *s++;
    }
}

/*
 * Materialize the default (8GB) DRAM timings into the runtime-writable
 * dram_timing instance (in .bss). Must be called once at startup, before
 * EEPROM variant detection.
 */
void ddr_timing_init(void)
{
    ddr_timing_copy(&dram_timing, &dram_timing_8gb,
        sizeof(struct dram_timing_info));
}

/*
 * The ddr_phy_pie struct contains some registers more than once. Only the last
 * occurrence of such a register is used by the hardware, so the diff tables
 * hold the value for the last occurrence and the patch functions below write
 * to the last occurrence:
 * 0x41613  0x41617  0x4163B  0x4163F
 * 0x41663  0x41667  0x4168B  0x4168F
 * 0x41723  0x41727  0x4174B  0x4174F
 * 0x417F3  0x417F7  0x4181B  0x4181F
 * 0x41A2B  0x41A2F  0x41A53  0x41A57
 * 0x41A7B  0x41A7F  0x41AA3  0x41AA7
 * 0x41B33  0x41B37  0x41B5B  0x41B5F
 * 0x41BFB  0x41BFF  0x41C23  0x41C27
 */

/*
 * Look up every reg from diff table in timing table (searching from the end,
 * so the last occurrence of a register is patched) and overwrite val.
 */
#define DDR_TIMING_PATCH_FUN(name, type_)                               \
void name(const struct type_ *diff, unsigned int dnum,                  \
    struct type_ *tbl, unsigned int num)                                \
{                                                                       \
    unsigned int i, j;                                                  \
                                                                        \
    for (j = 0; j < dnum; j++) {                                        \
        for (i = num; i > 0; i--) {                                     \
            if (tbl[i - 1].reg == diff[j].reg) {                        \
                tbl[i - 1].val = diff[j].val;                           \
                break;                                                  \
            }                                                           \
        }                                                               \
        if (i == 0) {                                                   \
            printf("WARNING: ddr timing register 0x%x not found "       \
                "(%u entries)\n", diff[j].reg, num);                    \
        }                                                               \
    }                                                                   \
}

DDR_TIMING_PATCH_FUN(ddr_timing_patch_ddrc, ddrc_cfg_param)
DDR_TIMING_PATCH_FUN(ddr_timing_patch, ddrphy_cfg_param)

void create_dram_timing_scratches(void)
{
    volatile uint32_t *w;
    uint32_t n;

    /*
     * Word-normalize the scratch window before any sub-word write: the
     * memory may have ECC and byte writes into never-written words fault
     * (see the comment block above). The window size is a multiple of 4 and
     * both boundaries are 32-bit aligned.
     */
    w = (volatile uint32_t *)DDR_TIMING_SCRATCH;
    n = DDR_TIMING_SCRATCH_SIZE / sizeof(uint32_t);
    while (n--) {
        *w++ = 0U;
    }

    /*
     * Copy the tables the 4GB variant modifies into the scratch window in
     * the NPU SRAM and point dram_timing at the copies. The original
     * arrays live in .data (TCML) and stay read-only at runtime.
     */
    ddr_timing_copy(DDR_TIMING_MSG_RT, ddr_dram_fsp_msg,
        sizeof(ddr_dram_fsp_msg));
    dram_timing.fsp_msg = DDR_TIMING_MSG_RT;

    ddr_timing_copy(DDR_TIMING_DDRC_RT, ddr_ddrc_cfg, sizeof(ddr_ddrc_cfg));
    dram_timing.ddrc_cfg = DDR_TIMING_DDRC_RT;

    ddr_timing_copy(DDR_TIMING_FSPCFG_RT, ddr_dram_fsp_cfg,
        sizeof(ddr_dram_fsp_cfg));
    dram_timing.fsp_cfg = DDR_TIMING_FSPCFG_RT;

    ddr_timing_copy(DDR_TIMING_FSP0_DDRC_RT, ddr_dram_fsp0_ddrc_cfg,
        sizeof(ddr_dram_fsp0_ddrc_cfg));
    dram_timing.fsp_cfg[0].ddrc_cfg = DDR_TIMING_FSP0_DDRC_RT;

    ddr_timing_copy(DDR_TIMING_CFG_RT, ddr_ddrphy_cfg, sizeof(ddr_ddrphy_cfg));
    dram_timing.ddrphy_cfg = DDR_TIMING_CFG_RT;

    ddr_timing_copy(DDR_TIMING_PIE_RT, ddr_phy_pie, sizeof(ddr_phy_pie));
    dram_timing.ddrphy_pie = DDR_TIMING_PIE_RT;

    ddr_timing_copy(DDR_TIMING_FSP0_RT, ddr_phy_fsp0_cfg,
        sizeof(ddr_phy_fsp0_cfg));
    dram_timing.fsp_msg->fsp_phy_cfg = DDR_TIMING_FSP0_RT;

    ddr_timing_copy(DDR_TIMING_MSGH_RT, ddr_phy_msgh_fsp0_cfg,
        sizeof(ddr_phy_msgh_fsp0_cfg));
    dram_timing.fsp_msg->fsp_phy_msgh_cfg = DDR_TIMING_MSGH_RT;

    ddr_timing_copy(DDR_TIMING_PIE_FSP0_RT, ddr_phy_pie_fsp0_cfg,
        sizeof(ddr_phy_pie_fsp0_cfg));
    dram_timing.fsp_msg->fsp_phy_pie_cfg = DDR_TIMING_PIE_FSP0_RT;
}
