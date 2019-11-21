/*
 * VismaMain.c
 *
 *  Created on: Nov 12, 2019
 *      Author: Jesper Mellquist
 */

#include"Visma.h"

extern char *filename;

extern int total_weeks;
extern int total_days;

extern Employee *employee;

int main(int argc, char *argv[]) {
	filename = argv[1];

	// Declare timestamps and fraction array.
	// timestamps should contain structs (uint16_t year, uint8_t hour, uint8_t min).
	// fractions should contain floats with calculated time spent on work.
	Timestamp timestamps[total_weeks][DAYS_COUNT][TIMESTAMPS_COUNT];
	float flextimes[total_days];

	// NB: This function allocated memory for global
	// employee variable. Don't forget to release it.
	intialize_employee_from_filename();


	// Get data from file. the 'read_timestamps_file' will allocate
	// memory (sizeof(char) * total_weeks * total_days * LINE_COLUMNS)).
	// Which means.. don't forget to free the return value!!!
	char *data_ptr = NULL;
	data_ptr = read_timestamps_file();


	// Convert the string to timestamps.
	format_input(timestamps, data_ptr);


	// Calculate each timestamp to floats (all timestamps in a day to a float)
	// and place them to fractions.
	calculate_flextimes(flextimes, timestamps);


	// Export the beautiful content to a .dat file.
	export(flextimes);

	// Good job!
	free(data_ptr);

	// This is tricky. intialize_employee_from_filename function
	// does allocate memory for employee, which means it needs
	// to be released.
	free(employee);

	return 0;
}

