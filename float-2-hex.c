#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define BUFFER_SIZE 256
#define GETOPT_STRING "fdH"
FILE *ofile = NULL;


int opt = 0;
int is_double = 0;
double double_num = 0.0;
float float_num = 0.0;
char buf[BUFFER_SIZE] = {'\0'};

int
main(int argc, char *argv[])
{
    FILE *ifile = stdin;
    ofile = stdout; 


    while((opt = getopt(argc, argv, GETOPT_STRING)) != -1) {
    
        switch(opt) {
	    case 'f':
	        // convert input into C standard float. (default)
		is_double = 0;
	        break;

	    case 'd':
		// convert input C standrd double 
		is_double = 1;
		break;

	    case 'H':
		// display help message 
		printf("Usage ./float-2-hex [OPTION...]\n");
		printf("\t-f convert the input into floats for hex output (this is the default)\n");
		printf("\t-d convert the input into doubles for hex output\n");
		printf("\t-H display this help message and exit\n");
		exit(EXIT_SUCCESS);
		break;
	    default:
		fprintf(stderr, "Invalid command line option\n");
		exit(EXIT_FAILURE);
		break;
	}
  
    }


    while(fgets(buf, BUFFER_SIZE, ifile) != NULL) {

	buf[strlen(buf) - 1] = '\0';
	
	if(is_double == 1) {
	    if(sscanf(buf, "%lf", &double_num) != 1) {
		fprintf(stderr, "Unable to scan value from input <%s>\n", buf);
		continue;
	    }
	printf("%-40s\t%.16le\t%.16lf\t0x%016lx\n", buf, double_num, double_num, *(unsigned long *)&double_num);
	}

	else {
	    if (sscanf(buf, "%f", &float_num) != 1) {
		fprintf(stderr, "Unable to scan value from input <%s>\n", buf);
		continue;
		}
	printf("%-40s\t%.10e\t%.10f\t0x%08x\n", buf, float_num, float_num, *(unsigned int *)&float_num);
	}
    }

    return EXIT_SUCCESS;
}
