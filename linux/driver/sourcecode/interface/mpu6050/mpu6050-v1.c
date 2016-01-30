#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define SMPLRT_DIV		0x19
#define CONFIG			0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG	0x1C
#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48
#define PWR_MGMT_1		0x6B

static int mpu6050_read_byte(struct i2c_client *client, unsigned char reg)
{
	int ret;

	char txbuf[1] = { reg };
	char rxbuf[1];

	struct i2c_msg msg[2] = {
		{client->addr, 0, 1, txbuf},
		{client->addr, I2C_M_RD, 1, rxbuf}
	};

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
		printk("ret = %d\n", ret);
		return ret;
	}

	return rxbuf[0];
}

static int mpu6050_write_byte(struct i2c_client *client, unsigned char reg, unsigned char val)
{
	char txbuf[2] = {reg, val};

	struct i2c_msg msg[2] = {
		{client->addr, 0, 2, txbuf},
	};

	i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));

	return 0;
}

static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	unsigned short accel_x = 0, accel_y = 0, accel_z = 0;
	unsigned short gyro_x = 0, gyro_y = 0, gyro_z = 0;
	unsigned short temp = 0;

	printk("match OK!\n");
	printk("irqno = %d\n", client->irq);
	printk("addr = %d\n", client->addr);

	mpu6050_write_byte(client, PWR_MGMT_1, 0x00);
	mpu6050_write_byte(client, SMPLRT_DIV, 0x07);
	mpu6050_write_byte(client, CONFIG, 0x06);
	mpu6050_write_byte(client, GYRO_CONFIG, 0x18);
	mpu6050_write_byte(client, ACCEL_CONFIG, 0x01);
	
	while(1) {
		accel_x = mpu6050_read_byte(client, ACCEL_XOUT_L);
		accel_x |= mpu6050_read_byte(client, ACCEL_XOUT_H) << 8;

		accel_y = mpu6050_read_byte(client, ACCEL_YOUT_L);
		accel_y |= mpu6050_read_byte(client, ACCEL_YOUT_H) << 8;

		accel_z = mpu6050_read_byte(client, ACCEL_ZOUT_L);
		accel_z |= mpu6050_read_byte(client, ACCEL_ZOUT_H) << 8;

		printk("acceleration data: x = %04x, y = %04x, z = %04x\n", accel_x, accel_y, accel_z);

		gyro_x = mpu6050_read_byte(client, GYRO_XOUT_L);
		gyro_x |= mpu6050_read_byte(client, GYRO_XOUT_H) << 8;

		gyro_y = mpu6050_read_byte(client, GYRO_YOUT_L);
		gyro_y |= mpu6050_read_byte(client, GYRO_YOUT_H) << 8;

		gyro_z = mpu6050_read_byte(client, GYRO_ZOUT_L);
		gyro_z |= mpu6050_read_byte(client, GYRO_ZOUT_H) << 8;

		printk("gyroscope data: x = %04x, y = %04x, z = %04x\n", gyro_x, gyro_y, gyro_z);

		temp = mpu6050_read_byte(client, TEMP_OUT_L);
		temp |= mpu6050_read_byte(client, TEMP_OUT_H) << 8;

		printk("temperature data: %x\n", temp);

		mdelay(500);
	}

	return 0;
}

static int mpu6050_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id mpu6050_id[] = {
	{ "mpu6050", 0},
	{}
}; 

static struct of_device_id mpu6050_dt_match[] = {
	{.compatible = "invensense,mpu6050" },
	{/*northing to be done*/},
};

struct i2c_driver mpu6050_driver = {
	.driver = {
		.name 			= "mpu6050",
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(mpu6050_dt_match),
	},
	.probe 		= mpu6050_probe,
	.remove 	= mpu6050_remove,
	.id_table 	= mpu6050_id,
};

module_i2c_driver(mpu6050_driver);

