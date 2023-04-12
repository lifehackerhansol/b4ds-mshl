/*
	Copyright (c) 2010 Moonlight

	SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <nds/ndstypes.h>

#define ExtLinkBody_MaxLength (256)
#define ExtLinkBody_ID (0x30545845) // EXT0

typedef u16 UnicodeChar;

typedef struct {
	u32 ID, dummy1, dummy2, dummy3; // dummy is ZERO.
	char DataFullPathFilenameAlias[ExtLinkBody_MaxLength];
	char DataPathAlias[ExtLinkBody_MaxLength];
	char DataFilenameAlias[ExtLinkBody_MaxLength];
	char NDSFullPathFilenameAlias[ExtLinkBody_MaxLength];
	char NDSPathAlias[ExtLinkBody_MaxLength];
	char NDSFilenameAlias[ExtLinkBody_MaxLength];
	UnicodeChar DataFullPathFilenameUnicode[ExtLinkBody_MaxLength];
	UnicodeChar DataPathUnicode[ExtLinkBody_MaxLength];
	UnicodeChar DataFilenameUnicode[ExtLinkBody_MaxLength];
	UnicodeChar NDSFullPathFilenameUnicode[ExtLinkBody_MaxLength];
	UnicodeChar NDSPathUnicode[ExtLinkBody_MaxLength];
	UnicodeChar NDSFilenameUnicode[ExtLinkBody_MaxLength];
} TExtLinkBody;

#ifdef __cplusplus
}
#endif
