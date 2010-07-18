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
#include <arpa/inet.h>

#include <pcap.h>

#include "cdpc.h"
#include "cdp.h"

#define ETHER_ADD_LEN 6
#define SIZE_ETHER 14
#define SIZE_CDP 4
#define SIZE_LLCSNAP 8
#define SIZE_CDP_DATA 4

#define CDP_DEVICE_ID 0x0001
#define CDP_ADDRESS 0x0002
#define CDP_PORT_ID 0x0003
#define CDP_CAPAB 0x0004
#define CDP_VERSION 0x0005
#define CDP_PLATFORM 0x0006
#define CDP_HELLO 0x0008
#define CDP_VLAN 0x000A

struct ether_header {

  u_char ether_dest[ETHER_ADD_LEN];
  u_char ether_src[ETHER_ADD_LEN];
  u_short ether_type;
  
};

struct cdp_header {

  u_char version;
  u_char ttl;
  u_short checksum;

};

struct cdp_data {

  u_short type;
  u_short length;

};

void process_packet(u_char *, const struct pcap_pkthdr *,
		    const u_char *);


extern int verbosity;
extern int itimer;

pcap_t *handle;
int got_packet;

int
get_cdp_info(char *device, int timeout, PORTINFO *my_port)
{
  char errbuf[PCAP_ERRBUF_SIZE];
  bpf_u_int32 mask;
  bpf_u_int32 net;
  struct bpf_program filter;
  struct pcap_pkthdr header;
  const u_char *packet;
  char filter_exp[] = "ether host 01:00:0c:cc:cc:cc and ether[20:2] = 0x2000";
  int result;

  result = pcap_lookupnet(device, &net, &mask, errbuf);
  if (result == -1 ) {

    fprintf(stderr, "Can't get netmask for device %s\n", device);
    net = 0;
    mask = 0;

  }

  handle = pcap_open_live(device, BUFSIZ, 1, timeout, errbuf);
  if (handle == NULL) {

    fprintf(stderr, "Can't open device %s: %s\n", device, errbuf);
    return(-1);

  }

  result = pcap_compile(handle, &filter, filter_exp, 0, net);
  if (result == -1 ) {

    fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return(-1);

  }

  result = pcap_setfilter(handle, &filter);
  if (result == -1 ) {

    fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return(-1);

  }

  packet = NULL;

  if (itimer == 1 ) {

    while ((got_packet != 1) && (packet == NULL)) {

      if (verbosity == 1) {

	printf("Waiting for packet...\n");

      }

      packet = pcap_next(handle, &header);
      pcap_loop(handle, 1, process_packet, (u_char *) my_port);
      
    }

    cdp_cleanup();
    return (0);

  } else {

    packet = pcap_next(handle, &header);

  }

  got_packet = 1;

  pcap_close(handle);

  if (packet == NULL) {

    fprintf(stderr, "No packet was captured.\n");
    return(-1);
  }

  process_packet((u_char *) my_port, &header, packet);

  return(0);

}

void
process_packet(u_char *port, const struct pcap_pkthdr *header,
	       const u_char *packet)

{
  int delta;
  int payload_size;
  int result;
  int length, type;
  u_short vlan;
  const struct ether_header *my_ether_header;
  const struct cdp_header *my_cdp_header;
  struct cdp_data *my_cdp_data;
  const u_char *payload;
  PORTINFO *my_port;

  got_packet = 1;

  my_port = (PORTINFO *) port;

  my_cdp_data = malloc(sizeof(my_cdp_data));
  memset(my_cdp_data, 0, sizeof(my_cdp_data));

  my_ether_header = (struct ether_header *) (packet);
  my_cdp_header = (struct cdp_header *) (packet + SIZE_ETHER + SIZE_LLCSNAP);

  payload = (u_char *) (packet + SIZE_ETHER + SIZE_CDP + SIZE_LLCSNAP);

  payload_size = header->len - ( SIZE_ETHER + SIZE_CDP + SIZE_LLCSNAP);

  if (verbosity == 1) {

    printf("Packet length: %i bytes\n", payload_size);
    result = (int) my_cdp_header->version;
    printf("CDP version: %i\n", result);
    result = (int) my_cdp_header->ttl;
    printf("TTL: %i seconds\n", result);
    printf("\n");

  }

  delta = 0;
  while (delta <= payload_size) {

    memcpy(my_cdp_data, (struct cdp_data *) (payload + delta), sizeof(my_cdp_data));
 
    delta += SIZE_CDP_DATA;

    type = ntohs(my_cdp_data->type);
    length = ntohs(my_cdp_data->length);

    if (length > SIZE_CDP_DATA) {

      length -= SIZE_CDP_DATA;

      switch(type) {

      case CDP_DEVICE_ID:
	memcpy(my_port->switchname, payload + delta, length);
	delta += length;
	break;

      case CDP_ADDRESS:
	memcpy(my_port->switchaddress, payload + delta, length);
	delta += length;
	break;

      case CDP_PORT_ID:
	memcpy(my_port->switchport, payload + delta, length);
	delta += length;
	break;

      case CDP_CAPAB:
	delta += length;
	break;

      case CDP_HELLO:
	delta += length;
	break;

      case CDP_PLATFORM:
	memcpy(my_port->switchtype, payload + delta, length);
	delta += length;
	break;

      case CDP_VERSION:
	my_port->switchversion = malloc(length + 1);
	memset(my_port->switchversion, 0, length + 1);
	memcpy(my_port->switchversion, payload + delta, length);
	delta += length;
	break;

      case CDP_VLAN:
	if (length == 2) {
	  memcpy(&vlan, payload + delta, length);
	  snprintf(my_port->switchvlan, 254, "%i", ntohs(vlan));
	} else {
	  snprintf(my_port->switchvlan, 254, "N/A");
	}
	delta += length;
	break;

      default:
	if (verbosity == 1) {

	  printf("Unkown type 0x%x with size %i found.\n", type, length);

	}

	delta += length;
	break;

      };

    }

  }

}

int 
cdp_cleanup(void)
{

  pcap_breakloop(handle);
  pcap_close(handle);
  return(0);

}
