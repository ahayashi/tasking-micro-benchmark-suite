#include "hclib.h"
#include "timing.h"

#include <stdio.h>
#include "fan_out.h"

void empty_task(void *arg) {
    /*
     * Unfortunately need to put something here to compare against OpenMP tasks,
     * otherwise some OpenMP compilers will make the task a no-op.
     */
    int incr = 0;
    incr = incr + 1;
}

void entrypoint(void *arg) {
    int nworkers = hclib_get_num_workers();

    printf("Using %d HClib workers\n", nworkers);

    const unsigned long long start_time = current_time_ns();

    hclib_promise_t prom;
    hclib_promise_init(&prom);

    hclib_future_t *fut = hclib_get_future_for_promise(&prom);

    hclib_start_finish();
    {
        int i;
        for (i = 0; i < FAN_OUT; i++) {
            hclib_async(empty_task, NULL, &fut, 1, NULL);
        }

        hclib_promise_put(&prom, NULL);
    }
    hclib_end_finish();
    const unsigned long long end_time = current_time_ns();
    printf("METRIC fan_out %d %.20f\n", FAN_OUT,
            (double)FAN_OUT / ((double)(end_time - start_time) / 1000.0));
}

int main(int argc, char **argv) {
    hclib_launch(entrypoint, NULL, NULL, 0);
    return 0;
}
