#ifndef READ_IMU_DATA_H
#define READ_IMU_DATA_H



//Libraries

#include <Arduino.h>
#include <Wire.h>
#include <math.h>



#include "config.h"



//Function declarations

/*#############################################################################################################*/
/**
 * @brief Read 16-bit data from BNO055 registers
 * @param reg Register address to read from
 * @return Combined 16-bit value
 */
/*#############################################################################################################*/
int16_t read_16_bit_LSB_MSB(uint8_t reg);



/*#############################################################################################################*/
/**
 * @brief Read all raw sensor data from BNO055
 * @param imu_raw_data Pointer to store raw sensor readings
 * @return None
 */
/*#############################################################################################################*/
void read_IMU_raw_data(IMU_Raw_Data *imu_raw_data);



/*#############################################################################################################*/
/**
 * @brief Convert raw sensor data to physical units
 * @param imu_raw_data Input raw sensor data
 * @param imu_real_local_data Pointer to store converted data
 * @return None
 */
/*#############################################################################################################*/
void convert_IMU_raw_data_to_real_local_data(IMU_Raw_Data imu_raw_data, IMU_Real_local_Data *imu_real_local_data);



/*#############################################################################################################*/
/**
 * @brief Calculate inverse of a quaternion
 * @param quar Input quaternion
 * @return Inverted quaternion
 */
/*#############################################################################################################*/
Real_quaternion quaternion_inverse(Real_quaternion quar);



/*#############################################################################################################*/
/**
 * @brief Multiply two quaternions
 * @param quar_1 First quaternion
 * @param quar_2 Second quaternion
 * @return Result quaternion
 */
/*#############################################################################################################*/
Real_quaternion quarternion_multiply(Real_quaternion quar_1, Real_quaternion quar_2);



/*#############################################################################################################*/
/**
 * @brief Rotate a vector using quaternion
 * @param vec Vector to rotate
 * @param quar Rotation quaternion
 * @return Rotated vector
 */
/*#############################################################################################################*/
Real_vector3 rotate_vector_by_quarternion(Real_vector3 vec, Real_quaternion quar);



/*#############################################################################################################*/
/**
 * @brief Convert local IMU data to global coordinates
 * @param imu_real_local_data Input local coordinate data
 * @param imu_real_global_data Pointer to store global coordinate data
 * @return None
 */
/*#############################################################################################################*/
void convert_IMU_local_data_to_global_data(IMU_Real_local_Data imu_real_local_data, IMU_Data *imu_real_global_data);



/*#############################################################################################################*/
/**
 * @brief Main function to read and process IMU data
 * @param imu_data Pointer to store final processed data
 * @return None
 */
/*#############################################################################################################*/
void read_IMU_data(IMU_Data *imu_data);



#endif