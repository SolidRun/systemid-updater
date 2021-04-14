/*
 * Simple read/write utility to program the SystemID eeprom
 * on Gridless.mini devices
 * EEPROM layout based on this paper: https://www.nxp.com/docs/en/application-note/AN3638.pdf
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include "eeprom.h"
#include "crc.h"

//#define DEBUG

#ifndef DEBUG
#define EEPROM_PATH "/sys/bus/nvmem/devices"
#define EEPROM_NAME "0-00500"
#define EEPROM (EEPROM_PATH "/" EEPROM_NAME "/nvmem")
#else
#define EEPROM "./debug_eeprom"
#endif


void print_usage(char *prg)
{
	fprintf(stderr, "Usage: %s [<eeprom-file-name>] [Options]\n"
					"\n"
					"systemID-updater can be used to setup, update or just read the systemID eeprom.\n"
					"The structure on the EEPROM is based on https://www.nxp.com/docs/en/application-note/AN3638.pdf\n"
					"\n"
					"Options:\n"
					" -c  --check           check the consistence of the eeprom and dump its content\n"
					" -i, --init            start with an empty EEPROM (will overwrite everything)\n"
					" -w, --write           write eeprom to given file\n"
					" -u, --update          update fields in the current EEPROM\n"
					" -s  --sn              Serialnumber: 9 characters\n"
					" -m  --mac             read hw-mac-addr from read only eeprom\n"
					" -a  --addr            read hw-mac-addr from read only eeprom\n",
					prg);
	fprintf(stderr, " -v, --verbose         be verbose\n"
					" -h  --help            this help\n"
					);

}


int main(int argc, char **argv) {
	int opt;
	uint8_t check = 0, verbose = 0, init = 0, update = 0, write = 0;
	int mac = 0, max_mac = 0;
	char *eeprom_path = EEPROM;
	char macs[NXID_MAC_PORTS][19] = {'\0'};
	char sn[10] = {'\0'};
	uint8_t exitcode = EXIT_SUCCESS;

	systemid_t eeprom;
	memset((void *) &eeprom, 0, EEPROM_SIZE);

	struct option long_options[] = {
			{ "check",		no_argument,			0, 'c' },
			{ "init",		no_argument,			0, 'i' },
			{ "update",		no_argument,			0, 'u' },
			{ "write",		no_argument,			0, 'w' },
			{ "sn",			required_argument,		0, 's' },
			{ "mac",		optional_argument,		0, 'm' },
			{ "addr",		required_argument,		0, 'a' },
			{ "verbose",		no_argument,			0, 'v' },
			{ "help",		no_argument,			0, 'h' },
			{ 0,		0,			0, 0},
	};

	int option_index = 0;
	while ((opt = getopt_long(argc, argv, "ciuws:m:a:vh", long_options, &option_index)) != -1) {
		switch (opt) {
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;

			case 'h':
				print_usage(basename(argv[0]));
				exit(EXIT_SUCCESS);

			case 'c':
				check = 1;
				break;

			case 'i':
				init = 1;
				break;

			case 'u':
				update = 1;
				break;

			case 'w':
				write = 1;
				break;

			case 's':
				strncpy(sn, optarg, 10);
				break;

			case 'm':
				if (optarg) {
					mac = atoi(optarg);
					fprintf (stdout, "updating mac %d", mac);
				}
				break;

			case 'a':
				if (mac > 0 && mac <= NXID_MAC_PORTS) {
					strncpy(macs[mac - 1], optarg, 19);
					//bit 0-6 is used
					max_mac |= (1 << (mac-1));
					if (optarg)
						fprintf (stdout, " with address %s", optarg);
					fprintf (stdout, "\n");
				}
				break;

			case 'v':
				verbose++;
				break;

			default:
				fprintf(stderr, "Unknown option %c\n", opt);
				break;
		}
	}

	// store eeprom path
	if (argv[optind] != NULL) {
		eeprom_path = argv[optind];
	}

	// setup structure from given eeprom or from scratch
	if ((init && update) || (!init && !update) && (!check)) {
		fprintf(stderr, "please specify 'update' or 'init' or 'check'\n");
		print_usage(basename(argv[0]));
		exit(EXIT_FAILURE);
	} else {
		if (update || check) {
			exitcode = read_eeprom(&eeprom, eeprom_path);
		} else {
			exitcode = init_eeprom(&eeprom, eeprom_path, mac);
		}
	}

	// dump eeprom content
	if (exitcode==EXIT_SUCCESS && check) {
		if (verbose)
			print_eeprom(&eeprom);
		check_eeprom(&eeprom);
	}

	// set serialnumber if given
	if (sn[0]) {
		strncpy(eeprom.sn, sn, 10);
	}

	// set mac addresses if given
	if (max_mac) {
		for (uint8_t i = 0; i < NXID_MAC_PORTS; i++) {
			if(max_mac & (1<<i)){
				write_mac_address(eeprom.mac[i], macs[i]);
				if (eeprom.macsize < i+1) {
					eeprom.macsize = (uint8_t) (i+1);
				}
			}
		}
	}

	if (exitcode==EXIT_SUCCESS && (update || write)) {
		update_crc(&eeprom);
	}

	check_crc(&eeprom);

	// dump eeprom content
	if (exitcode==EXIT_SUCCESS && check && (update || write)) {
		if (verbose)
			print_eeprom(&eeprom);
		check_eeprom(&eeprom);
	}

	// write eeprom content to file
	if (exitcode == EXIT_SUCCESS && (update || write)) {
		exitcode = write_eeprom(&eeprom, eeprom_path);
	}

	return 0;
}
