#include <machine/rtems-bsd-user-space.h>

/*-
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0
#ifndef lint
static char const copyright[] =
"@(#) Copyright (c) 1988, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)hostname.c	8.1 (Berkeley) 5/31/93";
#endif /* not lint */
#endif
#ifdef __rtems__
#define __need_getopt_newlib
#include <getopt.h>
#include <rtems/netcmds-config.h>
#include <machine/rtems-bsd-program.h>
#include <machine/rtems-bsd-commands.h>
#endif /* __rtems__ */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <rtems/bsd/sys/param.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(void);

#ifdef __rtems__
static int main(int argc, char *argv[]);

static int hostname_command(int argc, char *argv[])
{
	int exit_code;

	rtems_bsd_program_lock();

	exit_code = rtems_bsd_program_call_main("hostname", main, argc, argv);

	rtems_bsd_program_unlock();

	return exit_code;
}

rtems_shell_cmd_t rtems_shell_HOSTNAME_Command = {
  .name = "hostname",
  .usage = "hostname [-fs] [name-of-host]",
  .topic = "net",
  .command = hostname_command
};
#endif /* __rtems__ */
int
main(int argc, char *argv[])
{
	int ch, sflag;
	char *p, hostname[MAXHOSTNAMELEN];
#ifdef __rtems__
	struct getopt_data getopt_data;
	memset(&getopt_data, 0, sizeof(getopt_data));
#define optind getopt_data.optind
#define getopt(argc, argv, opt) getopt_r(argc, argv, "+" opt, &getopt_data)
#endif /* __rtems__ */

	sflag = 0;
	while ((ch = getopt(argc, argv, "fs")) != -1)
		switch (ch) {
		case 'f':
			/*
			 * On Linux, "hostname -f" prints FQDN.
			 * BSD "hostname" always prints FQDN by
			 * default, so we accept but ignore -f.
			 */
			break;
		case 's':
			sflag = 1;
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc > 1)
		usage();

	if (*argv) {
		if (sethostname(*argv, (int)strlen(*argv)))
			err(1, "sethostname");
	} else {
		if (gethostname(hostname, (int)sizeof(hostname)))
			err(1, "gethostname");
		if (sflag) {
			p = strchr(hostname, '.');
			if (p != NULL)
				*p = '\0';
		}
		(void)printf("%s\n", hostname);
	}
	exit(0);
}

static void
usage(void)
{

	(void)fprintf(stderr, "usage: hostname [-fs] [name-of-host]\n");
	exit(1);
}
