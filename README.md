# libtracer: trace-level debug logging framework

libtracer is a library that makes it easy to include trace-level debug logs in
production systems by storing a configurable number of recent entries in an
on-disk ring buffer. The ring buffer size can be limited by entry count or by
an amount of time.

This means that you can trace freely without worrying about cluttering your
main logs or using valuable disk space. If an unexpected crash happens, you
instantly have all the `printf` debugging you probably would have done readily
available. If things run smoothly and nothing crashes, then you've only
sacrificed an easily configurable amount of disk space.

If you only want to include tracing in certain configurations, you can simply
skip the call to `tracer_start` and all invocations of the `tracer` macro will
become no-ops, minus the cost of invoking a function.

Tracer is thread-safe as well. The act of committing an entry to disk is
entirely atomic and completed in-thread so that entries are safely in the
operating system's write buffers before returning. Since large parts of
`tracer` invocations are serialized, however, excessive use of it in highly
concurrent code can cause lock contention that slows your code down
significantly.

## API

```c
void tracer_config_default(tracer_config_t*);
void tracer_config_set_path(tracer_config_t*, const char *path);
void tracer_config_set_max_entries(tracer_config_t*, long max_entries);
void tracer_config_set_max_seconds(tracer_config_t*, long max_seconds);

void tracer_start(tracer_t*, tracer_config_t*);

#define tracer(T, FORMAT...)
```

### Example usage

```c
#include <tracer.h>

tracer_config_t my_tracer_config;
tracer_t my_tracer;

...

int main(int argc, char *argv[]) {
        tracer_config_default(&my_tracer_config);
        tracer_config_set_path(&my_tracer_config, "tracer.log");
        tracer_config_set_max_seconds(&my_tracer_config, 60);

        tracer_start(&my_tracer, &my_tracer_config);

        tracer(&my_tracer, "opened tracer with %d seconds", 60);

        ...

        tracer(&my_tracer, "iterating over %d values", count);
        for (i=0; i<count; i++) {
                tracer(&my_tracer, "iteration=%d", i);
                tracer(&my_tracer, "  value=%p", values[i]);
                tracer(&my_tracer, "    name=%s", values[i]->name);

                ...

                tracer(&my_tracer, "done, next iteration");
        }
        tracer(&my_tracer, "processing finished");
}
```

## The `tracer` tool

`tracer` is the command line tool for interacting with libtracer output files.
Its usage is perhaps best captured in the help output:

```text
usage: tracer dump FILE
```
