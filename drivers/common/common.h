/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2025 NXP
 */
#ifndef COMMON_H
#define COMMON_H

#define ALIGN_MASK(x, m)    (((x) + (m)) & ~(m))
#define ALIGN(x, a)         ALIGN_MASK((x), (__typeof__(x))(a) - 1)
#define ALIGN_DOWN(x, a)    ALIGN((x) - ((a) - 1), (a))

/* The following defines are set to keep phytec_som_detection code as close to U-Boot version as
 * feasible
 */
#define	EINVAL  22  /* Invalid argument */

/* gcc_cross.mak sets -Wno-unused-parameter which makes the unused attribute somewhat useless.
 * Nevertheless, add it to have fewer modifications in the phytec_som_detection code from the U-Boot
 * port
 */
#define __maybe_unused  __attribute__((__unused__))
#define __packed        __attribute__((__packed__))

#endif /* COMMON_H */
