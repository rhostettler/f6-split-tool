/*
 * f6.h -- Version 0.0.3
 * Definitions for the F11 heartrate monitor
 *
 * Roland Hostettler <r.hostettler@gmx.ch>
 * 2008-09-03
 * 
 * Licensed under the GNU GPL v2
 */

#ifndef F11_H
#define F11_H

#include "f6-split-tool.h"

/* definitions */
#define	F11_HEADER_LENGTH		8
#define	F11_SECTION_LENGTH		255
#define	F11_SECTION_HEADER_LENGTH	3
#define	F11_DIARY_LENGTH		15
#define	F11_EXERCISE_LENGTH		44
#define	F11_SECTION_START		0x55
#define	F11_EXERCISE_START		0x81
#define	F11_EOF				0x07
#define	HRM_TYPE_F11			1

/* F11 struct */
struct hrm_defs f11_defs = {
		F11_HEADER_LENGTH, 
		F11_SECTION_LENGTH, 
		F11_SECTION_HEADER_LENGTH,
		F11_DIARY_LENGTH,
		F11_EXERCISE_LENGTH,
		F11_SECTION_START,
		F11_EXERCISE_START,
		F11_EOF
	};

#endif

