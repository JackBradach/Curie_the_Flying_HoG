
#include <ipm.h>
#include <ipm/ipm_quark_se.h>
#include <misc/printk.h>
#include <zephyr.h>


QUARK_SE_IPM_DEFINE(ping_ipm, 0, QUARK_SE_IPM_INBOUND);

void ping_ipm_callback(void *context, u32_t id, volatile void *data)
{
	printk("Ping!\n");
}

void main(void)
{
	struct device *ipm;

	printk("ARC alive!\n");

	ipm = device_get_binding("ping_ipm");
	ipm_register_callback(ipm, ping_ipm_callback, NULL);
	ipm_set_enabled(ipm, 1);
}
