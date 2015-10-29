/* tracer.c -- command line utility
   Copyright (C) 2015 Alex Iadicicco

   This file is part of libtracer and is protected under the terms contained in
   the COPYING file in the project root */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

struct context {
	int fd;
	uint32_t count;
	uint32_t head;
};

#define TRACER_RECORD_SIZE (128)
#define TRACER_DATA_SIZE (TRACER_RECORD_SIZE - 8)

struct tracer_record {
	uint32_t secs;
	uint32_t usecs;
	char data[TRACER_DATA_SIZE];
};

static void open_log(struct context *ctx, const char *path) {
	int i;
	off_t size;
	struct timeval min, cur;
	struct tracer_record rec;

	ctx->fd = open(path, O_RDONLY);

	size = lseek(ctx->fd, 0, SEEK_END);
	lseek(ctx->fd, 0, SEEK_SET);

	ctx->count = size / TRACER_RECORD_SIZE;

	min.tv_sec = 0;
	min.tv_usec = 0;
	for (i=0; i<ctx->count; i++) {
		pread(ctx->fd, &rec, TRACER_RECORD_SIZE, i * TRACER_RECORD_SIZE);
		cur.tv_sec = rec.secs;
		cur.tv_usec = rec.usecs;

		if (i == 0 || timercmp(&cur, &min, <)) {
			ctx->head = i;
			min.tv_sec = cur.tv_sec;
			min.tv_usec = cur.tv_usec;
		}
	}
}

static void print_record(struct context *ctx, uint32_t offs) {
	struct tracer_record rec;

	pread(ctx->fd, &rec, TRACER_RECORD_SIZE, offs * TRACER_RECORD_SIZE);
	printf("[%d.%03d] %s\n", rec.secs, rec.usecs/1000, rec.data);
}

int main(int argc, char *argv[]) {
	struct context ctx;
	int i;

	open_log(&ctx, argv[1]);

	for (i=0; i<ctx.count; i++) {
		print_record(&ctx, (ctx.head + i) % ctx.count);
	}
}
