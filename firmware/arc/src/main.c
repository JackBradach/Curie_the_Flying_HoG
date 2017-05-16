
#include <ipm.h>
#include <ipm/ipm_quark_se.h>
#include <misc/printk.h>
#include <zephyr.h>

#include "include/ipm_defs.h"
//#include "bmi160.h"

QUARK_SE_IPM_DEFINE(ping_ipm, IPM_CH_PING, QUARK_SE_IPM_INBOUND);
QUARK_SE_IPM_DEFINE(pong_ipm, IPM_CH_PONG, QUARK_SE_IPM_OUTBOUND);
QUARK_SE_IPM_DEFINE(imu_ipm, IPM_CH_IMU, QUARK_SE_IPM_OUTBOUND);
QUARK_SE_IPM_DEFINE(adc_ipm, IPM_CH_ADC, QUARK_SE_IPM_OUTBOUND);

static void ipm_init(void);


void ping_ipm_callback(void *context, u32_t id, volatile void *data)
{
	struct device *ipm;
	int rc;

	ipm = device_get_binding("pong_ipm");
	rc = ipm_send(ipm, 1, 0, NULL, 0);

	ipm = device_get_binding("imu_ipm");
	rc = ipm_send(ipm, 1, 0, NULL, 0);

	ipm = device_get_binding("adc_ipm");
	rc = ipm_send(ipm, 1, 0, NULL, 0);
}


void main(void)
{
	printk("ARC alive!\n");
	ipm_init();
	// Initialize IMU
//	imu_start();
//struct device *ipm = device_get_binding("ping_ipm");
	// Initialize 
}


static void ipm_init(void)
{
	struct device *ipm;

	/* Register the IPM channels. */
	ipm = device_get_binding("ping_ipm");
	ipm_register_callback(ipm, ping_ipm_callback, NULL);
	ipm_set_enabled(ipm, 1);
}
 