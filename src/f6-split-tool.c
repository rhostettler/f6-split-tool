/*
 * f6-split-tool.c -- Version 0.0.2
 * Extracts RAW exercise data from the RAW sonic link data captured using 
 * rs200_decode for the Polar F6 HRM.
 *
 * Roland Hostettler <r.hostettler@gmx.ch>
 * 2008-01-31
 * 2008-04-10
 * 
 * License under the GNU GPL v2
 * 
 * Note: This is only a temporary solution until a more sophisticated tool is 
 * available.
 */

/* includes */
#include <stdio.h>

/* definitions */
#define	EXERCISES_COUNT		12
#define HEADER_LENGTH		8
#define SECTION_LENGTH		255
#define	SECTION_HEADER_LENGTH	3
#define	EXERCISE_LENGTH		43
#define SECTION_START		0x55
#define	EXERCISE_START		0x80
#define	FILE_END		0x07

/*
 * splits the file into sub-files containing either diary, totals or an exercise
 * 
 * TODO: export totals
 * TODO: export diary
 * TODO: Header check, etc.
 */
int split_file(char *infile) {
	FILE *fin = NULL;
	FILE *fout = NULL;
	char outfile[20];
	unsigned char c = 0;
	int i = 0;
	int j = 0;
	int sec_bytes_read = 0;
	int ex_bytes_read = 0;
	int ex_read = 0;
	int num_sections = 0;
	int section_nr = 0;
	int section_data_len = 0;
	unsigned char exetime_sec, exetime_min, exetime_h;
	unsigned char calsum_b1, calsum_b2, calsum_b3;
	
	/* open the input file */
	fin = fopen(infile, "rb");
	if(fin == NULL) {
		printf("Input file \"%s\" could not be opened, stop.\n", infile);
		return -1;
	}
	
	/* start reading the header */
	if(fgetc(fin) != SECTION_START) {
		printf("Header start not found, stop.\n");
		return -1;
	}
	
	/*
	 * skip two unknown fields, then read the number of sections and
	 * skip the rest of the header (might be CRC-16, see rs200_decode)
	 */
	fseek(fin, 2, SEEK_CUR);
	num_sections = fgetc(fin);
	fseek(fin, HEADER_LENGTH-4, SEEK_CUR);
	
	/* start reading section 1 */
	if(fgetc(fin) != SECTION_START) {
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
	fseek(fin, 3, SEEK_CUR);
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
	
	/* skip the totals dates as we have no use for that right now */
	fseek(fin, 13, SEEK_CUR);
	sec_bytes_read += 13;
	
	/*
	 * read the remaining data sections
	 * only exercise data marked by EXERCISE_START (0x80) is processed,
	 * the diary in front of the exercises is skipped
	 */
	for(j = 0; j < num_sections; j++) {
		/* read the whole bunch of section data */
		for(i = sec_bytes_read; i < section_data_len; i++) {
			c = fgetc(fin);
			
			/*
			 * check wheter we are expecting the beginning of an 
			 * exercise record and the marker was found
			 */
			if(c == EXERCISE_START && ex_bytes_read == 0) {
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
			if(ex_bytes_read == EXERCISE_LENGTH) {
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
		
		/* forward the remaining padding bytes */
		fseek(fin, SECTION_LENGTH-SECTION_HEADER_LENGTH-section_data_len, SEEK_CUR);
		
		/* check for the section header */
		c = getc(fin);
		if(c != SECTION_START && c != FILE_END) {
			printf("Section start or end-of-file not found, stop.\n");
			return -1;
		} else {		
			/* read the section header */
			section_nr = fgetc(fin);
			section_data_len = fgetc(fin);
			sec_bytes_read = 0;
		}
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
	int err = 0;

	if(argc != 2) {
		usage();
		return 1;
	}
	
	printf("Splitting exercises in file %s.\n", argv[1]);
	err = split_file(argv[1]);
	
	if(err == 0) {
		printf("Successfully completed.\n");
	} else {
		printf("Stopped due to errors, see messages above.\n");
	}
	
	return err;
}

