/* tracer.h -- libtracer header file
   Copyright (C) 2015 Alex Iadicicco

   This file is part of libtracer and is protected under the terms contained in
   the COPYING file in the project root */

#ifndef __INC_TRACER_H__
#define __INC_TRACER_H__

#include <pthread.h>
#include <stdint.h>

typedef struct tracer_config tracer_config_t;
typedef struct tracer tracer_t;

extern void tracer_config_default(tracer_config_t*);
extern void tracer_config_set_path(tracer_config_t*, const char *path);
extern void tracer_config_set_max_entries(tracer_config_t*, long);

extern void tracer_start(tracer_t*, tracer_config_t*);

#define tracer(T, FMT...) \
	_tracer_fmt((T), __FILE__, __LINE__, FMT)

extern void _tracer_fmt(tracer_t*, const char*, int, const char*, ...);

#define TRACER_DEFAULT_MAX_ENTRIES 8192

struct tracer_config {
	const char *path;
	long max_entries;
};

struct tracer {
	tracer_config_t *cfg;

	pthread_mutex_t ring_mutex;

	int fd;

	uint32_t count;
	uint32_t head;
};

#endif
