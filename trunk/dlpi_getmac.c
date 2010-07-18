/*
  Copyright (c) 2008-2010 Miguel Mendez <mmendez@energyhq.be>

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
#include <stropts.h>
#include <ctype.h>
#include <unistd.h>

#include <sys/types.h>
#include <fcntl.h>

#include <sys/dlpi.h>

#include "dlpi_getmac.h"

#define AREA_SZ 4096
#define INSAP 22

int 
get_mac_dlpi(u_char *addr, char *device)
{
  int i, j, k;
  int ppa;
  int fd;
  int flags;

  u_long ctl_area[AREA_SZ];
  u_long dat_area[AREA_SZ];
  struct strbuf ctl = {AREA_SZ, 0, (char *)ctl_area};
  struct strbuf dat = {AREA_SZ, 0, (char *)dat_area};

  dl_attach_req_t *attach_req = (dl_attach_req_t *)ctl_area;
  dl_error_ack_t *err_ack = (dl_error_ack_t *)ctl_area;
  dl_bind_req_t *bind_req = (dl_bind_req_t *)ctl_area;
  dl_bind_ack_t *bind_ack = (dl_bind_ack_t *)ctl_area;

  char instance[4];
  char dev_tmp[16];
  char my_device[32];

  memset(instance, 0, 4);
  memset(dev_tmp, 0, 16);

  j = 0;
  k = 0;

  for (i = 0; i < strlen(device); i++) {

    if (isdigit(device[i])) {

      instance[j] = device[i];
      j++;

    } else {

      dev_tmp[k] = device[i];
      k++;

    }

  }

  snprintf(my_device, sizeof(my_device), "/dev/%s", dev_tmp);
  ppa = atoi(instance);

  if ((fd = open(my_device, O_RDWR)) == -1) {

    fprintf(stderr, "Error: Cannot open %s for read/write.\n", my_device);
    return -1;

  }

  attach_req->dl_primitive = DL_ATTACH_REQ;
  attach_req->dl_ppa = ppa;

  ctl.len = sizeof(dl_attach_req_t);

  if (putmsg(fd, &ctl, 0, 0) < 0) {

    perror("putmsg: ");
    return -1;

  }

  flags = 0;

  ctl_area[0] = 0;
  dat_area[0] = 0;

  if (getmsg(fd, &ctl, &dat, &flags) < 0 ) {

    perror("getmsg: ");
    return -1;

  }

  if (err_ack->dl_primitive != DL_OK_ACK) {

    fprintf(stderr, "Streams error.\n");
    return -1;

  }  

  bind_req->dl_primitive = DL_BIND_REQ;
  bind_req->dl_sap = INSAP;
  bind_req->dl_max_conind = 1;
  bind_req->dl_service_mode = DL_CLDLS;
  bind_req->dl_conn_mgmt = 0;
  bind_req->dl_xidtest_flg = 0;

  ctl.len = sizeof(dl_bind_req_t);

  if (putmsg(fd, &ctl, 0, 0) < 0) {

    perror("putmsg: ");
    return -1;

  }

  flags = 0;

  ctl_area[0] = 0;
  dat_area[0] = 0;

  if (getmsg(fd, &ctl, &dat, &flags) < 0 ) {

    perror("getmsg: ");
    return -1;

  }

  if (err_ack->dl_primitive != DL_BIND_ACK) {

    fprintf(stderr, "Streams error.\n");
    return -1;

  }

  memcpy(addr, (u_char *)bind_ack + bind_ack->dl_addr_offset, 
	 bind_ack->dl_addr_length);

  close(fd);


  return 0;

}
