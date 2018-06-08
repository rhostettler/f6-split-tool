/*
 * f6-split-tool.c -- Version 0.0.2
 * Extracts RAW exercise data from the RAW sonic link data captured using 
 * rs200_decode for the Polar F6/F11 HRMs.
 *
 * Roland Hostettler <r.hostettler@gmx.ch>
 * 2008-01-31
 * 2008-04-10
 * 2008-09-04
 * 
 * Licensed under the GNU GPL v2
 */

/* includes */
#include <stdio.h>
#include <string.h>

/* local includes */
#include "f6-split-tool.h"
#include "f6.h"
#include "f11.h"

/* array of available monitors */
struct hrm_defs *monitors[2] = {&f6_defs, &f11_defs};

/*
 * splits the file into sub-files containing either diary, totals or an exercise
 * 
 * TODO: export diary
 * TODO: Header check
 */
int split_file(int hrm, char *infile) {
	FILE *fin = NULL;
	FILE *fout = NULL;
	char outfile[20];
	int i = 0;
	int j = 0;
	
	/* exercise data */
	unsigned int num_exercises = 0;
	unsigned int ex_read = 0;
	unsigned int ex_bytes_read = 0;

	/* diary data */
	unsigned int num_diary_weeks = 0;
	unsigned int diary_data_len = 0;
	unsigned int diary_bytes_read = 0;
	
	/* sections data */
	unsigned int num_sections = 0;
	unsigned int section_nr = 0;
	unsigned int section_data_len = 0;
	unsigned int sec_bytes_read = 0;
	
	/* other storage */
	unsigned char c = 0;
	unsigned char exetime_sec, exetime_min, exetime_h;
	unsigned char calsum_b1, calsum_b2, calsum_b3;
	
	/* open the input file */
	fin = fopen(infile, "rb");
	if(fin == NULL) {
		printf("Input file \"%s\" could not be opened, stop.\n", infile);
		return -1;
	}
	
	/* start reading the header */
	if(fgetc(fin) != monitors[hrm]->section_start_marker) {
		printf("Header start not found, stop.\n");
		return -1;
	}
	
	/*
	 * skip two unknown fields, then read the number of sections and
	 * skip the rest of the header (might be CRC-16, see rs200_decode)
	 */
	fseek(fin, 2, SEEK_CUR);
	num_sections = fgetc(fin);
	fseek(fin, monitors[hrm]->header_len-4, SEEK_CUR);
	
	/* start reading section 1 */
	if(fgetc(fin) != monitors[hrm]->section_start_marker) {
		printf("Section 1 start not found, stop.\n");
		return -1;
	}
	
	/* read section header:  */
	section_nr = fgetc(fin);
	section_data_len = fgetc(fin);
	sec_bytes_read = 0;

	/* 
	 * note: the totals are added to each exercise in the end and this is
	 * introduced by the f6-split-tool. this is only as an aid so that PolarViewer
	 * (part of SportsTracker) can display them
	 */
	/* read the (useful) totals stored in section 1
	   store the total execution time */
	fseek(fin, 1, SEEK_CUR);
	num_diary_weeks = fgetc(fin)+1;
	num_exercises = fgetc(fin);
	exetime_sec = fgetc(fin);
	exetime_min = fgetc(fin);
	exetime_h = fgetc(fin);
	sec_bytes_read += 6;
	
	/* store the total calories sum */
	fseek(fin, 3, SEEK_CUR);
	calsum_b1 = fgetc(fin);
	calsum_b2 = fgetc(fin);
	calsum_b3 = fgetc(fin);
	sec_bytes_read += 6;
	
	/* skip the totals' dates as we have no use for them right now */
	fseek(fin, 13, SEEK_CUR);
	sec_bytes_read += 13;

	/* calculate the diary data length */
	diary_data_len = num_diary_weeks*monitors[hrm]->diary_len;

	/*
	 * read the remaining data sections
	 * only exercise data marked by EXERCISE_START (0x80) is processed,
	 * the diary in front of the exercises is skipped
	 */
	for(j = section_nr; j <= num_sections; j++) {
		/* read the whole bunch of section data */
		for(i = sec_bytes_read; i < section_data_len; i++) {
			c = fgetc(fin);
			
			/* 
			 * check whether the read byte is a diary byte or an 
			 * exercise byte. In the first case, we just ignore that
			 * byte.
			 */
			if(diary_bytes_read < diary_data_len) {
				diary_bytes_read++;
			} else if(ex_read < num_exercises) {
				/*
				 * check whether we are expecting the beginning of an 
				 * exercise record and the marker was found
				 */
				if(ex_bytes_read == 0) {
					/* open the output file */
					sprintf(outfile, "exercise_%d.frd", ex_read);
					fout = fopen(outfile, "wb");
			
					if(fout == NULL) {
						printf("Error opening output file, stop.\n");
						return -1;
					}
				}

				/* if we're reading an exercise, write the data out */
				if(fout != NULL) {		
					fputc(c, fout);
					ex_bytes_read++;
				}
		
				/*
				 * close the output file and reset the counter if the
				 * whole exercise record has been read
				 *
				 * also append the total exercise time and calories 
				 * consumption to the exercies
				 */
				if(ex_bytes_read == monitors[hrm]->exercise_len) {
					fputc(exetime_sec, fout);
					fputc(exetime_min, fout);
					fputc(exetime_h, fout);
					fputc(calsum_b1, fout);
					fputc(calsum_b2, fout);
					fputc(calsum_b3, fout);
					fclose(fout);
					fout = NULL;
					ex_bytes_read = 0;
					ex_read++;
				}
			}
		}
		
		/* forward the remaining padding bytes */
		fseek(fin, monitors[hrm]->section_len-monitors[hrm]->section_header_len-section_data_len, SEEK_CUR);
		
		/* check and read the next section's header */
		c = fgetc(fin);
		if(c != monitors[hrm]->section_start_marker && c != monitors[hrm]->eof_marker) {
			printf("Section start or end-of-file not found, stop.\n");
			return -1;
		} else {		
			/* read the section header */
			section_nr = fgetc(fin);
			section_data_len = fgetc(fin);
			sec_bytes_read = 0;
		}
	}

	/* clean up and inform the user */
	fclose(fin);
	printf("Splitting completed, found %d exercise(s).\n", num_exercises);
	return 0;
}

/*
 * a little hint
 */
void usage(void) {
	printf("Usage:\n");
	printf("f6-split-tool TYPE FILE\n\n");
	printf("  TYPE\tHeartrate monitor type:\n");
	printf("    --f6\tPolar F6\n");
	printf("    --f11\tPolar F11\n\n");
	printf("  FILE\tBinary dumpfile\n\n");
}

/*
 * checks the supplied program arguments, returns the specified HRM type
 */
signed int check_args(int argc, char *argv[]) {
	/* check the number of supplied arguments */
	if(argc != 3) {
		return -1;
	/* check whether --f6 or --F6 was specified */
	} else if(!strcmp(argv[1], "--f6") || !strcmp(argv[1], "--F6")) {
		return HRM_TYPE_F6;
	/* check whether --f11 or --F11 was specified */
	} else if(!strcmp(argv[1], "--f11") || !strcmp(argv[1], "--F11")) {
		return HRM_TYPE_F11;
	/* no valid watch was specified -> error */
	} else {
		return -1;
	}

	return -1;
}

/*
 * main
 */
int main(int argc, char *argv[]) {
	int err = 0;
	int hrm = -1;
	
	/* check the arguments and determine the chosen HRM type (F6/F11) */
	hrm = check_args(argc, argv);
	if(hrm == -1) {
		usage();
		return 1;
	}
	
	/* split the dump */
	printf("Splitting exercises in file %s.\n", argv[2]);
	err = split_file(hrm, argv[2]);
	
	/* summary */
	if(err == 0) {
		printf("Successfully completed.\n");
	} else {
		printf("Stopped due to errors, see messages above.\n");
	}
	
	return err;
}

