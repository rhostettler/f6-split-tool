/*
 * f6-split-tool.h -- Version 0.0.3
 * Common defintions
 *
 * Roland Hostettler <r.hostettler@gmx.ch>
 * 2008-09-03
 * 
 * Licensed under the GNU GPL v2
 */

#ifndef F6_SPLIT_TOOL_H
#define F6_SPLIT_TOOL_H

/* structure to save the hrm-file data */
struct hrm_defs {
	unsigned int header_len;
	unsigned int section_len;
	unsigned int section_header_len;
	unsigned int diary_len;
	unsigned int exercise_len;
	unsigned int section_start_marker;
	unsigned int exercise_start_marker;
	unsigned int eof_marker;
};

#endif

