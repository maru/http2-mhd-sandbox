/*
     This file is part of libmicrohttpd
     Copyright (C) 2007 Christian Grothoff (and other contributing authors)

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
/**
 * @file http2_minimal_example.c
 * @brief minimal example for how to use libmicrohttpd with HTTP/2
 * @author Christian Grothoff
 * @author Maru Berezin
 */

#include "platform.h"
#include <microhttpd.h>
#include <nghttp2/nghttp2.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title></head><body>libmicrohttpd demo</body></html>\n"

static int
ahc_echo (void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data, size_t *upload_data_size, void **ptr)
{
  static int aptr;
  const char *me = cls;
  struct MHD_Response *response;
  int ret;
  (void)url;               /* Unused. Silent compiler warning. */
  (void)version;           /* Unused. Silent compiler warning. */
  (void)upload_data;       /* Unused. Silent compiler warning. */
  (void)upload_data_size;  /* Unused. Silent compiler warning. */

  // if (0 != strcmp (method, "GET"))
  //   return MHD_NO;              /* unexpected method */
  if (&aptr != *ptr)
    {
      /* do never respond on first call */
      *ptr = &aptr;
      return MHD_YES;
    }
  *ptr = NULL;                  /* reset when done */
  response = MHD_create_response_from_buffer (strlen (me),
					      (void *) me,
					      MHD_RESPMEM_PERSISTENT);
  MHD_add_response_header(response, "header-test", "my value");
  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);
  return ret;
}

int
main (int argc, char *const *argv)
{
  struct MHD_Daemon *d;

  if (argc != 2)
    {
      printf ("%s PORT\n", argv[0]);
      return 1;
    }

  /* Default HTTP2 server settings */
  nghttp2_settings_entry settings[3];
  int slen = 0;
  settings[slen].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  settings[slen].value = 100;
  ++slen;

  d = MHD_start_daemon (/* MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG, */
                        MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG | MHD_USE_HTTP2,
                        /* MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG | MHD_USE_POLL, */
      /* MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG | MHD_USE_POLL, */
      /* MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG, */
                      atoi (argv[1]),
                      NULL, NULL, &ahc_echo, PAGE,
			MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 10,
			MHD_OPTION_STRICT_FOR_CLIENT, (int) 1,
			MHD_OPTION_H2_SETTINGS, slen, settings,
			MHD_OPTION_END);
  if (d == NULL)
    return 1;
  (void) getc (stdin);
  MHD_stop_daemon (d);
  return 0;
}
