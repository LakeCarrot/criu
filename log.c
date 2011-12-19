#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <fcntl.h>

#include "compiler.h"
#include "types.h"
#include "util.h"

/*
 * Note pr_ helpers rely on this
 * descriptor!
 */
static int logfd = STDERR_FILENO;

int get_logfd(void)
{
	return logfd;
}

int init_log(const char *name)
{
	struct rlimit rlimit;
	int fd = STDERR_FILENO;

	if (name) {
		fd = open(name, O_CREAT | O_WRONLY);
		if (fd == -1) {
			pr_perror("Can't create log file %s\n", name);
			return -1;
		}
	}

	if (getrlimit(RLIMIT_NOFILE, &rlimit)) {
		pr_err("can't get rlimit: %m\n");
		return -1;
	}

	logfd = rlimit.rlim_cur - 1;
	if (dup2(fd, logfd) < 0) {
		pr_err("can't duplicate descriptor %d->%d: %m\n",
			fd, logfd);
		logfd = STDERR_FILENO;
		return -1;
	}

	return 0;
}

void fini_log(void)
{
	if (logfd != STDERR_FILENO &&
	    logfd != STDIN_FILENO &&
	    logfd != STDERR_FILENO)
		close(logfd);

	logfd = STDERR_FILENO;
}
