/*
 * f6-split-tool.c -- Version 0.0.1
 * Extracts RAW exercise data from the RAW sonic link data captured using 
 * rs200_decode for the Polar F6 HRM.
 *
 * Roland Hostettler <r.hostettler@gmx.ch>
 * 2008-01-31
 * 
 * License under the GNU GPL v2
 * 
 * Note: This is only a temporary solution until a more sophisticated tool is 
 * available.
 */

/* includes */
#include <stdio.h>
#include <error.h>

/* definitions */
#define	EXERCISES_COUNT		12
#define HEADER_LENGTH		8
#define SECTION_LENGTH		255
#define	SECTION_HEADER_LENGTH	3
#define	EXERCISE_LENGTH		43

/* 
 * forward in the file
 */
int ffwd(FILE *fd, int bytes) {
	int i = 0;
	
	while(!feof(fd) && i++ < bytes) {
		fgetc(fd);
	}
	
	return bytes;
}

/*
 * splits the file into sub-files containing either diary, totals or an exercise
 * 
 * TODO: export totals
 * TODO: export diary
 * TODO: Header check, etc.
 */
int split_file(char *infile) {
	FILE *fin;
	FILE *fout = NULL;
	char outfile[20];
	unsigned char c;
	int i = 0;
	int j = 0;
	int ex_bytes_read = 0;
	int ex_read = 0;
	int section_nr = 0;
	int section_data_len = 0;
	unsigned char exetime_sec, exetime_min, exetime_h;
	unsigned char calsum_b1, calsum_b2, calsum_b3;
	
	
	fin = fopen(infile, "rb");
	if(fin == NULL) {
		error(1, 0, "File \"%s\" could not be opened, stop.\n", infile);
	}
	
#if 0
	/* read the section start delimiter */
	if(fgetc(fin) != 0x55) {
		error(1, 0, "Section start not found, stop.\n");
	}

	/* skip the first 8+255 = 263 bytes (header, totals)*/
	ffwd(fin, HEADER_LENGTH+SECTION_LENGTH);
#endif

	/* read the totals */
	/* 
	 * note: the totals are added to each exercise in the end and this is
	 * introduced by the split-tool. this is only as an aid so that PolarViewer
	 * (part of SportsTracker) can display them
	 */

	/* store the total execution time */
	ffwd(fin, HEADER_LENGTH);
	ffwd(fin, 6);
	exetime_sec = fgetc(fin);
	exetime_min = fgetc(fin);
	exetime_h = fgetc(fin);
	
	/* store the total calories sum */
	ffwd(fin, 3);
	calsum_b1 = fgetc(fin);
	calsum_b2 = fgetc(fin);
	calsum_b3 = fgetc(fin);
	
	/* skip the diary-part */
	ffwd(fin, SECTION_LENGTH-6-3-3-3);
	
	/* read the 3 remaining sections */
	for(j = 0; j < 3; j++) {
		/* check for the section header */
		if((c = getc(fin)) != 0x55) {
			error(1, 0, "Section start not found, got 0x%2X, expected 0x55 stop\n", c);
		}
	
		/* read the section number and the data length */
		/* NOTE: a check could be added here! */
		section_nr = fgetc(fin);
		section_data_len = fgetc(fin);
	
		/* read the whole bunch of section data */
		for(i = 0; i < section_data_len; i++) {
			c = fgetc(fin);
			
			/* check wheter we are expecting the beginning of an 
			   exercise record and the marker was found */
			if(c == 0x80 && ex_bytes_read == 0) {
				/* open the output file */
				sprintf(outfile, "exercise_%d.frd", ex_read);
				fout = fopen(outfile, "wb");
			
				if(fout == NULL) {
					error(1, 0, "Error opening output file, stop.\n");
				}
			}

			/* if there is an output file open, write the read byte
			   to it */
			if(fout != NULL) {		
				fputc(c, fout);
				ex_bytes_read++;
			}
		
			/* close the output file and reset the counter if the
			   whole exercise record has been read */
			if(ex_bytes_read == EXERCISE_LENGTH) {
				/* append the total exercise time and calories 
				   consumption to the exercies */
				fputc(exetime_sec, fout);
				fputc(exetime_min, fout);
				fputc(exetime_h, fout);
				fputc(calsum_b1, fout);
				fputc(calsum_b2, fout);
				fputc(calsum_b3, fout);
				fclose(fout);
				ex_bytes_read = 0;
				ex_read++;
			}
		}
		
		/* forward the remaining padding bytes */
		ffwd(fin, SECTION_LENGTH-SECTION_HEADER_LENGTH-section_data_len);
	}

	fclose(fin);
	return 0;
}

/*
 * a little hint
 */
void usage(void) {
	printf("Usage:\n");
	printf("  f6-split-tool FILE\n\n");
}

/*
 * main
 */
int main(int argc, char *argv[]) {
	if(argc != 2) {
		usage();
		return 1;
	}
	
	return split_file(argv[1]);
}

/* eof */
