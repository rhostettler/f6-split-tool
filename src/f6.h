/*
 * f6.h -- Version 0.0.3
 * Definitions for the F6 heartrate monitor
 *
 * Roland Hostettler <r.hostettler@gmx.ch>
 * 2008-09-03
 * 
 * Licensed under the GNU GPL v2
 */

#ifndef F6_H
#define F6_H

#include "f6-split-tool.h"

/* definitions */
#define	F6_HEADER_LENGTH		8
#define	F6_SECTION_LENGTH		255
#define	F6_SECTION_HEADER_LENGTH	3
#define	F6_DIARY_LENGTH			15
#define	F6_EXERCISE_LENGTH		43
#define	F6_SECTION_START		0x55
#define	F6_EXERCISE_START		0x80
#define	F6_EOF				0x07
#define	HRM_TYPE_F6			0

/* create the F6 struct */
struct hrm_defs f6_defs = {
		F6_HEADER_LENGTH, 
		F6_SECTION_LENGTH, 
		F6_SECTION_HEADER_LENGTH,
		F6_DIARY_LENGTH,
		F6_EXERCISE_LENGTH,
		F6_SECTION_START,
		F6_EXERCISE_START,
		F6_EOF
	};

#endif

