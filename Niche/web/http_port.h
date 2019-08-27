/*
 * Filename: http_port.h
 *
 * Copyright 2004 by InterNiche Technologies Inc. All rights reserved.
 *
 * NetPort PPP layer's per-port definitions. The portions of the PPP 
 * stack which may need to be modified for new ports are placed in 
 * this file.
 *
 * All these should be re-examined with each new port of the code.
 *
 * This file for:
 *   ALTERA Cyclone Dev board with the ALTERA Nios2 Core.
 *   SMSC91C11 10/100 Ethernet Controller
 *   GNU C Compiler provided by ALTERA Quartus Toolset.
 *   Quartus HAL BSP
 *   uCOS-II RTOS Rel 2.76 as distributed by Altera/NIOS2
 *
 * 06/21/2004
 *
 */

#ifndef _HTTP_PORT_H
#define _HTTP_PORT_H 1

#include "ipport.h"
#include "vfsfiles.h"

#define   HTTP_RX_BUF_SIZE        8192  /* Receive buffer size */
#define   HTTP_TX_BUF_SIZE        8192  /* Transmission buffer size */
#define   HTTP_URI_SIZE           8192  /* Max size of a URI *URL) string */
#define   HTTP_KEEP_ALIVE_COUNT   20    /* Max number of files per connection */
#define   HTTP_KEEP_ALIVE_TIME    5000  /* TCP connection keep-alive time (ms) */
#define   HTTP_PORT               80    /* TCP port number to listen on */
#define   HTTP_NUM_CONNECTIONS    6     /* Maximum concurrent HTTP connections */
#define   HTTP_MAX_LINE_SIZE      256   /* The maximum size of any line. */

#define   HTTP_VERSION_STRING     "HTTP/1.1 "
#define   HTTP_OK                 200
#define   HTTP_OK_STRING          "200 OK\r\n"
#define   HTTP_NO_CONTENT_STRING  "204 No Content\r\n"
#define   HTTP_NOT_FOUND          404
#define   HTTP_NOT_FOUND_STRING   "404 Not Found\r\n"
#define   HTTP_NOT_FOUND_FILE     "/not_found.html"
#define   HTTP_CONTENT_TYPE       "Content-Type: "
#define   HTTP_CONTENT_TYPE_HTML  "text/html\r\n"
#define   HTTP_CONTENT_TYPE_JPG   "image/jpg\r\n"
#define   HTTP_CONTENT_TYPE_GIF   "image/gif\r\n"
#define   HTTP_CONTENT_TYPE_PNG   "image/png\r\n"
#define   HTTP_CONTENT_TYPE_JS    "application/x-javascript\r\n"
#define   HTTP_CONTENT_TYPE_CSS   "text/css\r\n"
#define   HTTP_CONTENT_TYPE_SWF   "application/x-shockwave-flash\r\n"
#define   HTTP_CONTENT_TYPE_ICO   "image/vnd.microsoft.icon\r\n"
#define   HTTP_CONTENT_LENGTH     "Content-Length: "
#define   HTTP_KEEP_ALIVE         "Connection: Keep-Alive\r\n"
#define   HTTP_CLOSE              "Connection: close\r\n"
#define   HTTP_CR_LF              "\r\n"
#define   HTTP_END_OF_HEADERS     "\r\n\r\n"
#define   HTTP_CT_SIZE            40
#define   BOUNDARY_SIZE           80
#define   MAXLINE                 256

#define   UPLOAD_BUF_SIZE         8192 /* 6000000 */

typedef struct http_socket
{
  enum            { READY, PROCESS, DATA, COMPLETE, RESET, CLOSE } state; 
  enum            { UNKNOWN=0, GET, POST } action;
  enum            { COUNTED=0,CHUNKED,UNKNOWN_ENC } rx_encoding;
  int             fd;                               /* Socket descriptor */
  int             close;                            /* Close the connection after we're done? */
  int             content_length;                   /* Extracted content length */
  int             content_received;                 /* Content we've received on this connection. */
  int             keep_alive_count;                 /* No. of files tx'd in single connection */
  int             file_length;                      /* Length of the current file being sent */
  int             data_sent;                        /* Number of bytes already sent */
  int             file_upload;                      /* File upload flag. */
  FILE*           file_handle;                      /* File handle for file we're sending */
  unsigned long   activity_time;                    /* Time of last HTTP activity */
  char*           rx_rd_pos;                        /* position we've read up to */
  char*           rx_wr_pos;                        /* position we've written up to */
  char*           srec_start;                       /* place holder for the start of an SREC buffer. */
  char*           srec_end;	                        /* place holder for the end of an SREC buffer. */
  char*           rx_buffer;                        /* pointer to global RX buffer */
  char*           tx_buffer;                        /* pointer to global TX buffer */
  char            filename[256];                    /* filename for an uploaded file */
  char            flash_device[20];                 /* Flash Device field */
  char            content_type[HTTP_CT_SIZE];       /* content type for detecting multipart forms. */
  char            boundary[BOUNDARY_SIZE];          /* Boundary between elements of a multi-part form. */
  char            uri[HTTP_URI_SIZE];               /* URI buffer */
}http_conn;

struct http_form_data
{
  unsigned char LED_ON;
  unsigned char SSD_ON;
  char LCD_TEXT[20];
  unsigned char File_Upload[20];
};

extern struct vfs_file httplist[];
extern int    httplist_size;
extern struct vfroutines httpio;

extern int close (int fd);
extern void prep_httpdata(void);
extern void die_with_error(char err_msg[256]);

#endif   /* _HTTP_PORT_H */
/* end of file http_port.h */

