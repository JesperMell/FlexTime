/*
 * Visma.h
 *
 *  Created on: Nov 12, 2019
 *      Author: Jesper Mellquist
 */

#ifndef VISMA_H_
#define VISMA_H_

/* ----- INCLUDES ----- */

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

/* ----- MACROS ----- */

#define OFFSET_FOR_START_WORK 0
#define OFFSET_FOR_END_WORK 3
#define OFFSET_FOR_START_LUNCH 1
#define OFFSET_FOR_END_LUNCH 2

#define TIMESTAMP_COLUMNS 11
#define LINE_COLUMNS 40

// a.k.a 'fractions'
// TODO: DAYS_COUNT should have more appropriate name
// perhaps MAX_DAYS_IN_WEEK
#define DAYS_COUNT 5
#define TIMESTAMPS_COUNT 4

/* ----- STRUCTS ----- */

typedef struct {
	uint16_t day;
	uint8_t hour;
	uint8_t minute;
} Timestamp;


typedef struct {
	char name[20];
	char period[20];
	char number[20];
} Employee;

/* ----- FUNCTIONS ----- */

/*
 * export
 *
 * Prints the result of calculated and transformed input.
 * Also export the content to file.
 *
 * input: *flextimes.
 * output: void.
 */


void export(float *flextimes);


/*
 * read_timestamps_file
 *
 * Reads timestamps from file. Expects format to be 4
 * timestamps for each day. Each timestamp should contain 3
 * numbers representing day, hour and minute. Example day:
 * 308,07,08;308,11,55;308,12,08;308,16,30;
 *
 * input: none
 * output: pointer to file content as string.
 */


char *read_timestamps_file();


/*
 * format_input
 *
 * Convert segments of string to timestamps.
 * Each day segment must be seperated by ';', and
 * each timestamp segment must be seperated by ','.
 * Fills the first argument with new converted timestamps.
 *
 * input: *timestamps (the array which will be filled), data (string).
 * output: void.
 */

void format_input(Timestamp *timestamps, char *data);


/*
 * calculate_flextimes
 *
 * Calculate amount of minutes for each day segment.
 * Appends each calculated result to flextimes array.
 *
 * input: *fractions (the array which will be filled),
 *   timestamps (list of timestamps).
 * output: void.
 */

void calculate_flextimes(float *flextimes, Timestamp *timestamps);


/*
 * convert_to_minutes
 *
 * Convert timestamp data to minutes.
 * This is a helper function for calculate_flextimes.
 *
 * input: *timestamp.
 * output: int.
 */

int convert_to_minutes(Timestamp *timestamp);

#endif /* VISMA_H_ */
