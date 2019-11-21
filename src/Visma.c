/*
 * Visma.c
 *
 *  Created on: Nov 13, 2019
 *      Author: Jesper Mellquist
 */

#include"Visma.h";

char *filename;
char root_path[] = "tmp/";

int total_days = 10;
int total_weeks = 2;

Employee *employee = NULL;

Employee *intialize_employee_from_filename() {
	// I don't want to use global filename string,
	// so I think it suits better to copy filename to
	// local string.
	char filename_temp[25] = {0};
	strcpy(filename_temp, filename);

	if((employee = malloc(sizeof(Employee))) == NULL) {
		printf("Could not allocate memory for employee");
	}

	char *token;
	int i = 0;

	token = strtok(filename_temp, "_");
	while(token != NULL) {
		switch(i++) {
		case 0:
			strcpy(employee->name, token);
			break;
		case 1:
			strcpy(employee->number,token);
			break;
		case 2:
			strcpy(employee->period, token);
			break;
		};
		token = strtok(NULL, "_");
	}

	// In case someone would like to use the
	// pointer for testing, I set the global
	// variable and return the employee address.
	return employee;
}


/* --- read_timestamp_file --- */

char *read_timestamps_file() {
	FILE *f;

	// declare the complete file path. root_path[] + filename[]
	char file_path[] = {0};

	// construct the full filename path.
	strcat(file_path, root_path);
	strcat(file_path, filename);
	strcat(file_path, ".txt");

	if((f = fopen(file_path, "rt")) == NULL) {
		printf("Could not open this file: %s", file_path);
		exit(1);
	}
	char *data_ptr = NULL;

	if((data_ptr = malloc(sizeof(char) * total_days * LINE_COLUMNS)) == NULL) {
		printf("Not enough memory");
	}

	// Remove all those random values by declaring it to '0'.
	char temp[TIMESTAMP_COLUMNS + 5] = {0};

	// Keep track of current timestamp of day.
	int i = 0;

	do {
		fgets(temp, TIMESTAMP_COLUMNS, f);

		// In case the loop for fetching timestamps
		// reaches a newline character, the loop should
		// append zeros until the day is filled.
		if(temp[0] == '\n' || feof(f)) {
			while(i < TIMESTAMPS_COUNT) {
				strcat(data_ptr, "000,00,00;");
				i++;
			}
			// Go to next line and restart current timestamp
			// when day is filled.
			i = 0;
			continue;
		}

		// Concatenate the line to the allocated memory.
		strcat(data_ptr, temp);

		// Restart current timestamp if max timestamps is reached.
		i<TIMESTAMPS_COUNT ? (i++) : (i=0);
	} while(!feof(f));

	fclose(f);

	return data_ptr;
}


/* ----- convert_to_minutes ----- */


int convert_to_minutes(Timestamp *timestamp) {
	return (timestamp->hour * 60) + timestamp->minute;
}


/* ----- calculate_flextimes ----- */


void calculate_flextimes(float *flextimes, Timestamp *timestamps) {
	Timestamp *temp = NULL;
	float time;
	int work_start_min, work_end_min, lunch_start_min, lunch_end_min, i = 0;

	for(temp = timestamps; temp < timestamps + (total_days*TIMESTAMPS_COUNT); temp+=TIMESTAMPS_COUNT) {
		work_start_min  = convert_to_minutes(temp + OFFSET_FOR_START_WORK);
		work_end_min    = convert_to_minutes(temp + OFFSET_FOR_END_WORK);
		lunch_start_min = convert_to_minutes(temp + OFFSET_FOR_START_LUNCH);
		lunch_end_min   = convert_to_minutes(temp + OFFSET_FOR_END_LUNCH);

		// This probably looks rather complicated. The following lines calculates
		// minutes for pre noon and after noon, add them together, and lastly divides the
		// sum by 60. The next line determine if the time record should be
		// full (8) or the previous calculated quota. If calculation equals to zero,
		// the time record should be full, with other words: missing full day.
		time = ( (lunch_start_min-work_start_min) + (work_end_min - lunch_end_min) )/60.0f;
		*(flextimes + (i++)) = (time == 0.0f ? 8.0f : time);
	}
}

/* ----- format_input ----- */


void format_input(Timestamp *timestamps, char *data) {
	char *end_data;

	// Start by slicing the string.
	char *token = strtok_r(data, ";", &end_data);
	char *day, *hour, *min;
	char *end_token;

	int i = 0;

	while(token != NULL) {

		// Expects token to be 'int,int,int'.
		day  = strtok_r(token, ",", &end_token);
		hour = strtok_r(NULL, ",", &end_token);
		min  = strtok_r(NULL, ",", &end_token);

		// Assign data to struct.
		(timestamps + (i))->day = atoi(day);
		(timestamps + (i))->hour = atoi(hour);
		(timestamps + (i))->minute = atoi(min);

		// Go to next index in memory.
		i++;

		// Go to next ';' slice.
		token = strtok_r(NULL, ";", &end_data);
	}

	// I do not like that missing timestamps gets an random number.
	// So I think it suits better to set them to zero.
	while(i < (total_weeks * DAYS_COUNT * TIMESTAMPS_COUNT)) {
		(timestamps + (i))->day = 0;
		(timestamps + (i))->hour = 0;
		(timestamps + (i))->minute = 0;
		i++;
	}
}


/* ----- export ----- */


void export(float *flextimes) {
	if(employee == NULL) {
		printf("Employee is NULL, please run 'intialize_employee_from_filename' before this function.");
		printf("\nThe program will terminate.\n");
		exit(1);
	}

	FILE *f;
	float total = 0, flex = 0;
	char export_filename[20] = {0}, prefix = '+';

	// Construct the filename.
	strcat(export_filename, employee->name);
	strcat(export_filename, "_");
	strcat(export_filename, employee->number);
	strcat(export_filename, "_");
	strcat(export_filename, employee->period);
	strcat(export_filename, "DONE.dat");

	// Open the file.
	f = fopen(export_filename,"wt");

	// Print the data to terminal and print to file.
	for(int i = 0; i < total_weeks; i++) {
		printf("week %d: ", i + 1);
		fprintf(f, "week %d: ", i + 1);

		for(int j = (DAYS_COUNT * i); j < ((DAYS_COUNT * i) + DAYS_COUNT) && (j < total_days); j++) {
			printf("%.2f ", *(flextimes + j));
			fprintf(f, "%.2f ", *(flextimes + j));

			total += *(flextimes + j);
		}
		printf("\n");
		fputc('\n', f);
	}

	flex = total - (8 * total_days);

	// It feels so unnecessary to include
	// Math.h lib only to convert negative number
	// to positive. I think this solution is enough.
	if(flex >= 0) {
		prefix = '+';
	} else {
		prefix = '-';
		flex = flex - flex - flex;
	}

	printf("\nPeriod time: %.2f (%c%.2f)", total, prefix, flex);
	fprintf(f, "\nPeriod time: %.2f (%c%.2f)", total, prefix, flex);

	fclose(f);

}
