#include <stdio.h>
#include <tracer.h>

#define COUNT 1000

tracer_config_t my_tracer_config;
tracer_t my_tracer;

#define my_trace(FMT...) tracer(&my_tracer, FMT)

int values[COUNT];

int main(int argc, char *argv[]) {
	int i, count, sum;

	tracer_config_default(&my_tracer_config);
	tracer_config_set_path(&my_tracer_config, "tracer.log");
	tracer_config_set_max_entries(&my_tracer_config, 1000);

	tracer_start(&my_tracer, &my_tracer_config);

	my_trace("opened tracer with %d entries", 1000);

	count = COUNT;

	for (i=0; i<count; i++) {
		values[i] = 3 * i;
	}

	my_trace("summing %d values", count);
	sum = 0;
	for (i=0; i<count; i++) {
		my_trace("iteration=%d, value=%d", i, values[i]);
		sum += values[i];
		my_trace("  current sum=%d", sum);
		if (i == 666) {
			/* OH NO! */
			*(char*)NULL = i;
		}
	}
	my_trace("sum finished: %d", sum);

	printf("sum is %d\n", sum);
}
