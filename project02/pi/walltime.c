#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#include "pi-serial.h"
#include "pi-reduction.h"
#include "pi-critical.h"

int main(int argc, char *argv[])
{

    int SIZE_VEC = 1000000;
    char *size_vec_str = getenv("SIZE_VEC");
    if (size_vec_str != NULL)
    {
        SIZE_VEC = atoi(size_vec_str);
    }

    int tries = 100;
    double pi_serial = 0, pi_reduction = 0, pi_critical = 0;
    double start, end;
    int i;

    double serial_time = 0;
    for (i = 0; i < tries; i++)
    {
        start = omp_get_wtime();
        pi_serial += pi_serial_calc(SIZE_VEC);
        end = omp_get_wtime();
        serial_time += end - start;
    }
    serial_time /= tries;
    printf("Serial,%.10f,", pi_serial / tries);
    printf("%f,", serial_time);
    printf("%d,", omp_get_max_threads());
    printf("%f,", serial_time / serial_time);
    printf("%i\n", SIZE_VEC);

    double reduction_time = 0;
    for (i = 0; i < tries; i++)
    {
        start = omp_get_wtime();
        pi_reduction += pi_reduction_calc(SIZE_VEC);
        end = omp_get_wtime();
        reduction_time += end - start;
    }
    reduction_time /= tries;
    printf("Reduction,%.10f,", pi_reduction / tries);
    printf("%f,", reduction_time);
    printf("%d,", omp_get_max_threads());
    printf("%f,", serial_time / reduction_time);
    printf("%i\n", SIZE_VEC);

    double critical_time = 0;
    for (i = 0; i < tries; i++)
    {
        start = omp_get_wtime();
        pi_critical += pi_critical_calc(SIZE_VEC);
        end = omp_get_wtime();
        critical_time += end - start;
    }
    critical_time /= tries;
    printf("Critical,%.10f,", pi_critical / tries);
    printf("%f,", critical_time);
    printf("%d,", omp_get_max_threads());
    printf("%f,", serial_time / critical_time);
    printf("%i\n", SIZE_VEC);
    return 0;
}
