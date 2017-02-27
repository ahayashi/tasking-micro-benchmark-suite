#include "timing.h"

#include <omp.h>
#include <stdio.h>
#include "task_spawn.h"

int main(int argc, char **argv) {
    int nthreads;
#pragma omp parallel default(none) shared(nthreads)
#pragma omp master
    {
        nthreads = omp_get_num_threads();
    }
    printf("Using %d OpenMP threads\n", nthreads);

#pragma omp parallel default(none)
#pragma omp master
    {

#pragma omp taskgroup
        {
            int incr = 0;
            int nlaunched = 0;
            const unsigned long long spawn_start_time = current_time_ns();

            do {
#pragma omp task default(none) firstprivate(incr)
                {
                    incr = incr + 1;
                }

                nlaunched++;
            } while (nlaunched < NTASKS);

            const unsigned long long spawn_end_time = current_time_ns();
            printf("METRIC task_create %d %.20f\n", NTASKS,
                    (double)NTASKS / ((double)(spawn_end_time -
                            spawn_start_time) / 1000.0));
        }

        int incr = 0;
        int nlaunched = 0;
        const unsigned long long schedule_start_time = current_time_ns();
#pragma omp taskgroup
        {
            nlaunched = 0;
            do {
#pragma omp task default(none) firstprivate(incr)
                {
                    incr = incr + 1;
                }

                nlaunched++;
            } while (nlaunched < NTASKS);
        }

        const unsigned long long schedule_end_time = current_time_ns();
        printf("METRIC task_run %d %.20f\n", NTASKS,
                (double)NTASKS / ((double)(schedule_end_time -
                        schedule_start_time) / 1000.0));
    }

    return 0;
}
