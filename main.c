/*
 * Copyright (C) 2013 3DSGuy
 * Copyright (C) 2015 173210
 *
 * This file is part of make_cdn_cia.
 *
 * make_cdn_cia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * make_cdn_cia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with make_cdn_cia.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cia.h"
#include "ctr_endian.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define VER "1.00"

int main(int argc, char *argv[])
{
	TIKCtx tik;
	TMDCtx tmd;
	FILE *out;
	
	atexit(freeBuffer);
	
	if(argc != 5) {
		printf("CTR_Toolkit - CIA Generator for CDN Content\n"
			"Version " VER " (C) 2013-2015 3DSGuy, fork by ZeroSkill1, 173210\n\n"
			"Usage: %s <CDN Content Dir> <tmd file> <ticket file> <output CIA file>\n",
			argv[0]);
		return EINVAL;
	}

	out = fopen(argv[4],"wb");
	if (out == NULL) {
		perror("CIA: error");
		return errno;
	}

	tik.fp = fopen(argv[3],"rb");
	if (tik.fp == NULL) {
		fclose(out);
		perror("CETK: error");
		return errno;
	}
	
	if (processTIK(&tik)) {
		fclose(out);
		fclose(tik.fp);
		return errno;
	}

	tmd.fp = fopen(argv[2],"rb");
	
	if (tmd.fp == NULL) {
		perror("TMD: error");
		fclose(out);
		fclose(tik.fp);
		return errno;
	}
	
	if (processTMD(&tmd)) {
		fclose(out);
		fclose(tik.fp);
		fclose(tmd.fp);
		return errno;
	}

	if (chdir(argv[1])) {
		perror("CIA: error");
		free(tmd.content);
		fclose(out);
		fclose(tik.fp);
		fclose(tmd.fp);
		return errno;
	}
	
	if (tik.titleID != tmd.titleID) {
		printf("warning: CETK and TMD Title IDs do not match\n"
			"       CETK Title ID: 0x%016luX\n"
			"       TMD Title ID:  0x%016lX\n",
			be64toh(tik.titleID), be64toh(tmd.titleID));
	}
	
	if (writeCIA(&tmd, &tik, out)) {
		fclose(out);
		fclose(tik.fp);
		fclose(tmd.fp);
		free(tmd.content);
		return errno;
	}
	
	free(tmd.content);
	fclose(tmd.fp);
	fclose(tik.fp);
	
	return 0;
}
