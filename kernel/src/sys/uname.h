#pragma once

typedef struct __uname {
  char sysname[128];  /* Operating system name */
  char nodename[128]; /* Name within communications network
                      to which the node is attached, if any */
  char release[128];  /* Operating system release */
  char version[128];  /* Operating system version */
  char machine[128];  /* Hardware type identifier */
#ifdef _GNU_SOURCE
  char domainname[]; /* NIS or YP domain name */
#endif
} uname_t;