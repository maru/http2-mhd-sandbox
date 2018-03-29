/*
     This file is part of libmicrohttpd
     Copyright (C) 2018 Christian Grothoff (and other contributing authors)

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
 * @file h2spec_server.c
 * @brief example server for h2spec tool
 * @author Maru Berezin
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <microhttpd.h>

/**
 * Index page.
 */
#define PAGE "<html><head><title>libmicrohttpd demo</title></head><body>libmicrohttpd demo</body></html>"

/**
 * Invalid method page.
 */
#define METHOD_ERROR "<html><head><title>Illegal request</title></head><body>Go away.</body></html>"


/**
 * Iterator over key-value pairs where the value
 * maybe made available in increments and/or may
 * not be zero-terminated.  Used for processing
 * POST data.
 *
 * @param cls user-specified closure
 * @param kind type of the value
 * @param key 0-terminated key for the value
 * @param filename name of the uploaded file, NULL if not known
 * @param content_type mime-type of the data, NULL if not known
 * @param transfer_encoding encoding of the data, NULL if not known
 * @param data pointer to size bytes of data at the
 *              specified offset
 * @param off offset of data in the overall value
 * @param size number of bytes in data available
 * @return MHD_YES to continue iterating,
 *         MHD_NO to abort the iteration
 */
static int
process_upload_data (void *cls,
	       enum MHD_ValueKind kind,
	       const char *key,
	       const char *filename,
	       const char *content_type,
	       const char *transfer_encoding,
	       const char *data, uint64_t off, size_t size)
{
  (void)cls;              /* Unused. Silent compiler warning. */
  (void)kind;              /* Unused. Silent compiler warning. */
  (void)key;              /* Unused. Silent compiler warning. */
  (void)filename;          /* Unused. Silent compiler warning. */
  (void)content_type;      /* Unused. Silent compiler warning. */
  (void)transfer_encoding; /* Unused. Silent compiler warning. */
  (void)data; /* Unused. Silent compiler warning. */
  (void)off; /* Unused. Silent compiler warning. */
  (void)size; /* Unused. Silent compiler warning. */

  return MHD_YES;
}


/**
 * Main MHD callback for handling requests.
 *
 * @param cls argument given together with the function
 *        pointer when the handler was registered with MHD
 * @param connection handle identifying the incoming connection
 * @param url the requested url
 * @param method the HTTP method used ("GET", "PUT", etc.)
 * @param version the HTTP version string (i.e. "HTTP/1.1")
 * @param upload_data the data being uploaded (excluding HEADERS,
 *        for a POST that fits into memory and that is encoded
 *        with a supported encoding, the POST data will NOT be
 *        given in upload_data and is instead available as
 *        part of MHD_get_connection_values; very large POST
 *        data *will* be made available incrementally in
 *        upload_data)
 * @param upload_data_size set initially to the size of the
 *        upload_data provided; the method must update this
 *        value to the number of bytes NOT processed;
 * @param ptr pointer that the callback can set to some
 *        address and that will be preserved by MHD for future
 *        calls for this request; since the access handler may
 *        be called many times (i.e., for a PUT/POST operation
 *        with plenty of upload data) this allows the application
 *        to easily associate some request-specific state.
 *        If necessary, this state can be cleaned up in the
 *        global "MHD_RequestCompleted" callback (which
 *        can be set with the MHD_OPTION_NOTIFY_COMPLETED).
 *        Initially, <tt>*con_cls</tt> will be NULL.
 * @return MHS_YES if the connection was handled successfully,
 *         MHS_NO if the socket must be closed due to a serios
 *         error while handling the request
 */
static int
create_response (void *cls,
		 struct MHD_Connection *connection,
		 const char *url,
		 const char *method,
		 const char *version,
		 const char *upload_data,
		 size_t *upload_data_size,
		 void **ptr)
{
  struct MHD_Response *response;
  struct MHD_PostProcessor *pp;
  int ret;
  (void)cls;(void)version;      /* Unused. Silent compiler warning. */
  const char* encoding;

  /* Need this header to create a post processor */
  encoding = MHD_lookup_connection_value (connection,
                                          MHD_HEADER_KIND,
                                          MHD_HTTP_HEADER_CONTENT_TYPE);
  if (NULL == encoding)
    {
      MHD_set_connection_value (connection, MHD_HEADER_KIND,
                                MHD_HTTP_HEADER_CONTENT_TYPE,
                                MHD_HTTP_POST_ENCODING_FORM_URLENCODED);
    }

  if (0 == strcmp (method, MHD_HTTP_METHOD_POST))
    {
      pp = *ptr;
      if (pp == NULL)
        {
          pp = MHD_create_post_processor (connection, 1024, &process_upload_data, NULL);
          *ptr = pp;
        }
      MHD_post_process (pp, upload_data, *upload_data_size);
      if (0 == *upload_data_size)
        {
          response = MHD_create_response_from_buffer (strlen (url),
                                                      (void *) url,
                                                      MHD_RESPMEM_MUST_COPY);
          ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
          MHD_destroy_response (response);
          MHD_destroy_post_processor (pp);
          *ptr = NULL;
          return ret;
        }
      *upload_data_size = 0;
      return MHD_YES;
    }

  if ( (0 == strcmp (method, MHD_HTTP_METHOD_GET)) ||
       (0 == strcmp (method, MHD_HTTP_METHOD_HEAD)) )
    {
      response = MHD_create_response_from_buffer (strlen (PAGE),
    					      (void *) PAGE,
    					      MHD_RESPMEM_PERSISTENT);
      ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
      MHD_destroy_response (response);
      return ret;
    }
  /* unsupported HTTP method */
  response = MHD_create_response_from_buffer (strlen (METHOD_ERROR),
					      (void *) METHOD_ERROR,
					      MHD_RESPMEM_PERSISTENT);
  ret = MHD_queue_response (connection,
			    MHD_HTTP_NOT_ACCEPTABLE,
			    response);
  MHD_destroy_response (response);
  return ret;
}


/**
 * Callback called upon completion of a request.
 *
 * @param cls not used
 * @param connection connection that completed
 * @param con_cls session handle
 * @param toe status code
 */
static void
request_completed_callback (void *cls,
			    struct MHD_Connection *connection,
			    void **con_cls,
			    enum MHD_RequestTerminationCode toe)
{
  struct MHD_PostProcessor *pp = *con_cls;
  (void)cls;(void)connection;(void)toe; /* Unused. Silent compiler warning. */

  if (NULL != pp)
    MHD_destroy_post_processor (pp);
  *con_cls = NULL;
}


/**
 * Call with the port number as the only argument.
 * Never terminates (other than by signals, such as CTRL-C).
 */
int
main (int argc, char *const *argv)
{
  struct MHD_Daemon *d;
  int use_http2 = 0;
  uint16_t port;

  switch (argc)
    {
    case 2:
      port = atoi (argv[1]);
      break;
    case 3:
      if (strcmp(argv[1], "-h2") == 0)
        {
          use_http2 = MHD_USE_HTTP2;
          port = atoi (argv[2]);
          break;
        }
    default:
      printf ("%s [-h2] PORT\n", argv[0]);
      return 1;
    }

  /* initialize PRNG */
  d = MHD_start_daemon (MHD_USE_ERROR_LOG | MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD | use_http2,
                        port,
                        NULL, NULL,
			&create_response, NULL,
			MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 15,
			MHD_OPTION_NOTIFY_COMPLETED, &request_completed_callback, NULL,
			MHD_OPTION_END);
  if (NULL == d)
    return 1;
  (void) getc (stdin);
  MHD_stop_daemon (d);
  return 0;
}
