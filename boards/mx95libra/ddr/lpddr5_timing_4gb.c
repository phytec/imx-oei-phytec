/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2026 PHYTEC Messtechnik GmbH
 *
 * 4GB LPDDR5 timing register differences applied on top of the 8GB tables.
 */

#include "ddr.h"
#include "lpddr5_timing.h"
#include "lpddr5_timing_8gb.h"

/*
 * 4GB variant register differences.
 */

/* dram_timing.ddrc_cfg (ddr_ddrc_cfg) */
static const struct ddrc_cfg_param ddr_ddrc_cfg_diff_4gb[] = {
    {0x5e080110, 0x41110001U},
    {0x5e080000, 0xffU},
    {0x5e080010, 0x1U},
    {0x5e080084, 0x0U}
};

/* dram_timing.fsp_cfg[0].ddrc_cfg (ddr_dram_fsp0_ddrc_cfg) */
static const struct ddrc_cfg_param ddr_dram_fsp0_ddrc_cfg_diff_4gb[] = {
    {0x5e080104, 0x0066000CU},
    {0x5e080160, 0x00000001U}
};

/* dram_timing.ddrphy_cfg (ddr_ddrphy_cfg) */
static const struct ddrphy_cfg_param ddr_ddrphy_cfg_diff_4gb[] = {
    {0x100a3, 0x811},
    {0x110a3, 0x81d},
    {0x120a3, 0x811},
    {0x130a3, 0x81d},
    {0x300ae, 0x280},
    {0x300ad, 0x280},
    {0x300ac, 0x280},
    {0x310ae, 0x280},
    {0x310ad, 0x280},
    {0x310ac, 0x280}
};

/* dram_timing.ddrphy_pie (ddr_phy_pie) */
static const struct ddrphy_cfg_param ddr_phy_pie_diff_4gb[] = {
    {0x4101c, 0xcf08},
    {0x9090d, 0x0},
    {0x2007e, 0x11}
};

/* dram_timing.fsp_msg->fsp_phy_cfg (ddr_phy_fsp0_cfg) */
static const struct ddrphy_cfg_param ddr_phy_fsp0_cfg_diff_4gb[] = {
    {0x1005f, 0x0},
    {0x10061, 0x0},
    {0x10063, 0x0},
    {0x10065, 0x0},
    {0x1105f, 0x0},
    {0x11061, 0x0},
    {0x11063, 0x0},
    {0x11065, 0x0},
    {0x1205f, 0x0},
    {0x12061, 0x0},
    {0x12063, 0x0},
    {0x12065, 0x0},
    {0x1305f, 0x0},
    {0x13061, 0x0},
    {0x13063, 0x0},
    {0x13065, 0x0}
};

/* dram_timing.fsp_msg->fsp_phy_msgh_cfg (ddr_phy_msgh_fsp0_cfg) */
static const struct ddrphy_cfg_param ddr_phy_msgh_fsp0_cfg_diff_4gb[] = {
    {0x58000, 0x0},
    {0x58012, 0x110},
    {0x58020, 0x1},
    {0x58043, 0x1e1e},
    {0x58044, 0x1e1e}
};

/* dram_timing.fsp_msg->fsp_phy_pie_cfg (ddr_phy_pie_fsp0_cfg) */
static const struct ddrphy_cfg_param ddr_phy_pie_fsp0_cfg_diff_4gb[] = {
    {0x4101c, 0xcf08},
    {0x90802, 0x1},
    {0x20072, 0x1},
    {0x9080e, 0x1},
    {0x20073, 0x1},
    {0x9080f, 0x1}
};

void set_dram_timings_4gb(void)
{
    create_dram_timing_scratches();
    /* apply 4GB diff tables to the runtime modifiable copies of 8gb timing tables */

    /* DDRC main config (ddr_ddrc_cfg) */
    ddr_timing_patch_ddrc(ddr_ddrc_cfg_diff_4gb, ARRAY_SIZE(ddr_ddrc_cfg_diff_4gb),
        dram_timing.ddrc_cfg, dram_timing.ddrc_cfg_num);

    /* DDRC FSP0 config (ddr_dram_fsp0_ddrc_cfg) */
    ddr_timing_patch_ddrc(ddr_dram_fsp0_ddrc_cfg_diff_4gb,
        ARRAY_SIZE(ddr_dram_fsp0_ddrc_cfg_diff_4gb),
        dram_timing.fsp_cfg[0].ddrc_cfg, dram_timing.fsp_cfg[0].ddrc_cfg_num);

    /* PHY initialize config (ddr_ddrphy_cfg) */
    ddr_timing_patch(ddr_ddrphy_cfg_diff_4gb,
        ARRAY_SIZE(ddr_ddrphy_cfg_diff_4gb),
        dram_timing.ddrphy_cfg, dram_timing.ddrphy_cfg_num);

    /* PHY common PIE (ddr_phy_pie) */
    ddr_timing_patch(ddr_phy_pie_diff_4gb, ARRAY_SIZE(ddr_phy_pie_diff_4gb),
        dram_timing.ddrphy_pie, dram_timing.ddrphy_pie_num);

    /* PHY FSP0 config (ddr_phy_fsp0_cfg) */
    ddr_timing_patch(ddr_phy_fsp0_cfg_diff_4gb,
        ARRAY_SIZE(ddr_phy_fsp0_cfg_diff_4gb),
        dram_timing.fsp_msg->fsp_phy_cfg,
        dram_timing.fsp_msg->fsp_phy_cfg_num);

    /* PHY message block (ddr_phy_msgh_fsp0_cfg) */
    ddr_timing_patch(ddr_phy_msgh_fsp0_cfg_diff_4gb,
        ARRAY_SIZE(ddr_phy_msgh_fsp0_cfg_diff_4gb),
        dram_timing.fsp_msg->fsp_phy_msgh_cfg,
        dram_timing.fsp_msg->fsp_phy_msgh_cfg_num);

    /* PHY FSP0 PIE (ddr_phy_pie_fsp0_cfg) */
    ddr_timing_patch(ddr_phy_pie_fsp0_cfg_diff_4gb,
        ARRAY_SIZE(ddr_phy_pie_fsp0_cfg_diff_4gb),
        dram_timing.fsp_msg->fsp_phy_pie_cfg,
        dram_timing.fsp_msg->fsp_phy_pie_cfg_num);
}
