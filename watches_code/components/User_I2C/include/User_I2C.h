/*
 * @Author: baymax 2978043737@qq.com
 * @Date: 2022-10-17 21:02:26
 * @LastEditors: baymax 2978043737@qq.com
 * @LastEditTime: 2022-10-23 22:32:11
 * @FilePath: \esps3_V1\components\User_I2C\include\User_I2C.h
 * @Description:
 *
 * Copyright (c) 2022 by baymax 2978043737@qq.com, All Rights Reserved.
 */
#ifndef User_I2C_H
#define User_I2C_H

#include "stdio.h"
#include "driver/i2c.h"

#define I2C_SLAVE_PORT_NUM 0        /*选择使用I2C0*/
#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C 主 写 */
#define READ_BIT I2C_MASTER_READ    /*!< I2C 主 读 */
#define I2C_MASTER_FREQ_HZ 100000   /*I2C频率*/
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< 主机不需要缓冲区 */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< 主机不需要缓冲区 */
#define ACK_CHECK_EN 0x1            /*!< I2C主端检查从端ack*/
#define ACK_CHECK_DIS 0x0           /*!< I2C主端不检查从端ack */
#define ACK_VAL 0x0                 /*!< 使能ack */
#define NACK_VAL 0x1                /*!< 关闭ack */

esp_err_t I2C_slave_init(void);
esp_err_t I2C_Read_Len(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
esp_err_t I2C_Write_Len(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);
esp_err_t I2C_Read_byte(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t *data);
esp_err_t I2C_Write_byte(i2c_port_t i2c_num, uint8_t addr, uint8_t reg, uint8_t data);
#endif /* User_I2C_H */
