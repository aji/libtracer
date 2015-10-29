# libtracer: trace-level debug logging framework

libtracer is a library that makes it easy to include trace-level debug logs in
production systems by storing a configurable number of recent entries in an
on-disk ring buffer. The ring buffer size can be limited by entry count or by
an amount of time.

## API

```c
void tracer_config_default(tracer_config_t*);
void tracer_config_set_path(tracer_config_t*, const char *path);
void tracer_config_set_max_entries(tracer_config_t*, long max_entries);
void tracer_config_set_max_seconds(tracer_config_t*, long max_seconds);

void tracer_start(tracer_t*, tracer_config_t*);

#define tracer(T, FORMAT...)
```

## The `tracer` tool

`tracer` is the command line tool for interacting with libtracer output files.
Its usage is perhaps best captured in the help output:

```text
usage: tracer dump FILE
```
