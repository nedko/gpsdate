/* -*- Mode: C ; c-basic-offset: 2 -*- */
/* Copyright (c) 2014 Nedko Arnaudov
 * See the file COPYING for copying permission. */

#include <gps.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>

int main(int argc, char ** argv)
{
  int ret;
  struct gps_data_t gpsdata;
  struct timeval tv;
  int verbose;
  const char * host;
  const char * port;

  verbose = 0;
  host = "localhost";
  port = "2947";

  while (argc > 1 && argv[1][0] == '-')
  {
    if (strcmp(argv[1], "-v") == 0)
    {
      verbose = 1;
    }
    else if (strcmp(argv[1], "-h") == 0 ||
             strcmp(argv[1], "--help") == 0)
    {
      ret = 0;
    help:
      printf("Usage: gpsdate [-v] [host [port]]\n");
      printf("  -v - print the GPS time\n");
      printf("  server - gpsd host\n");
      printf("  port - gpsd port\n");
      goto exit;
    }
    else
    {
      ret = 1;
      goto help;
    }

    argc--;
    argv++;
  }

  if (argc > 1)
  {
    host = argv[1];
    argc--;
    argv++;
  }

  if (argc > 1)
  {
    port = argv[1];
    argc--;
    argv++;
  }

  ret = 1;

  if (argc > 1)
  {
    ret = 1;
    goto help;
  }

  if (gps_open(host, port, &gpsdata) != 0)
  {
    perror("cannot connect to gpsd");
    goto exit;
  }

  if (gps_stream(&gpsdata, WATCH_ENABLE | WATCH_JSON, NULL) != 0)
  {
    perror("cannot watch gpsd");
    goto close;
  }

  while (gps_waiting(&gpsdata, 5000000))
  {
    errno = 0;
    if (gps_read(&gpsdata) == -1)
    {
      perror("cannot read data from gpsd");
      goto unwatch;
    }

    if (isnan(gpsdata.fix.time) == 0)
    {
      tv.tv_sec = (int)round(gpsdata.fix.time);
      tv.tv_usec = 0;

      if (verbose)
      {
        printf("GPS time is: %s", ctime(&tv.tv_sec));
      }

      if (settimeofday(&tv, NULL) != 0)
      {
        perror("cannot set system time");
        goto unwatch;
      }

      ret = 0;
      goto unwatch;
    }
  }

  fprintf(stderr, "gpsd timeout\n");
  ret = 1;

unwatch:
  gps_stream(&gpsdata, WATCH_DISABLE, NULL);
close:
  gps_close(&gpsdata);
exit:
  return ret;
}
