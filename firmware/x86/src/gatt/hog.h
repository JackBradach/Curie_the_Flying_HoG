/** @file
 *  @brief HoG Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

void hog_init(void);
void hog_notify(int dx, int dy, uint8_t btn_msk);

#ifdef __cplusplus
}
#endif
