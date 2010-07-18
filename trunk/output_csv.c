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

  $Id: output_csv.c 45 2008-01-07 16:41:37Z mmendez $

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "cdpc.h"
#include "output_csv.h"

int
output_csv(PORTINFO *my_port, char *outfile)
{
  FILE *fp;
  int i;
  u_char *mac;

  mac = my_port->addr;

  if (outfile != NULL) {

    fp = fopen(outfile, "w");

    if( fp == NULL) {

      perror("fopen");
      return(-1);

    }

  } else {

    fp = stdout;

  }

  // CSV format: hostname, iface, MAC, switch name, switch port, vlan
  fprintf(fp, "%s,%s,", 
	  my_port->hostname, my_port->iface);

#if defined(__sun__)

  for (i=0; i<6; i++) {
    fprintf(fp, "%2.2x", mac[i]);
  }

  fprintf(fp, ",");

#endif

  fprintf(fp, "%s,%s,%s\n",
          my_port->switchname, my_port->switchport,
          strlen(my_port->switchvlan) > 0 ? my_port->switchvlan : "0");

  if (outfile != NULL) {

    fclose(fp);

  } else {

    fflush(stdout);

  }

  return 0;

}
