/* Attempts to get a Curie to show up as a mouse using HID.
 *
 * Author: Jack Bradach <jack@bradach.net>
 *
 */

#include "CurieIMU.h"
#include "CurieBLE.h"

int acc_x, acc_y, acc_z;         // accelerometer values

#define IMU_SAMPLE_RATE_HZ 1
#define MOUSE_ACCEL 50

#define BLE_APPEARANCE_HID_MOUSE 962
#define BLE_GATT_SVC_HID "1812"  // HID Service
#define BLE_GATT_CHAR_BMIR "2A33"  // Boot Mouse Input Report
#define BLE_GATT_HID_INFO "2A4A"

#define BLE_BMIR_LEN 3  // Length of the report
#define BLE_PNP_ID_LEN 7  // PNP ID length


/* The Curie is going to be used as a BLE peripheral */
BLEPeripheral peripheral;

/* ...we also need to define the HID service and associated characteristics.
 * See the Bluetooth LE 'HID over GATT Profile Specification,' available
 * wherever the finest search terms are googled.  tl;dr is that there are
 * some required services and characteristics before this thing will properly
 * behave like a HID.
 */
BLEService hid_service(BLE_GATT_SVC_HID);
BLECharCharacteristic protocol_mode("2A4E", BLERead | BLEWriteWithoutResponse);
BLECharacteristic boot_mouse_report(BLE_GATT_CHAR_BMIR, BLERead | BLENotify, BLE_BMIR_LEN);
BLECharacteristic hid_info("2A4A", BLERead, 4 );
BLECharacteristic report_map("2A4B", BLERead, 50);
BLECharCharacteristic controlPoint("2A4C", BLEWriteWithoutResponse);
BLECharacteristic report("2A4D", BLERead | BLENotify, 3); //the report value

/* Battery monitoring (faked for now) */
BLEService battery_service("180F");
BLECharCharacteristic battery_level("2A19", BLERead);

/* Device Information Service w/PnP ID characteristic */
BLEService devinfo_service("180A");
BLECharacteristic pnp_id("2A50", BLERead, BLE_PNP_ID_LEN);

long ticks_last;
unsigned char mouse_status[3] = {0, 0, 0};


const unsigned char battery_level_data = 0x63;
//unsigned char hid_info_data[] = {0x03, 0x00, 0x12, 0x01};
const unsigned char hid_info_data[] = {0,0,0,0};
unsigned char pnp_id_data[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34};

static unsigned char report_map_data[] = {
    0x05, 0x01,           // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,           // USAGE (Mouse)
    0xa1, 0x01,           // COLLECTION (Application)
    0x09, 0x01,           //   USAGE (Pointer)
    0xA1, 0x00,           //   COLLECTION (Physical)
    0x85, 0x00,           //     REPORT_ID
    0x05, 0x09,           //     USAGE_PAGE (Button)
    0x19, 0x01,           //     USAGE_MINIMUM
    0x29, 0x03,           //     USAGE_MAXIMUM
    0x15, 0x00,           //     LOGICAL_MINIMUM (0)
    0x25, 0x01,           //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,           //     REPORT_COUNT (3)
    0x75, 0x01,           //     REPORT_SIZE (1)
    0x81, 0x02,           //     INPUT (Data,Var,Abs)
    0x95, 0x01,           //     REPORT_COUNT (1)
    0x75, 0x05,           //     REPORT_SIZE (5)
    0x81, 0x03,           //     INPUT (Const,Var,Abs)
    0x05, 0x01,           //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,           //     USAGE (X)
    0x09, 0x31,           //     USAGE (Y)
    0x15, 0x81,           //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,           //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,           //     REPORT_SIZE (8)
    0x95, 0x02,           //     REPORT_COUNT (2)
    0x81, 0x06,           //     INPUT (Data,Var,Rel)
    0xC0,                 //   END_COLLECTION
    0xC0                  // END COLLECTION
};


static void imu_init(void)
{
    CurieIMU.begin();
    CurieIMU.setAccelerometerRate(200);
    CurieIMU.setAccelerometerRange(2);
    CurieIMU.begin();
}

/* Reads the X/Y/Z offsets from the accelerometer */
static void imu_print_offsets(void)
{
    Serial.print("x_offset: ");
    Serial.println(CurieIMU.getAccelerometerOffset(X_AXIS));
    Serial.print("y_offset: ");
    Serial.println(CurieIMU.getAccelerometerOffset(Y_AXIS));
    Serial.print("z_offset: ");
    Serial.println(CurieIMU.getAccelerometerOffset(Z_AXIS));
}

/* "Auto-calibrates" the accelerometer, with the assumption that
 * the device is sitting flat on a table and pointing straight up.
 * This is a terrible assumption, and I'm looking for something
 * more sensible; if values are stable, I'd expect one could infer
 * which way is "up" using trig and figure out the compensation
 * values.
 */
// TODO - 2017/04/29 - Come up with a better method for calibration!
static void imu_calibrate(void)
{
    CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
    CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
    CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
}

static void ble_init(void)
{
    /* Set a name for the peripheral */
    peripheral.setLocalName("Crylus");

    peripheral.addAttribute(hid_service);
    peripheral.setAdvertisedServiceUuid(hid_service.uuid());

    boot_mouse_report.setValue(mouse_status, 3);
    peripheral.addAttribute(boot_mouse_report);

    protocol_mode.setValue(1);
    peripheral.addAttribute(protocol_mode);

    /* HID Service Characteristics */
    hid_info.setValue(hid_info_data, 4);
    report_map.setValue(report_map_data, 50);
    report.setValue(mouse_status, 3);
    peripheral.addAttribute(hid_info);
    peripheral.addAttribute(report_map);
    peripheral.addAttribute(controlPoint);
    peripheral.addAttribute(report);

    /* Battery */
    battery_level.setValue(battery_level_data);
    peripheral.addAttribute(battery_service);
    peripheral.addAttribute(battery_level);


    /* Device Information */
    pnp_id.setValue(pnp_id_data, 7);
    peripheral.addAttribute(devinfo_service);
    peripheral.addAttribute(pnp_id);

    // '962' is the BLE appearence category for 'mouse'
    // TODO - jbradach - make this a constant, BLE_APPEARANCE_XXX
    peripheral.setAppearance(BLE_APPEARANCE_HID_MOUSE);

    peripheral.begin();
}

void setup()
{
    /* Initialize serial (actually over USB on a Curie) */
    Serial.begin(115200);
    while (!Serial);

    /* Set up the IMU */
    imu_init();
    imu_calibrate();
    imu_print_offsets();

    ble_init();

    Serial.println("BLE alive; wait for connections...");
    ticks_last = millis();
}

void loop()
{
    long ticks_now = millis();

    if (ticks_now - ticks_last < (1000 / IMU_SAMPLE_RATE_HZ)) {
        return;
    }

    BLECentral central = peripheral.central();

    // if a central is connected then we start to do things
    if (!central) {
        return;
    }

    Serial.print("central: ");
    Serial.println(central.address());

    CurieIMU.readAccelerometer(acc_x, acc_y, acc_z);

    mouse_status[1] = acc_x;
    mouse_status[2] = acc_y;

    Serial.print("acc_x: ");
    Serial.println(acc_x);
    Serial.print("acc_y: ");
    Serial.println(acc_y);

    boot_mouse_report.setValue(mouse_status, 3);
    report.setValue(mouse_status, 3);

    ticks_last = ticks_now;
}
