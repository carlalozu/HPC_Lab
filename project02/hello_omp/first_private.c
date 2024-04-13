#include <stdio.h>
#include <omp.h>

int main()
{
    int shared_variable = 3;

// Parallel region starts
#pragma omp parallel firstprivate(shared_variable)
    {
        int thread_id = omp_get_thread_num();
        shared_variable += thread_id; // Each thread will update its private copy of shared_variable

        // Output the private copy for each thread
        printf("Thread %d: shared_variable = %d\n", thread_id, shared_variable);
    }
    // Parallel region ends

    // The original shared_variable remains unchanged
    printf("After parallel region, shared_variable =  %d\n", shared_variable);

    return 0;
}
