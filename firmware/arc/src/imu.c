#include <zephyr.h>
#include <device.h>
#include <sensor.h>
#include <misc/printk.h>
#include <ipm.h>
#include <ipm/ipm_quark_se.h>

#include "include/ipm_defs.h"

#define STACKSIZE 1024
#define PRIORITY 7

char thread_stacks[1][STACKSIZE];
static struct device *bmi160;

void imu_start(void)
{
	struct sensor_value attr;

	bmi160 = device_get_binding(CONFIG_BMI160_NAME);
	if (!bmi160) {
		printk("Gyro: Device not found.\n");
		return;
	}

	/*
	 * Set accelerometer range to +/- 16G. Since the sensor API needs SI
	 * units, convert the range to m/s^2.
	 */
	sensor_g_to_ms2(16, &attr);

	if (sensor_attr_set(bmi160, SENSOR_CHAN_ACCEL_XYZ,
			    SENSOR_ATTR_FULL_SCALE, &attr) < 0) {
		printk("Cannot set accelerometer range.\n");
		return;
	}

	/*
	 * Set gyro range to +/- 250 degrees/s. Since the sensor API needs SI
	 * units, convert the range to rad/s.
	 */
	sensor_degrees_to_rad(250, &attr);

	if (sensor_attr_set(bmi160, SENSOR_CHAN_GYRO_XYZ,
			    SENSOR_ATTR_FULL_SCALE, &attr) < 0) {
		printk("Cannot set gyro range.\n");
		return;
	}

	printk("IMU's probably online!\n");
}

void imu_thread(void *argle, void *bargle, void *wark)
{
	struct device *ipm;
	int rc;

	ARG_UNUSED(argle);
	ARG_UNUSED(bargle);
	ARG_UNUSED(wark);

	struct sensor_value accel_xyz[3];
	struct sensor_value gyro_xyz[3];

	ipm = device_get_binding("imu_ipm");
	while(1) {

		rc = sensor_channel_get(bmi160, SENSOR_CHAN_GYRO_XYZ, gyro_xyz);
		if (rc) {
			printk("Couldn't read gyro: %d\n", rc);
			continue;
		}
		printk("gyro: x%d y%d z%d\n", gyro_xyz[0].val1, gyro_xyz[1].val1, gyro_xyz[2].val1);

		rc = sensor_channel_get(bmi160, SENSOR_CHAN_ACCEL_XYZ, accel_xyz);
		if (rc) {
			printk("Couldn't read gyro: %d\n", rc);
			continue;
		}
		printk("accel: x%d y%d z%d\n", accel_xyz[0].val1, accel_xyz[1].val1, accel_xyz[2].val1);
		
//		rc = ipm_send(ipm, 1, 0, NULL, 0);
		k_sleep(1000);
	}
}

K_THREAD_DEFINE(thread_imu_id, STACKSIZE, imu_thread,
				NULL, NULL, NULL,
				PRIORITY, 0, K_NO_WAIT);