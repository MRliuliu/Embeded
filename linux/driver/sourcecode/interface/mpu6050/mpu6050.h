#ifndef MPU6050_HHHH
#define MPU6050_HHHH

#define MPU6050_MAGIC 'K'

union mpu6050_data
{
	struct {
		unsigned short x;
		unsigned short y;
		unsigned short z;
	}accel;
	struct {
		unsigned short x;
		unsigned short y;
		unsigned short z;
	}gyro;
	unsigned short temp;
};

#define GET_ACCEL _IOR(MPU6050_MAGIC, 0, union mpu6050_data)
#define GET_GYRO  _IOR(MPU6050_MAGIC, 1, union mpu6050_data) 
#define GET_TEMP  _IOR(MPU6050_MAGIC, 2, union mpu6050_data)

#endif
