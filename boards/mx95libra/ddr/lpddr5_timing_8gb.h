/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2026 PHYTEC Messtechnik GmbH
 *
 * Default 8GB LPDDR5 timing tables.
 */

#ifndef __LPDDR5_TIMING_8GB_H__
#define __LPDDR5_TIMING_8GB_H__

#include "ddr.h"

/*
 * Entry counts of the default 8GB tables. Other translation units need the
 * sizes for sizeof()/ARRAY_SIZE(). Keep these equal to the number of
 * initializers in lpddr5_timing_8gb.c: a count smaller than the initializer
 * list is a compile error, but a larger count is accepted silently (the extra
 * entries are zero-filled).
 */
#define DDR_8GB_DDRC_CFG_NUM             37
#define DDR_8GB_DDRC_FSP0_CFG_NUM        18
#define DDR_8GB_DDRPHY_CFG_NUM           87
#define DDR_8GB_DDRPHY_TRAINED_CSR_NUM   5699
#define DDR_8GB_PHY_FSP0_CFG_NUM         427
#define DDR_8GB_PHY_MSGH_FSP0_CFG_NUM    43
#define DDR_8GB_PHY_PIE_FSP0_CFG_NUM     219
#define DDR_8GB_PHY_PIE_NUM              4042
#define DDR_8GB_FSP_MSG_NUM              1
#define DDR_8GB_FSP_CFG_NUM              1

extern struct ddrc_cfg_param ddr_ddrc_cfg[DDR_8GB_DDRC_CFG_NUM];
extern struct ddrc_cfg_param ddr_dram_fsp0_ddrc_cfg[DDR_8GB_DDRC_FSP0_CFG_NUM];
extern struct ddrphy_cfg_param ddr_ddrphy_cfg[DDR_8GB_DDRPHY_CFG_NUM];
extern struct ddrphy_cfg_param ddr_ddrphy_trained_csr[DDR_8GB_DDRPHY_TRAINED_CSR_NUM];
extern struct ddrphy_cfg_param ddr_phy_fsp0_cfg[DDR_8GB_PHY_FSP0_CFG_NUM];
extern struct ddrphy_cfg_param ddr_phy_msgh_fsp0_cfg[DDR_8GB_PHY_MSGH_FSP0_CFG_NUM];
extern struct ddrphy_cfg_param ddr_phy_pie_fsp0_cfg[DDR_8GB_PHY_PIE_FSP0_CFG_NUM];
extern struct ddrphy_cfg_param ddr_phy_pie[DDR_8GB_PHY_PIE_NUM];
extern struct dram_fsp_msg ddr_dram_fsp_msg[DDR_8GB_FSP_MSG_NUM];
extern struct dram_fsp_cfg ddr_dram_fsp_cfg[DDR_8GB_FSP_CFG_NUM];
extern const struct dram_timing_info dram_timing;

#endif /* __LPDDR5_TIMING_8GB_H__ */
