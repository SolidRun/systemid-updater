//
// Created by Felix Hübner on 2020-09-21.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <zlib.h>
#include "eeprom.h"
#include "crc.h"


void update_crc(systemid_t *e) {
	uint32_t crc = (uint32_t)  crc32(0L, (void *) e, EEPROM_SIZE - 4);
	fprintf(stdout,"update CRC! (%08X -> %08X)\n", ntohl(e->crc32), crc);
	e->crc32 = htonl(crc);
}


uint8_t check_crc(systemid_t *e) {
	uint32_t crc = (uint32_t)  crc32(0L, (void *) e, EEPROM_SIZE - 4);
	if (crc == ntohl(e->crc32)) {
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "CRC invalid: %08X != %08X!\n",ntohl(e->crc32), crc);
		return EXIT_FAILURE;
	}
}
