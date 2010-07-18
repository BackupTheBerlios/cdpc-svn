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

  $Id: cdpc.h 47 2008-01-07 16:47:41Z mmendez $

*/

#ifndef CDPC_H
#define CPDC_H

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

typedef struct portinfo {

  char hostname[HOST_NAME_MAX + 1];
  char switchname[1024];
  char iface[1024];
  char addr[1024];
  char switchport[1024];
  char switchtype[1024];
  char *switchversion;
  char switchvlan[1024];
  char switchaddress[1024];
  unsigned int ip;
  unsigned int mask;

} PORTINFO;

#define CDPC_VERSION "0.2.0 \"Rule the Planet (Paul Oakenfold Remix)\""

#define OUTPUT_PLAIN 1
#define OUTPUT_CSV 2

#endif
