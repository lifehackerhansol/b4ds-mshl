/*
	Copyright (c) 2021-2023 lifehackerhansol

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>

#include <nds.h>
#include <nds/arm9/dldi.h>
#include <nds/arm9/dldi_asm.h>
#include <fat.h>

#include "extlink.h"
#include "nds_loader_arm9.h"

size_t ucs2tombs(unsigned char* dst, const unsigned short* src, size_t len) {
	size_t i = 0, j = 0;
	for (; src[i]; i++){
		if(src[i] <= 0x007f){
			if(!dst) j++; else {
				if(len-j<2) break;
				dst[j++] = (src[i] & 0x007f);
			}
		} else if(src[i] <= 0x07ff) {
			if(!dst) j += 2; else {
				if(len-j<3)break;
				dst[j++] = ((src[i] & 0x07c0) >>  6) | 0xc0;
				dst[j++] = ((src[i] & 0x003f)      ) | 0x80;
			}
		}else if((src[i] & 0xdc00) == 0xd800 && (src[i+1] & 0xdc00) == 0xdc00){
			if(!dst)j+=4;else{
				unsigned short z = (src[i]&0x3ff)+0x40;
				if(len-j<5)break;
				dst[j++] = ((z      & 0x0300) >>  8) | 0xf0;   //2
				dst[j++] = ((z      & 0x00fc) >>  2) | 0x80;   //6
				dst[j++] = ((z      & 0x0003) <<  4)           //2
					  | ((src[i+1] & 0x03c0) >>  6) | 0x80; //4
				dst[j++] = ((src[i+1] & 0x003f)      ) | 0x80; //6
			}i++;
		}else{
			if(!dst)j+=3;else{
				if(len-j<4)break;
				dst[j++] = ((src[i] & 0xf000) >> 12) | 0xe0;
				dst[j++] = ((src[i] & 0x0fc0) >>  6) | 0x80;
				dst[j++] = ((src[i] & 0x003f)      ) | 0x80;
			}
		}
	}
	if(dst)dst[j]=0;
	return j;
}

int fail(std::string debug){
	consoleDemoInit();
	iprintf("extlink2argv boot fail:\n");
	iprintf(debug.c_str());
	iprintf("\nPress START to power off.");
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		int pressed = keysDown();
		if(pressed & KEY_START) break;
	}
	return 0;
}

int main(void) {
	if(!fatInitDefault()) {
		return fail("FAT init fail.");
	}

	// limit to 0xD, as nds-bootstrap only supports up to 8KB DLDI for now.
	if(io_dldi_data->driverSize > DLDI_SIZE_8KB) return fail("DLDI driver too large.\nPlease update your kernel.");

	TExtLinkBody extlink;
	FILE* f = fopen("/moonshl2/extlink.dat","r+b");

	if (f) {
		memset(&extlink, 0, sizeof(TExtLinkBody));
		fread(&extlink, 1, sizeof(TExtLinkBody),f);
		/*if(extlink.ID != ExtLinkBody_ID) {
			fclose(f);
			return fail("Extlink ID mismatch.");
		}*/
		fseek(f,0,SEEK_SET);
		fwrite("____", 1, 4, f);
		fflush(f);
		fclose(f);
	} else {
		return fail("Extlink does not exist.");
	}

	char target[768];
	// this is the path of the ROM
	ucs2tombs((unsigned char*)target, extlink.NDSFullPathFilenameUnicode, 768);

	const char *argarray[2] = {
		"fat:/_nds/ntr-forwarder/sdcard.nds",
		target
	};
	int err = runNdsFile(argarray[0], sizeof(argarray) / sizeof(argarray[0]), argarray);

	consoleDemoInit();
	iprintf("Start failed. Error %i\n", err);
	if (err == 1) {
		iprintf("/_nds/ntr-forwarder/sdcard.nds\n");
		iprintf("not found.\n");
		iprintf("\n");
		iprintf("Please get this file from\n");
		iprintf("the SD forwarder pack, in\n");
		iprintf("\"for SD card root/_nds/\n");
		iprintf("ntr-forwarder\"");
	}

	while (1) {
		swiWaitForVBlank();
	}

	return 0;
}
