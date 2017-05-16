#include <zephyr.h>
#include <misc/printk.h>
#include <ipm.h>
#include <ipm/ipm_quark_se.h>

#include "include/ipm_defs.h"

QUARK_SE_IPM_DEFINE(ping_ipm, IPM_CH_PING, QUARK_SE_IPM_OUTBOUND);
QUARK_SE_IPM_DEFINE(pong_ipm, IPM_CH_PONG, QUARK_SE_IPM_INBOUND);
QUARK_SE_IPM_DEFINE(imu_ipm, IPM_CH_IMU, QUARK_SE_IPM_INBOUND);
QUARK_SE_IPM_DEFINE(adc_ipm, IPM_CH_ADC, QUARK_SE_IPM_INBOUND);

static void ipm_init(void);
void imu_ipm_callback(void *ctx, uint32_t id, volatile void *data);

void main(void)
{
	struct device *ipm = device_get_binding("ping_ipm");
	printk("x86 alive!\n");

	// IMC setup
	ipm_init();

	while(1) {
		ipm_send(ipm, IPM_CH_PING, 0, NULL, 0);
		k_sleep(1000);
	}
}

void imu_ipm_callback(void *ctx, uint32_t id, volatile void *data)
{
	printk("IMU message received\n");
#if 0
	uint8_t *counter = (uint8_t *)context;
	char *datac = (char *)data;
	const char *expected;
#endif
}

void adc_ipm_callback(void *ctx, uint32_t id, volatile void *data)
{
	printk("ADC message received\n");
}

void pong_ipm_callback(void *ctx, uint32_t id, volatile void *data)
{
	printk("PONG!\n");
}

static void ipm_init(void)
{
	struct device *ipm;

	/* Register the IPM channels. */
	ipm = device_get_binding("pong_ipm");
	ipm_register_callback(ipm, pong_ipm_callback, NULL);
	ipm_set_enabled(ipm, 1);

	ipm = device_get_binding("imu_ipm");
	ipm_register_callback(ipm, imu_ipm_callback, NULL);
	ipm_set_enabled(ipm, 1);

	ipm = device_get_binding("adc_ipm");
	ipm_register_callback(ipm, adc_ipm_callback, NULL);
	ipm_set_enabled(ipm, 1);


}
 