/*
Copyright (c) 2021 lifehackerhansol

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

#include <stdio.h>
#include <string>
#include <vector>

#include <nds.h>
#include <fat.h>

#include "extlink.h"
#include "nds_loader_arm9.h"

int fail(void){
	iprintf("Boot fail.\nPress START to power off.");
	while(1) {
		swiWaitForVBlank();
		scanKeys();
		int pressed = keysDown();
		if(pressed & KEY_START) break;
	}
	return 0;
}

int main(void) {
	consoleDemoInit();
	fatInitDefault();
	iprintf("Reading extlink...\n");
	FILE* f = fopen("/moonshl2/extlink.dat","r+b");
	char* target;
	TExtLinkBody extlink;
	if (f) {
		memset(&extlink, 0, sizeof(TExtLinkBody));
		fread(&extlink, 1, sizeof(TExtLinkBody),f);
		if(extlink.ID != ExtLinkBody_ID) {
			fclose(f);
			return fail();
		}
		ucs2tombs((unsigned char*)target, extlink.DataFullPathFilenameUnicode, 768);
		fseek(f,0,SEEK_SET);
		fwrite("____", 1, 4, f);
		fflush(f);
		fclose(f);
	}
	iprintf("Reading ini...\n");
	iprintf(extlink.NDSFullPathFilenameAlias);
	iprintf("\n");
	std::string inipath = extlink.NDSFullPathFilenameAlias;
	inipath = inipath.erase(inipath.size() - 4) + ".txt";
	f = fopen(inipath.c_str(), "r");
	std::vector<char*> argarray;
	argarray.push_back(strdup("NULL"));
	if(fgets(argarray[0], 768, f) == NULL) return fail();
	argarray.at(1) = extlink.DataFullPathFilenameAlias;
	iprintf("Launching extlink...\n");
	int err = runNdsFile (argarray[0], argarray.size(), (const char **)&argarray[0]);
	if (err != 0) return fail();
}
