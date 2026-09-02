#ifndef __CDM_DEF_H
#define __CDM_DEF_H
#ifdef __cplusplus 
extern "C" { 
#endif
#include <stdint.h>

#define MS_TYPE_REQ     0x00
#define MS_TYPE_REP     0x01
#define MS_TYPE_ACK     0x02
#define MS_TYPE_ERROR   0xFF
/*
* Command List
*/
#define CMD_DEVICE_ID   0x00 /* 0x00 -- MCU, 0x01-- VIN, 0x02 -- both*/
#define CMD_CONFIG_RD   0x01 /* 0x00 --GSM/GPS,0x01 -- CAN, 0x02 -- WiFi */
#define CMD_CONFIG_WR   0x02 
#define CMD_ODB2_11     0x03
#define CMD_ODB2_29     0x04
#define CMD_GPS         0x05 /* Payload 0x00 - Off, 0x01 -- On, 0x02 -- Get location, 0x03 -- Date Time, 0x04 -- Update System Date Time */
#define CMD_GSM         0x06 /* Payload -- 0x00 -- Off, 0x01 -- On*/
#define CMD_CAN         0x07 /* Other CAN command -- not diagonosis */
#define CMD_CONTROL     0x0F


typedef struct ecu_ms
{
uint32_t header; /* last 2 bytes version */
uint8_t type;  /* 0x00 request, 
                * 0x01--response, 
                * 0x02--ack, 
                * 0xFF--error 
                */
uint8_t command; 
                /* 
                 *  0x00 -- device id read, 
                 *  0x01 -- config read, 
                 *  0x02 -- config write, 
                 *  0x03 -- ODB2-11, 
                 *  0x04 -- ODB2 29, 
                 *  0x05 -- read current config,
                 *  0x0F -- Control Message (reboot),
                 *  0x1F -- GPS on/off/read,
                 *  0xFF -- unknown or error
                 */
uint16_t size;
uint8_t data[64];
//timestamp to be added
uint8_t CRC;
}ecu_mesg_type;

void extract_odb2_cmd(ecu_mesg_type*);

void auto_sar_crc(uint8_t*,uint32_t, uint8_t);
uint32_t exec_unified_cmd(ecu_mesg_type*);
#ifdef __cplusplus 
} 
#endif
#endif