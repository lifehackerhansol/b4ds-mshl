/*
Copyright (c) 2021-present lifehackerhansol

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>

#include <nds.h>
#include <nds/arm9/dldi.h>
#include <fat.h>

#include "extlink.h"
#include "nds_loader_arm9.h"

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
	if(io_dldi_data->driverSize > 0xE) return fail("DLDI driver too large.\nPlease update your kernel.");
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
	ucs2tombs((unsigned char*)target, extlink.NDSFullPathFilenameUnicode, 768);
	std::string txtpath;
	for(u32 i = 0; i < strlen(target); i++){
		txtpath.push_back(target[i]);
	}
	txtpath = txtpath.erase(txtpath.size() - 4) + ".txt";
	f = fopen(txtpath.c_str(), "r");
	if (!f) return fail("Path text file does not exist.");
	std::vector<char*> argarray;
	argarray.push_back(strdup("NULL"));
	argarray.push_back(strdup("NULL"));
	ucs2tombs((unsigned char*)target, extlink.DataFullPathFilenameUnicode, 768);
	fgets(argarray[0], 768, f);
	argarray.at(1) = target;
	if(access(argarray[0], F_OK) != 0) return fail("Bootstrap target does not exist.");
	int err = runNdsFile (argarray[0], argarray.size(), (const char **)&argarray[0]);
	if (err != 0) return fail("NDS Launch error " + err);
}
