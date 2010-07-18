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
#include <fcntl.h>

#include "cdpc.h"

#include "output_plain.h"

int
output_plain(PORTINFO *my_port, char *outfile)
{

  FILE *fp;
  u_char *mac;
  int i;

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

  fprintf(fp, "Hostname: %s\n", my_port->hostname);
  fprintf(fp, "Interface: %s\n", my_port->iface);

#if defined(__sun__)

  fprintf(fp, "MAC address: ");

  for (i=0; i<6; i++) {
    fprintf(fp, "%2.2x", mac[i]);
  }

  fprintf(fp, "\n");

#endif

  fprintf(fp, "Switch Name: %s\n", my_port->switchname);
  fprintf(fp, "Switch Type: %s\n", my_port->switchtype);
  if (my_port->switchversion != NULL) {
    fprintf(fp, "IOS version: %s\n", my_port->switchversion);
  }
  fprintf(fp, "Switch port: %s\n", my_port->switchport);
  fprintf(fp, "VLAN: %s\n", my_port->switchvlan);
  fprintf(fp, "\n");

  if (outfile != NULL) {

    fclose(fp);

  } else {

    fflush(stdout);

  }

  return 0;

}
