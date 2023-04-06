#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


#define NORMALIZED 1
#define DENORMALIZED 2
#define SPECIAL 3

// CASE 1 -> NORMALIZED
// CASE 2 -> DENORMALIZED
// CASE 3 -> SPECIAL
int mode = 0;

#define F_EXP_BITS_DEFAULT 8
#define F_FRAC_BITS_DEFAULT 23
#define F_FRAC_ADD_DEFAULT 1.0
#define BUFFER_SIZE 256
#define GETOPT_STRING "i:d h b m e:E f:F v H" 

int main (int argc, char *argv[]) 
{
    long exp_bits = F_EXP_BITS_DEFAULT;
    long frac_bits = F_FRAC_BITS_DEFAULT;
    double exp_bias = 0.0;
    int is_verbose = 0, opt = 0;

    FILE * ifile = stdin;
    char buf[BUFFER_SIZE];
    unsigned long uv;

    while ((opt = getopt(argc, argv, GETOPT_STRING)) != -1) 
    {
        switch (opt) {
            case 'i':
                ifile = fopen(optarg, "r");
		if(!ifile) {
		    fprintf(stderr, "Error: couldn't open input file: %s\n", optarg);
                    exit(EXIT_FAILURE);
		}
                break;

            case 'd':
                exp_bits = 11;
                frac_bits = 52;
                exp_bias = 1023;
                break;

            case 'h':
                exp_bits = 5;
                frac_bits = 10;
                exp_bias = 15;
                break;

            case 'b':
                exp_bits = 8;
                frac_bits = 7;
                exp_bias = 127;
                break;

            case 'm':
                exp_bits = 4;
                frac_bits = 3;
                exp_bias = -2;
                break;

            case 'e':
                exp_bits = atoi(optarg);
                break;

            case 'E':
                exp_bias = atoi(optarg);
                break;

            case 'f':
                frac_bits = atoi(optarg);
                break;

            case 'F':
                frac_bits = atoi(optarg);
                break;

            case 'v':
                is_verbose = 1;
                fprintf(stderr, "Verbose output.\n");
                break;

            case 'H':
                printf("Usage: %s [OPTIONS]\n", argv[0]);
                printf("Convert hexadecimal floating-point numbers to decimal.\n");
                printf("\n");
                printf("OPTIONS:\n");
                printf("    -i FILE Read input from FILE instead of stdin.\n");
                printf("    -d  Convert double-precision (64-bit) floating-point numbers.\n");
                printf("    -h  Convert half-precision (16-bit) floating-point numbers.\n");
                printf("    -b  Convert single-precision (32-bit) floating-point numbers.\n");
                printf("    -m  Convert 8-bit floating-point numbers.\n");
                printf("    -e BITS Use BITS bits for the exponent (default: %ld).\n", exp_bits);
                printf("    -E BIAS Use BIAS as the exponent bias (default: %f).\n", exp_bias);
                printf("    -f BITS Use BITS bits for the fraction (default: %ld).\n", frac_bits);
                printf("    -v  Verbose output.\n");
                printf("    -H  Print this help message.\n");
                exit(EXIT_SUCCESS);
                break;

            default:
                exit(EXIT_FAILURE);
                break;
        }
    }

       if(exp_bias == 0.0)
           exp_bias = pow(2, (exp_bits - 1)) - 1;

       if (opt == 'm')
	    exp_bias *= -1;

    while(fgets(buf, BUFFER_SIZE, ifile))
    {
        unsigned long ub_exp = 0;
        unsigned long exp = 0; 
        double frac = 0.0;
        double sign = 1.0; 
        double value = 0.0;
	double E = 0.0;
	double mult_sign = 1.0;
        unsigned long mask = 1lu << (exp_bits + frac_bits);
        unsigned long frac_mask = 0x0lu;
        unsigned long sign_mask = 1lu << (exp_bits + frac_bits);
        unsigned long exp_mask = 0x1lu;

        sscanf(buf, "%lx", &uv);

        for(int i = 0; i < exp_bits - 1; i++) 
        {
            exp_mask <<= 1l;        
            exp_mask |= 0x1lu;      
        }
        exp_mask <<= frac_bits;    
                                    
        frac_mask = ~(sign_mask | exp_mask);
        
        sign = (uv & mask) ? 1 : 0;

        exp = (uv & exp_mask);

        frac = (double)(uv & frac_mask) / pow(2.0, frac_bits);

        printf("%s", buf);
        printf("\t%d ", (uv & mask) ? 1 : 0);
        for (int i = exp_bits; i > 0; i--) {
            mask >>= 1;
            printf("%d",  (uv & mask) ? 1 : 0);
        }
        printf(" ");
        for (int i = frac_bits; i > 0; i--) {
            mask >>= 1;
            printf("%d", (uv & mask) ? 1 : 0);
        }
        printf("\n\ts ");
        for (int i = exp_bits; i > 0; i--) {
            mask >>= 1;
            printf("e");
        }
        printf(" ");
        for (int i = frac_bits; i > 0; i--) {
            mask >>= 1;
            printf("f");
        }
        // SPECIAL
        if (exp == exp_mask) {
	    mode = SPECIAL; 
        }
        else {
	// DENORMALIZED
            if (exp == 0)
            {
                if (sign == 1)
                    mult_sign = -1.0;
                
                ub_exp = 1 - (unsigned long)exp_bias;
                E = 1.0 - exp_bias;
                value = mult_sign * frac * pow(2.0, E);
		mode = DENORMALIZED;
            }
	
            else {
	    // NORMALIZED
                if (sign == 1)
                    mult_sign = -1.0;
                
                ub_exp = exp >> frac_bits;
                E = ub_exp - exp_bias;
                value = mult_sign * (1 + frac) * pow(2, E);
		mode = NORMALIZED;
            }
        }

        switch (mode) {

        case NORMALIZED:

            printf("\n\tnormalized value\n");
            printf("\tsign:\t\t%s\n", sign > 0 ? "negative" : "positive");
            printf("\tbias:\t\t%-10.0lf\n", exp_bias);
            printf("\tunbiased exp:\t%-10.0ld\n", ub_exp);
            printf("\tE:\t\t%-10.0lf\n", E);
            printf("\tfrac:\t\t%-.20lf\n", frac);
            printf("\tM:\t\t%-.20lf\n", 1 + frac);
            printf("\tvalue:\t\t%-.20lf\n", value);
            printf("\tvalue:\t\t%-.20le\n\n", value);

	    break;

        case DENORMALIZED:

            printf("\n\tdenormalized value\n");
            printf("\tsign:\t\t%s\n", sign > 0 ? "negative" : "positive");
            printf("\tbias:\t\t%-10.0lf\n", exp_bias);
            printf("\tunbiased exp:\t%-10lx\n", exp);
            printf("\tE:\t\t%-10ld\n", ub_exp);
            printf("\tfrac:\t\t%-.20lf\n", frac);
            printf("\tM:\t\t%-.20lf\n", frac);
            printf("\tvalue:\t\t%-.20lf\n", value);
            printf("\tvalue:\t\t%-.20le\n\n", value);
	    break;
   
        case SPECIAL:
	    printf("\n\tspecial value\n"); 	
	    if(frac == 0) {
	    	printf("\t%s\n\n", (sign == 1) ? "negative infinity" : "positive infinity"); 
	    }
	    else {
	        printf("\tNaN\n\n");
	    }
	    break;

	}

	
        if (is_verbose)
            fprintf(stderr, "verbose mode on:\n");
    }
    if (ifile != stdin)
        fclose(ifile);

    return EXIT_SUCCESS;
}
