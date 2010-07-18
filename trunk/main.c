/*
  Copyright (c) 2007-2010 Miguel Mendez <mmendez@energyhq.be>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer. 
  * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution. 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  $Id$

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#include "cdpc.h"

#include "cdp.h"
#include "output_plain.h"
#include "output_csv.h"

#if defined(__sun__)
#include "dlpi_getmac.h"
#endif

void usage(void);
void showversion(void);
void handler(int sig);

extern int got_packet;
int verbosity;
int itimer;

int
main(int argc, char **argv)
{
  char cdevice[FILENAME_MAX + 1];
  char outfile[FILENAME_MAX + 1];

  int result;
  int wantfile;
  int ch;
  int options;
  int timeout;

  PORTINFO *my_port;

  struct itimerval rttimer;
  struct itimerval old_rttimer;

  options = OUTPUT_PLAIN;
  wantfile = 0;
  verbosity = 0;
  itimer = 1;
  got_packet = 0;

  memset(cdevice, 0, FILENAME_MAX + 1);
  memset(outfile, 0, FILENAME_MAX + 1);

  my_port = malloc(sizeof(PORTINFO));
  memset(my_port, 0, sizeof(PORTINFO));

  // Set default timeout to 120 seconds.
  timeout = 120;

  while ((ch = getopt(argc, argv, "hVvcpnt:d:o:")) != -1) {

    switch (ch) {

    case 'd':
      strncpy(cdevice, optarg, FILENAME_MAX);
      break;

    case 'o':
      wantfile = 1;
      strncpy(outfile, optarg, FILENAME_MAX);
      break;

    case 'c':
      options = OUTPUT_CSV;
      break;

    case 'p':
      options = OUTPUT_PLAIN;
      break;

    case 'v':
      showversion();
      exit(EXIT_SUCCESS);

    case 'V':
      verbosity = 1;
      break;

    case 'n':
      itimer = 0;
      break;

    case 't':
      timeout = atoi(optarg);
      break;

    case 'h':
    case '?':
    default:
      usage();

    }

  }

  argc -= optind;
  argv += optind;


  if ( strlen(cdevice) < 1 ) {

    usage();

  }

  if ( wantfile == 1 && strlen(outfile) < 1 ) {

    usage();

  }

  if (verbosity == 1) { 

    printf("\n");
    showversion();
    printf("\n");

  }

  result = gethostname(my_port->hostname, HOST_NAME_MAX + 1);

  if (result == -1) {

    perror("gethostname() : ");
    exit(-1);

  }

#if defined(__sun__)

  result = get_mac_dlpi(my_port->addr, cdevice);

  if (result == -1) {

    fprintf(stderr, "Cannot get MAC address for %s", cdevice);

  }

#endif

  snprintf(my_port->iface, 254, "%s", cdevice); 

  if (itimer == 1 ) {

    signal(SIGALRM, handler);
    rttimer.it_value.tv_sec     = timeout;
    rttimer.it_value.tv_usec    = 0;  
    rttimer.it_interval.tv_sec  = timeout;
    rttimer.it_interval.tv_usec = 0; 

    setitimer (ITIMER_REAL, &rttimer, &old_rttimer);

    // 1 second intervals
    timeout = 1000;

  } else {

    timeout *= 1000;

  }

  result = get_cdp_info(cdevice, timeout, my_port);
  if(result == -1 ) {

    exit(-1);

  }

  if (options == OUTPUT_PLAIN) {

    if (wantfile == 1) {

      result = output_plain(my_port, outfile);

    } else {

      result = output_plain(my_port, NULL);

    }

    if (result == -1) {

      exit(-1);

    }
 
  } else if (options == OUTPUT_CSV) {

    if (wantfile == 1) {

      result = output_csv(my_port, outfile);

    } else {

      result = output_csv(my_port, NULL);

    }

    if (result == -1) {

      exit(-1);

    }

  }

  return 0;

}

void
usage(void)
{

  printf("usage: cdpc [-h] [-v] [-V] [-c] [-p] [-d nic] [-t time] [-s] [-o file]\n");
  printf("\t-h\tshow this info.\n");
  printf("\t-v\tshow version info.\n");
  printf("\t-V\tenable verbose mode.\n");
  printf("\t-c\toutput information in csv mode.\n");
  printf("\t-p\toutput information in plain text.\n");
  printf("\t-d nic\tcapture packets from this network interface.\n");
  printf("\t-t time\ttimeout in seconds. Defaults to 75.\n");
  printf("\t-n\tuse libpcap's internal timer instead of a setitimer(2) handler.\n");
  printf("\t-o file\toutput resulting data to file.\n");
  exit(EXIT_SUCCESS);

}

void
showversion(void)
{

  printf("cdpc %s\n", CDPC_VERSION);
  printf("A Free Cisco Discovery Protocol client.\n");
  printf("Copyright (c) 2007-2008, Miguel Mendez. All rights reserved.\n");

}

void 
handler(int sig)
{
  if (got_packet != 1) {

    fprintf(stderr, "Caught SIGALRM and no packet captured.\n");
    cdp_cleanup();
    exit(EXIT_SUCCESS);

  }

}
