#include "timing.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/task_group.h"

#include <stdio.h>
#include "task_spawn.h"

int main(int argc, char **argv) {
    int nthreads = tbb::task_scheduler_init::default_num_threads();
    printf("Using %d TBB threads\n", nthreads);

    tbb::task_group g;
    int incr = 0;
    int nlaunched = 0;
    incr = incr + 0;

    const unsigned long long spawn_start_time = current_time_ns();
    do {
        g.run([=] {
            int incr = 0;
            incr = incr + 1;
        });

        nlaunched++;
    } while (nlaunched < NTASKS);
    const unsigned long long spawn_end_time = current_time_ns();
    printf("METRIC task_create %d %.20f\n", NTASKS,
            (double)NTASKS / ((double)(spawn_end_time -
                    spawn_start_time) / 1000.0));
    g.wait();

    nlaunched = 0;
    const unsigned long long schedule_start_time = current_time_ns();
    do {
        g.run([=] {
            int incr = 0;
            incr = incr + 1;
        });

        nlaunched++;
    } while (nlaunched < NTASKS);
    g.wait();
    const unsigned long long schedule_end_time = current_time_ns();
    printf("METRIC task_run %d %.20f\n", NTASKS,
            (double)NTASKS / ((double)(schedule_end_time -
                    schedule_start_time) / 1000.0));

    return 0;
}
