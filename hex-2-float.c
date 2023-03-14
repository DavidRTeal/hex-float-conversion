#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>


   // CASE 1 -> NORMALIZED
   // CASE 2 -> DENORMALIZED
   // CASE 3 -> SPECIAL
#define NORMALIZED 1
#define DENORMALIZED 2
#define SPECIAL 3
#define BUFFER_SIZE 256
#define GETOPT_STRING "i:d h b m e:E f:F v H"
#define F_EXP_BITS_DEFAULT 8
#define F_FRAC_BITS_DEFAULT 23
#define F_FRAC_ADD_DEFAULT 1.0
int opt = 0;


int
main(int argc, char *argv[])
{
    long exp_bits = F_EXP_BITS_DEFAULT, frac_bits = F_FRAC_BITS_DEFAULT;
    double exp_bias = 0.0, frac_add = F_FRAC_ADD_DEFAULT;
    FILE *ofile = stdout;
    FILE *ifile = stdin;
    int is_verbose = 0;

    while((opt = getopt(argc, argv, GETOPT_STRING)) != -1) {
    
        switch(opt) {
	    case 'i': // specify input file
		ifile = fopen(optarg, "r");
		if(!ifile){
		    fprintf(stderr, "Error: couldn't open input file: %s\n", optarg);
		    exit(EXIT_FAILURE);
		}
		break;
	    case 'd': // double (64 bits, 1-11-52)
		exp_bits = 11;
		frac_bits = 52;
		exp_bias = 1023;

		break;

	    case 'h': // half (binary 16 bits, 1-5-10) 
		exp_bits = 5;
		frac_bits = 10;
		exp_bias = 15;
		break;

	    case 'b':
		exp_bits = 8;
		frac_bits = 7;
		exp_bias = 127;
		break;

	    case 'm': // quarter (bfloat 16 bits, 1-8-7)
		exp_bits = 4;
		frac_bits = 3;
		exp_bias = -2;
		break;

	    case 'e': // sets exponenet bits 
		exp_bits = atoi(optarg);
		break;

	    case 'E': // sets exponent bias 
		exp_bias = atoi(optarg);
		break;

	    case 'f': // sets fraction bits
		frac_bits = atoi(optarg);		
		break;

	    case 'F': // sets value to add to fraction
		frac_add = atoi(optarg);		
		break;


	    case 'v': // verbose thing (goes to stderr and needs to generate an output)
		// example
		// input file      : stdin
		// total bits      : 32
		// exponent bits   : 8
		// exponent bias   : 127
		// fraction bits   : 23
		// fraction add    : 1
		// read binary     : no
		// verbose         : yes
		is_verbose = 1;
		break;

	    case 'H': // Displays help message 
		     printf("\t%s [OPTION ...]\n", argv[0]);
		     printf("\tSettings default to single precision (float, 32-bits, 1-8-23)\n");
		     printf("\tInput defaults to stdin if -i <file_name> is not used\n");
		     printf("\t-i <file_name> specify the name of the input file\n");
		     printf("\t-d   use settings for double precision (double, 64-bits, 1-11-52)\n");
		     printf("\t-h   use settings for half precision (_Float16, 16-bits, 1-5-10)\n");
		     printf("\t-b   use settings for half precision (bfloat16, 16-bits, 1-8-7)\n");
		     printf("\t-m   use settings for quarter precision (minifloat, 8-bits, 1-4-3  bias -2)\n");
		     printf("\t-e # set the number of bits to use for the exponent\n");
		     printf("\t-E # set the value used for the exponent bias\n");
		     printf("\t-f # set the number of bits to use for the fraction\n");
		     printf("\t-F # set the value to add to the fraction (unstored fraction bits, normalized form)\n");
		     printf("\t-B   read the input as binary. Spaces are okay between fields, but no trailing comments (eg 1 001 1101)\n");
	   	     printf("\t-v   print the settings (to stderr) before reading input\n");
		     printf("\t-H   display this most wonderful message and exit\n");
		break;

	    default: // if no command line default it to float
		break;
	}
    }
	long total_bits = frac_bits + exp_bits + 1;	

	exp_bias = pow(2, (exp_bits - 1)) - 1;
	if(opt == 'm') {
	    exp_bias *= -1;
        }
    char buf[BUFFER_SIZE];
    while(fgets(buf, BUFFER_SIZE, ifile) != NULL) {

        double ub_exp = 0.0; // unbiased exponent
        double exp = 0.0;    // exponent (with bias)
        double plain_frac = 0.0;
        double frac = 0.0;
        double power = exp_bits - 1;
        double sign = 1.0; // always 1 or -1
	double value = 0.0;

	unsigned long uv = 0x0; 
        unsigned long mask = 1lu << (exp_bits + frac_bits);
        unsigned long frac_mask = 0x0lu;
        unsigned long sign_mask = 1lu << (exp_bits + frac_bits);
        unsigned long exp_mask = 1lu;
	int mode = 0; // this will say if it's normalized denormalized or special

	 if (sscanf(buf, "%lx", &uv) != 1) {
            fprintf(stderr, "Error: failed to read input value from string %s\n", buf);
            exit(EXIT_FAILURE);
        }

	printf("%s", buf);

	for(int i = 0; i < exp_bits - 1; i++) {
	    exp_mask <<= 1l; // build the mask
	    exp_mask |= 0x1lu;  // one bit at a time
	}

	exp_mask <<= frac_bits;

	frac_mask = ~(sign_mask | exp_mask);

	sign = (uv & mask) ? 1 : 0;

	ub_exp = (uv & exp_mask);

	plain_frac = (double) (uv & frac_mask) / pow(2.0, frac_bits);

	printf("\t%d ", (uv & mask) ? 1 : 0);
	for (int i = exp_bits; i > 0; i--) {
	    mask >>= 1;
	    printf("%d", (uv & mask) ? 1 : 0);
        }
	printf(" ");
	for (int i = frac_bits; i > 0; i--) {
	    mask >>= 1;
	    printf("%d", (uv & mask) ? 1 : 0);
	}	
	printf("\n\ts ");
	for(int i = exp_bits; i > 0; i--) {
	    mask >>= 1;
	    printf("e");
	}
	printf(" ");
	for(int i = frac_bits; i > 0; i--) {
	    mask >>=1;
	    printf("f");
	}
	printf("\n");

// DENORMALIZED
    if(ub_exp == 0) {
	//E IS 1 - BIAS
	exp = 1.0 - exp_bias;
	frac = plain_frac;
	mode = DENORMALIZED;
	}
// SPECIAL VALUE
    else if(ub_exp == exp_mask) {
	mode = SPECIAL;
	}
 // NORMALIZED
    else{
        exp = (unsigned long)ub_exp >> frac_bits;
	exp = ub_exp - exp_bias;
	frac = frac_add + frac;
	mode = NORMALIZED;
    }
	
   value = ((sign == 1) ? -1 : 1) * frac * pow(2, exp); 	
   // CASE 1 -> NORMALIZED
   // CASE 2 -> DENORMALIZED
   // CASE 3 -> SPECIAL

    switch (mode) {

        case NORMALIZED:

	    printf("\tnormalized value\n");
	    printf("\tsign:\t\t%s\n", (sign == 1) ? "negative" : "positive");
	    printf("\tbias:\t\t%-10.0lf\n", exp_bias);
	    printf("\tunbiased exp:\t%-10.0lf\n", ub_exp);
	    printf("\tE:\t\t%-10.0lf\n", exp);
	    printf("\tfrac:\t\t%-.20lf\n", plain_frac);
	    printf("\tM:\t\t%-.20lf\n", frac);
	    printf("\tvalue:\t\t%-.20lf\n", value);
    	    printf("\tvalue:\t\t%-.20le\n\n", value);
	    break;

        case DENORMALIZED:

	    printf("\tdenormalized value\n");
	    printf("\tsign:\t\t%s\n", (sign == 1) ? "negative" : "positive");
	    printf("\tbias:\t\t%-10.0lf\n", exp_bias);
	    printf("\tunbiased exp:\t%-10.0lf\n", ub_exp);
	    printf("\tE:\t\t%-10.0lf\n", exp);
	    printf("\tfrac:\t\t%-.20lf\n", plain_frac);
	    printf("\tM:\t\t%-.20lf\n", frac);
	    printf("\tvalue:\t\t%-.20lf\n", value);
    	    printf("\tvalue:\t\t%-.20le\n\n", value);
	    break;
   
        case SPECIAL:
	    printf("\tspecial value\n"); 	
	    if(frac == 0)
	    	printf("\t%s\n", (sign == 1) ? "negative infinity" : "positive infinity"); 
	    else {
	        printf("\tNaN\n");
	    }
	    break;
	}
   	    
   }
   if(is_verbose == 1) {
	// example
       	// input file      : stdin
	// total bits      : 32
	// exponent bits   : 8
	// exponent bias   : 127
	// fraction bits   : 23
	// fraction add    : 1
	// read binary     : no
	// verbose         : yes
	printf("total bits\t: %lu\n", total_bits);
	printf("exponent bits\t %lu\n", exp_bits);
	printf("exponent bias:\t %lu\n", exp_bias);
	printf("fraction bits:\t %lu\n", frac_bits);
	printf("fraction add:\t %lu\n", frac_add);
	printf("verbose:\d %lu\n", is_verbose);
        }

  

   return 0;
}
