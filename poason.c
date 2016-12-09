#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

int poisson(double lambda){
    int k=0;
    double L=exp(-lambda), p=1;
    do 
    {
        ++k;
        p *= rand()/(double)INT_MAX;
    } 
    while (p > L);
    return --k;
}

int main(int argc, char**argv) {
	srand(time(NULL));
    int in = atoi(argv[--argc]);
    int i;
    
    for (i=0; i<100; i++) {
		printf("%d\n", poisson(in));
	}
	
}
