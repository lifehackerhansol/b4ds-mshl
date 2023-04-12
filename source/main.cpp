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
#include "inifile.h"
#include "nds_loader_arm9.h"
#include "ROMList.h"
#include "saveMap.h"

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

// returns save file path
std::string createSaveFile(std::string rompath) {
	u32 gamecode;
	FILE* f = fopen(rompath.c_str(), "rb");
	fseek(f, 0xC, SEEK_SET);
	fread(&gamecode, 4, 1, f);
	fclose(f);
	std::string savpath = rompath;
	savpath.replace(savpath.size() - 4, 4, ".sav");
	size_t pos = savpath.rfind('/');
	if(pos != std::string::npos) {
		savpath.insert(pos, "/saves");
	} else {
		iprintf("Something went wrong with ROM path\n");
		return "";
	}
	if(!access(savpath.c_str(), F_OK)) {
		iprintf("Save exists, ignoring\n");
		return "";
	}
	u32 savesize = 524288;
	for (int i = 0; i < sizeof(ROMList)/12; i++) {
		ROMListEntry* curentry = &ROMList[i];
		if (gamecode == curentry->GameCode) {
			if (curentry->SaveMemType != 0xFFFFFFFF) savesize = sramlen[curentry->SaveMemType];
			break;
		}
	}
	f = fopen(savpath.c_str(), "wb");
	if(!f) {
		fclose(f);
		iprintf("Failed to open save file for writing\n");
		return "";
	}
	fseek(f, savesize-1, SEEK_SET);
	fputc('\0', f);
	fflush(f);
	fclose(f);
	return savpath;
}

int NB_launchGame(std::string rompath, std::string savpath) {
	CIniFile bootstrapini("fat:/_nds/nds-bootstrap.ini");

	// Write
	bootstrapini.SetString("NDS-BOOTSTRAP", "NDS_PATH", rompath);
	bootstrapini.SetString("NDS-BOOTSTRAP", "SAV_PATH", savpath);
	bootstrapini.SetString("NDS-BOOTSTRAP", "HOMEBREW_ARG", "");
	bootstrapini.SetInt("NDS-BOOTSTRAP", "BOOST_CPU", -1);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "BOOST_VRAM", -1);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "CARD_READ_DMA", -1);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "ASYNC_CARD_READ", -1);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "DSI_MODE", false);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "EXTENDED_MEMORY", false);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "PATCH_MPU_REGION", 0);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "PATCH_MPU_SIZE", 0);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "CONSOLE_MODEL", 0);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "LANGUAGE", 0);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "REGION", 0);
	bootstrapini.SaveIniFile( "sd:/_nds/nds-bootstrap.ini" );

	std::vector<char*> argarray;
	argarray.push_back(strdup("NULL"));
	argarray.push_back(strdup("NULL"));

	argarray.at(0) = "/nds-bootstrap.nds";
	argarray.at(1) = "sd:/nds-bootstrap.nds";
	if(access(argarray[0], F_OK) != 0) return fail("Bootstrap target does not exist.");
	int err = runNdsFile (argarray[0], argarray.size(), (const char **)&argarray[0]);
	if (err != 0) return fail("NDS Launch error " + err);
}

int main(void) {
	if(!fatInitDefault()) {
		return fail("FAT init fail.");
	}

	// limit to 0xD, as nds-bootstrap only supports up to 8KB DLDI for now.
	if(io_dldi_data->driverSize > DLDI_SIZE_8KB) return fail("DLDI driver too large.\nPlease update your kernel.");

	FILE* f = fopen("/moonshl2/extlink.dat","r+b");

	TExtLinkBody extlink;
	if (f) {
		memset(&extlink, 0, sizeof(TExtLinkBody));
		fread(&extlink, 1, sizeof(TExtLinkBody),f);
		if(extlink.ID != ExtLinkBody_ID) {
			fclose(f);
			return fail("Extlink ID mismatch.");
		}
		fseek(f,0,SEEK_SET);
		fwrite("____", 1, 4, f);
		fflush(f);
		fclose(f);
	} else return fail("Extlink does not exist.");

	char target[768];
	// this is the path of the ROM
	ucs2tombs((unsigned char*)target, extlink.NDSFullPathFilenameUnicode, 768);
	std::string savpath = createSaveFile(target);
	return NB_launchGame(target, savpath);
}
