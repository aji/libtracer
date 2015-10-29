/* tracer.c -- main libtracer source file
   Copyright (C) 2015 Alex Iadicicco

   This file is part of libtracer and is protected under the terms contained in
   the COPYING file in the project root */

#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <tracer.h>

#define TRACER_RECORD_SIZE (128)
#define TRACER_DATA_SIZE (TRACER_RECORD_SIZE - 8)

typedef struct tracer_record tracer_record_t;

struct tracer_record {
	uint32_t secs;
	uint32_t usecs;
	char data[TRACER_DATA_SIZE];
};

void tracer_config_default(tracer_config_t *cfg) {
	cfg->path = "tracer.log";
	cfg->max_entries = TRACER_DEFAULT_MAX_ENTRIES;
}

void tracer_config_set_path(tracer_config_t *cfg, const char *path) {
	cfg->path = path;
}

void tracer_config_set_max_entries(tracer_config_t *cfg, long ents) {
	cfg->max_entries = ents;
}

void tracer_start(tracer_t *tr, tracer_config_t *cfg) {
	assert(sizeof(tracer_record_t) == TRACER_RECORD_SIZE);

	if (!tr) {
		return;
	}

	tr->cfg = cfg;

	/* TODO: check this */
	pthread_mutex_init(&tr->ring_mutex, NULL);

	/* TODO: check this */
	tr->fd = open(cfg->path, O_WRONLY | O_CREAT, 0600);

	tr->count = 0;
	tr->head = 0;
}

static void tracer_add(tracer_t *tr, tracer_record_t *rec) {
	uint32_t alloc;

	/* TODO: check this */
	pthread_mutex_lock(&tr->ring_mutex);

	if (tr->count < tr->cfg->max_entries) {
		alloc = tr->count * TRACER_RECORD_SIZE;
		tr->count ++;
	} else {
		alloc = tr->head * TRACER_RECORD_SIZE;
		tr->head ++;
		if (tr->head >= tr->cfg->max_entries) {
			tr->head -= tr->cfg->max_entries;
		}
	}

	/* TODO: check this */
	pthread_mutex_unlock(&tr->ring_mutex);

	/* TODO: check this */
	pwrite(tr->fd, rec, TRACER_RECORD_SIZE, alloc);
}

void _tracer_fmt(tracer_t *tr, const char *file, int line, const char *fmt, ...) {
	tracer_record_t rec;
	va_list va;
	struct timeval tv;

	if (!tr || !tr->cfg) {
		return;
	}

	/* TODO: check this */
	gettimeofday(&tv, NULL);
	rec.secs = tv.tv_sec;
	rec.usecs = tv.tv_usec;

	va_start(va, fmt);
	vsnprintf(rec.data, TRACER_DATA_SIZE, fmt, va);
	va_end(va);

	tracer_add(tr, &rec);
}
