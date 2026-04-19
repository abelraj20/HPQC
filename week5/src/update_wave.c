#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

typedef struct
{
	int points;
	int cycles;
	int samples;
	char *output_path;
} SimulationConfig;

// declares the functions that will be called within main
// note how declaration lines are similar to the initial line
// of a function definition, but with a semicolon at the end;
SimulationConfig check_args(int argc, char **argv);
int is_positive_integer(char *text);
void initialise_vector(double vector[], int size, double initial);
void print_vector(double vector[], int size);
int sum_vector(int vector[], int size);
void update_positions(double* positions, double* velocities, double* accelerations, int points, double time, double dt, double k, double m);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	// gets all of the user inputs from the command line
	SimulationConfig config = check_args(argc, argv);

	// declare and initialise the numerical argument variable
	int points = config.points;

	// creates variables for the vibration
	int cycles = config.cycles; // number of cycles to show
	int samples = config.samples; // sampling rate in samples per cycle
	int time_steps = cycles * samples + 1; // total timesteps
	double step_size = 1.0/samples;

	// sets values for the spring constant and mass
	// I kept these fixed here to keep the model simple
	double k = 5.0;
	double m = 1.0;

	// creates a vector for the time stamps in the data
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	if (time_stamps == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for time stamps.\n");
		exit(-1);
	}
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	// creates vectors for the current positions, velocities and accelerations
	double* positions = (double*) malloc(points * sizeof(double));
	double* velocities = (double*) malloc(points * sizeof(double));
	double* accelerations = (double*) malloc(points * sizeof(double));

	if (positions == NULL || velocities == NULL || accelerations == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for state vectors.\n");
		free(time_stamps);
		if (positions != NULL) free(positions);
		if (velocities != NULL) free(velocities);
		if (accelerations != NULL) free(accelerations);
		exit(-1);
	}

	// and initialises every element to zero
	initialise_vector(positions, points, 0.0);
	initialise_vector(velocities, points, 0.0);
	initialise_vector(accelerations, points, 0.0);

	// creates a file
	FILE* out_file;
     	out_file = fopen(config.output_path,"w");

	// checks that the file opened properly before continuing
	if (out_file == NULL)
	{
		fprintf(stderr, "ERROR: Could not open output file %s\n", config.output_path);
		free(time_stamps);
		free(positions);
		free(velocities);
		free(accelerations);
		exit(-1);
	}

	print_header(&out_file, points);

	// iterates through each time step in the collection
	for (int i = 0; i < time_steps; i++)
	{
		// updates the position using a function
		update_positions(positions, velocities, accelerations, points, time_stamps[i], step_size, k, m);

		// prints an index and time stamp
		fprintf(out_file, "%d, %lf", i, time_stamps[i]);

		// iterates over all of the points on the line
		for (int j = 0; j < points; j++)
		{
			// prints each y-position to a file
			fprintf(out_file, ", %lf", positions[j]);
		}
		// prints a new line
		fprintf(out_file, "\n");
	}

	// if we use malloc, must free when done!
	free(time_stamps);
	free(positions);
	free(velocities);
	free(accelerations);

	// closes the file
	fclose(out_file);

	return 0;
}

// prints a header to the file
// double-pointer used to allow this function to move the file pointer
void print_header(FILE** p_out_file, int points)
{
	fprintf(*p_out_file, "#, time");
	for (int j = 0; j < points; j++)
	{
		fprintf(*p_out_file, ", y[%d]", j);
	}
	fprintf(*p_out_file, "\n");
}

// defines a simple harmonic oscillator as the driving force
double driver(double time)
{
	double value = sin(time*2.0*M_PI);
	return(value);
}

// defines a function to update the positions
void update_positions(double* positions, double* velocities, double* accelerations, int points, double time, double dt, double k, double m)
{
	// creates temporary vector variables for the new positions and velocities
	double* new_positions = (double*) malloc(points * sizeof(double));
	double* new_velocities = (double*) malloc(points * sizeof(double));

	// checks the temporary memory allocation as well
	if (new_positions == NULL || new_velocities == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for new state vectors.\n");
		if (new_positions != NULL) free(new_positions);
		if (new_velocities != NULL) free(new_velocities);
		exit(-1);
	}

	// the first point is driven directly by the oscillator
	new_positions[0] = driver(time);
	new_velocities[0] = 0.0;
	accelerations[0] = 0.0;

	// the last point is fixed so the string has an end point
	new_positions[points - 1] = 0.0;
	new_velocities[points - 1] = 0.0;
	accelerations[points - 1] = 0.0;

	// updates all of the internal points using spring forces
	for (int i = 1; i < points - 1; i++)
	{
		// applies Hooke's law to the left and right sides
		// F = -kx, written here using the displacement relative to neighbours
		double force_left = k * (positions[i - 1] - positions[i]);
		double force_right = k * (positions[i + 1] - positions[i]);
		double total_force = force_left + force_right;

		// uses F = ma to get acceleration
		accelerations[i] = total_force / m;

		// updates velocity and position with a simple time step
		new_velocities[i] = velocities[i] + accelerations[i] * dt;
		new_positions[i] = positions[i] + new_velocities[i] * dt;
	}

	// propagates these new positions and velocities to the old ones
	for (int i = 0; i < points; i++)
	{
		positions[i] = new_positions[i];
		velocities[i] = new_velocities[i];
	}

	// frees the temporary vectors
	free(new_positions);
	free(new_velocities);
}

// defines a set of timestamps
int generate_timestamps(double* timestamps, int time_steps, double step_size)
{
	for (int i = 0; i < time_steps ; i++)
	{
		timestamps[i]=i*step_size;
	}	
	return time_steps;
}

// defines a function to sum a vector of ints into another int
int sum_vector(int vector[], int size)
{
	// creates a variable to hold the sum
	int sum = 0;

	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// sets the elements of the vector to the initial value
		sum += vector[i];
	}

	// returns the sum
	return sum;
}

// defines a function to initialise all values in a vector to a given inital value
void initialise_vector(double vector[], int size, double initial)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// sets the elements of the vector to the initial value
		vector[i] = initial;
	}
}

// defines a function to print a vector of ints
void print_vector(double vector[], int size)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// prints the elements of the vector to the screen
		printf("%d, %lf\n", i, vector[i]);
	}
}

// checks whether a string contains a positive integer
int is_positive_integer(char *text)
{
	int i = 0;

	if (text[0] == '\0')
	{
		return 0;
	}

	for (i = 0; text[i] != '\0'; i++)
	{
		if (!isdigit((unsigned char) text[i]))
		{
			return 0;
		}
	}

	if (atoi(text) <= 0)
	{
		return 0;
	}

	return 1;
}

// defines a function that checks your arguments to make sure they'll do what you need
SimulationConfig check_args(int argc, char **argv)
{
	SimulationConfig config;

	// expected use:
	// ./string_wave_spring [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]

	// check the number of arguments
	if (argc == 5)
	{
		// checks that the integer inputs actually make sense
		if (!is_positive_integer(argv[1]) || !is_positive_integer(argv[2]) || !is_positive_integer(argv[3]))
		{
			fprintf(stderr, "ERROR: Points, cycles and samples must all be positive integers.\n");
			fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]\n", argv[0]);
			exit(-1);
		}

		// stores all of the command line values in the struct
		config.points = atoi(argv[1]);
		config.cycles = atoi(argv[2]);
		config.samples = atoi(argv[3]);
		config.output_path = argv[4];
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: You did not provide the correct arguments.\n");
		fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]\n", argv[0]);

		// and exit COMPLETELY
		exit (-1);
	}
	return config;
}
