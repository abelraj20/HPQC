#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <mpi.h>

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
SimulationConfig check_args(int argc, char **argv, int rank);
int is_positive_integer(char *text);
void initialise_vector(double vector[], int size, double initial);
void print_vector(double vector[], int size);
int sum_vector(int vector[], int size);
void update_positions_parallel(double* positions, int local_points, double time, int rank, int size, MPI_Comm comm);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);
int get_local_points(int points, int rank, int size);
void build_counts_displs(int points, int size, int counts[], int displs[]);

int main(int argc, char **argv)
{
	// starts MPI before the rest of the program runs
	MPI_Init(&argc, &argv);

	// gets the rank and total number of processes
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// gets all of the user inputs from the command line
	SimulationConfig config = check_args(argc, argv, rank);

	// stops all ranks if the arguments were not valid
	if (config.points < 1 || config.cycles < 1 || config.samples < 1)
	{
		MPI_Finalize();
		return -1;
	}

	// this version works best when every rank owns at least one point
	if (config.points < size)
	{
		if (rank == 0)
		{
			fprintf(stderr, "ERROR: Number of points must be at least the number of MPI processes.\n");
		}
		MPI_Finalize();
		return -1;
	}

	// declare and initialise the numerical argument variable
	int points = config.points;

	// creates variables for the vibration
	int cycles = config.cycles; // number of cycles to show
	int samples = config.samples; // sampling rate in samples per cycle
	int time_steps = cycles * samples + 1; // total timesteps
	double step_size = 1.0/samples;

	// creates a vector for the time stamps in the data
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	if (time_stamps == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for time stamps on rank %d.\n", rank);
		MPI_Abort(MPI_COMM_WORLD, -1);
	}
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	// works out how many points belong to this rank
	int local_points = get_local_points(points, rank, size);

	// creates a vector variable for the current positions
	double* positions = (double*) malloc(local_points * sizeof(double));
	if (positions == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for positions on rank %d.\n", rank);
		free(time_stamps);
		MPI_Abort(MPI_COMM_WORLD, -1);
	}
	// and initialises every element to zero
	initialise_vector(positions, local_points, 0.0);

	// creates a file
	FILE* out_file = NULL;
	if (rank == 0)
	{
     		out_file = fopen(config.output_path,"w");

		// checks that the file opened properly before continuing
		if (out_file == NULL)
		{
			fprintf(stderr, "ERROR: Could not open output file %s\n", config.output_path);
			free(time_stamps);
			free(positions);
			MPI_Abort(MPI_COMM_WORLD, -1);
		}

		print_header(&out_file, points);
	}

	// only rank 0 needs the full counts and displacements for gathering
	int* counts = NULL;
	int* displs = NULL;
	double* gathered_positions = NULL;

	if (rank == 0)
	{
		counts = (int*) malloc(size * sizeof(int));
		displs = (int*) malloc(size * sizeof(int));
		gathered_positions = (double*) malloc(points * sizeof(double));

		if (counts == NULL || displs == NULL || gathered_positions == NULL)
		{
			fprintf(stderr, "ERROR: Could not allocate memory for gathering on rank 0.\n");
			if (counts != NULL) free(counts);
			if (displs != NULL) free(displs);
			if (gathered_positions != NULL) free(gathered_positions);
			free(time_stamps);
			free(positions);
			fclose(out_file);
			MPI_Abort(MPI_COMM_WORLD, -1);
		}

		build_counts_displs(points, size, counts, displs);
	}

	// iterates through each time step in the collection
	for (int i = 0; i < time_steps; i++)
	{
		// updates the position using a function
		update_positions_parallel(positions, local_points, time_stamps[i], rank, size, MPI_COMM_WORLD);

		// gathers the local chunks back to rank 0 so only one rank writes the file
		MPI_Gatherv(positions, local_points, MPI_DOUBLE,
		            gathered_positions, counts, displs, MPI_DOUBLE,
		            0, MPI_COMM_WORLD);

		if (rank == 0)
		{
			// prints an index and time stamp
			fprintf(out_file, "%d, %lf", i, time_stamps[i]);

			// iterates over all of the points on the line
			for (int j = 0; j < points; j++)
			{
				// prints each y-position to a file
				fprintf(out_file, ", %lf", gathered_positions[j]);
			}
			// prints a new line
			fprintf(out_file, "\n");
		}
	}

	// if we use malloc, must free when done!
	free(time_stamps);
	free(positions);

	if (rank == 0)
	{
		free(counts);
		free(displs);
		free(gathered_positions);

		// closes the file
		fclose(out_file);
	}

	MPI_Finalize();
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
void update_positions_parallel(double* positions, int local_points, double time, int rank, int size, MPI_Comm comm)
{
	// creates a temporary vector variable for the new positions
	double* new_positions = (double*) malloc(local_points * sizeof(double));

	// checks the temporary memory allocation as well
	if (new_positions == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for new positions on rank %d.\n", rank);
		MPI_Abort(comm, -1);
	}

	// this stores the value coming from the rank to the left
	double incoming_value = 0.0;

	// sends the last old value to the rank on the right and receives from the left
	double outgoing_value = positions[local_points - 1];
	MPI_Sendrecv(&outgoing_value, 1, MPI_DOUBLE, (rank == size - 1) ? MPI_PROC_NULL : rank + 1, 0,
	             &incoming_value, 1, MPI_DOUBLE, (rank == 0) ? MPI_PROC_NULL : rank - 1, 0,
	             comm, MPI_STATUS_IGNORE);

	// initialises the index
	int i = 0;

	// rank 0 is still driven by the oscillator, same as the serial version
	if (rank == 0)
	{
		new_positions[i] = driver(time);
	}
	else
	{
		// every other rank gets its first value from the chunk to the left
		new_positions[i] = incoming_value;
	}

	// creates new positions by setting value of previous element
	for (i = 1; i < local_points; i++)
	{
		new_positions[i] = positions[i-1];
	}

	// propagates these new positions to the old ones
	for (i = 0; i < local_points; i++)
	{
		positions[i] = new_positions[i];
	}

	// frees the temporary vector
	free(new_positions);
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

// this works out how many points each rank should own
int get_local_points(int points, int rank, int size)
{
	int base = points / size;
	int remainder = points % size;

	if (rank < remainder)
	{
		return base + 1;
	}
	else
	{
		return base;
	}
}

// this builds the gather information for rank 0
void build_counts_displs(int points, int size, int counts[], int displs[])
{
	int offset = 0;

	for (int r = 0; r < size; r++)
	{
		counts[r] = get_local_points(points, r, size);
		displs[r] = offset;
		offset += counts[r];
	}
}

// defines a function that checks your arguments to make sure they'll do what you need
SimulationConfig check_args(int argc, char **argv, int rank)
{
	SimulationConfig config;

	// sets bad defaults first so the program can stop cleanly if needed
	config.points = -1;
	config.cycles = -1;
	config.samples = -1;
	config.output_path = NULL;

	// expected use:
	// mpirun -np [PROCESSES] ./string_wave_mpi [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]

	// check the number of arguments
	if (argc == 5)
	{
		// checks that the integer inputs actually make sense
		if (!is_positive_integer(argv[1]) || !is_positive_integer(argv[2]) || !is_positive_integer(argv[3]))
		{
			if (rank == 0)
			{
				fprintf(stderr, "ERROR: Points, cycles and samples must all be positive integers.\n");
				fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]\n", argv[0]);
			}
		}
		else
		{
			// stores all of the command line values in the struct
			config.points = atoi(argv[1]);
			config.cycles = atoi(argv[2]);
			config.samples = atoi(argv[3]);
			config.output_path = argv[4];
		}
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		if (rank == 0)
		{
			fprintf(stderr, "ERROR: You did not provide the correct arguments.\n");
			fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]\n", argv[0]);
		}
	}
	return config;
}
