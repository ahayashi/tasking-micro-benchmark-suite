#include "qthread.h"
#include "timing.h"

#include <stdio.h>
#include <assert.h>
#include "fan_out_and_in.h"

#define MAX_NUM_WAITS 4

aligned_t empty_task(void *arg) {
    /*
     * Unfortunately need to put something here to compare against OpenMP tasks,
     * otherwise some OpenMP compilers will make the task a no-op.
     */
    int incr = 0;
    incr = incr + 1;
    return incr;
}

void entrypoint(void *arg) {
    int status = qthread_initialize();
    assert(status == QTHREAD_SUCCESS);
    int nworkers = qthread_num_workers();

    printf("Using %d Qthread workers\n", nworkers);

    aligned_t cond = 0;
    qthread_empty(&cond);    
    aligned_t futures[FAN_OUT_AND_IN];

    const unsigned long long start_time = current_time_ns();
    qthread_fill(&cond);    
    int i;
    for (i = 0; i < FAN_OUT_AND_IN; i++) {
	qthread_fork_precond(empty_task, NULL, &futures[i], 1, &cond);
    }

    int nfutures = FAN_OUT_AND_IN;
    while (nfutures > 1) {
        int next_nfutures = 0;

        for (i = 0; i < nfutures; i += MAX_NUM_WAITS) {
            int this_n_futures = nfutures - i;
            if (this_n_futures > MAX_NUM_WAITS) this_n_futures = MAX_NUM_WAITS;
	    int j;
	    for (j = 0; j < this_n_futures; j++) {
		qthread_readFF(NULL, &futures[i+j]);
	    }
	    qthread_fork(empty_task, NULL, &futures[next_nfutures]);
	    
            next_nfutures++;
        }
        nfutures = next_nfutures;
    }

    qthread_readFF(NULL, &futures[0]);
    const unsigned long long end_time = current_time_ns();
    printf("METRIC fan_out_and_in %d %.20f\n", FAN_OUT_AND_IN,
            (double)FAN_OUT_AND_IN / ((double)(end_time - start_time) /
                1000.0));
}

int main(int argc, char **argv) {
    entrypoint(NULL);
    return 0;
}
