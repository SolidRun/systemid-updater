//
// Created by Felix Hübner on 2020-09-21.
//

#ifndef SYSTEMID_UPDATER_EEPROM_H
#define SYSTEMID_UPDATER_EEPROM_H

#include <stdint.h>

#define NXID_MAC_PORTS 18

typedef struct __attribute__ ((__packed__)) {
	uint8_t YY;
	uint8_t MM;
	uint8_t DD;
	uint8_t hh;
	uint8_t mm;
	uint8_t ss;
} systemid_bcd_date_t;

typedef struct __attribute__ ((__packed__)) {
	unsigned char tagid[4]; // 00-03: literal “NXID”
	unsigned char sn[12]; // 04-0F: serial number
	unsigned char errata[5]; // 10-14: errata label
	systemid_bcd_date_t date;; // 15-1A: build date/time
	unsigned char res_0; // 1B : reserved
	unsigned char version[4]; // 1C-1F: NXID structure version
	unsigned char tempcal[8]; // 20-27: CPU temp. calibration factors
	unsigned char tempcalsys[2]; // 28-29: Board temp. calibration factors
	unsigned char tempcalflags; // 2A : Temp cal enable/qty/etc.
	unsigned char res_1[21]; // 2B-3F: reserved
	unsigned char macsize; // 40 : number of valid MAC addresses
	unsigned char macflags; // 41 : MAC table flags
	unsigned char mac[30][6]; // 42-F5: MAC addresses, array of 6-byte
	unsigned char res_u[6]; // F6-FB: reserved
	unsigned int crc32; // FC-FF: crc-32 checksum
} systemid_t;

#define EEPROM_SIZE sizeof(systemid_t)


uint8_t read_eeprom(systemid_t *e, const char *eeprom_path);
uint8_t write_eeprom(systemid_t *e, const char *eeprom_path);
void print_eeprom(systemid_t *e);
uint8_t init_eeprom(systemid_t *e, const char *eeprom_path, uint8_t read_hw_mac);
void check_eeprom(systemid_t *e);
void write_mac_address(uint8_t *emac, char *mac);

#endif //SYSTEMID_UPDATER_EEPROM_H
