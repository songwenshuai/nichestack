/* 
 * FILENAME: http_data.c
 *
 * Copyright 2009 By InterNiche Technologies Inc. All rights reserved.
 * JVN - Interniche Support Europe
 *
 * This module adds ROM-ed files 'index.htm' and 'favicon.ico' to 
 * the VFS for use in conjunction with the http listener and tftp
 * The files come with a separate IO 'device'. Hence #ifdef HT_EXTDEV
 * An embryonic 'CGI-alike' operation ensures that the index.htm file 
 * gets updated upon fread() to show basic dynamic content in the html. 
 * 
 * PUBLIC ROUTINES: prep_httpdata()
 */


#include "ipport.h"
#include "menu.h"
#include "http_port.h"

#ifdef WEBPORT

#if defined(VFS_FILES) && defined(HT_EXTDEV)

/* Map FILE to VFILE when using the VFS based html file */
#if defined(VFS_FILES) && defined(HT_EXTDEV)
#include "vfsfiles.h"
#else
#error This example needs VFS_FILES and HT_EXTDEV
#endif

/* 
 * Local file I/O functions. We need fread() as called by vfread()
 * of vfsfile.c under HT_EXTDEV, to retrieve the file content.
 * We also implement fseek() and ftell() 'dummies' (se below)
 * The VFS code handles open/close.
 */

static VFILE* 
http_fopen(char * name, char * mode)
{
   USE_ARG(mode);
   USE_ARG(name);
   return NULL;
}

static void   
http_fclose(VFILE * vfd)
{
   USE_ARG(vfd);
}

static int    
http_fwrite(char * buf, unsigned size, unsigned items, VFILE * vfd)
{
   USE_ARG(buf); 
   USE_ARG(size);
   USE_ARG(items);
   USE_ARG(vfd);   
   return 0;
}

static int    
http_fgetc(VFILE * vfd)
{    
   USE_ARG(vfd);
   return 0;
}

static int    
http_unlink(char * filename)
{
   USE_ARG(filename); 
   return 0;
}

/* fseek() here always resets to begin of file, regardless of mode */

static int
http_fseek(VFILE * vfd, long offset, int mode)
{
   vfd->cmploc = vfd->file->data;    
   USE_ARG(offset);
   USE_ARG(mode);
   return 0;
}

/* 
 * ftell() here always returns length of file as if it was preceeded 
 * by a real fseek(f, 0, SEEK_END) 
 */

static long   
http_ftell(VFILE * vfd)
{
   if (vfd && vfd->file)
      return vfd->file->real_size;
   else
      return 0;
}


/* fread() used both by httptest and tftp */

static int    
http_fread(char * buf, unsigned size, unsigned items, VFILE * vfd)
{
   u_long bcount;     
   u_long location;   
   unsigned long file_size = 0;

   /* determine real size */
   if (vfd && vfd->file)
      file_size = vfd->file->real_size; 

   /* offset within the file */
   location = (u_long)(vfd->cmploc - vfd->file->data);

   /* number of bytes to transfer to callers's buffer */
   bcount = (items * (u_long)size);   

   /* if near end of file, trim read count accordingly */
   if ((location + bcount) > file_size)
      bcount = ((u_long)file_size - location);

   /* trap bogus size items and end of memory conditions */
   if((location >= file_size) ||
      (bcount & 0xFFFF0000) ||
      (bcount == 0))
      return 0;

   /* copy data to given buf */
   MEMCPY(buf, vfd->cmploc, (unsigned)bcount);     
                                                                  
   /* before returning, adjust file position ptr for next read */
   vfd->cmploc += bcount;  

   /* return number of units read */
   return ((int)bcount/size);
}

 /* The httptest structure of io methods */

struct vfroutines httpio  =
{
   NULL,           /* link to next io list */
   http_fopen,     /* fopen  */
   http_fclose,    /* fclose */
   http_fread,     /* fread  */
   http_fwrite,    /* fwrite */
   http_fseek,     /* fseek  */
   http_ftell,     /* ftell  */
   http_fgetc,     /* fgetc  */
   http_unlink     /* unlink */
};


/* Initialisation */

void 
prep_httpdata(void)
{
   /* add our IO pointer to the master list */
   httpio.next = vfsystems;
   vfsystems = &httpio;

   /* add the httplist files to vfs list */
   httplist[0].next = vfsfiles;
   vfsfiles = &httplist[httplist_size - 1];
}


/* FUNCTION: lwsstart()
 *
 * start listener processing.
 *
 * RETURNS: Returns 0 if OK, non-zero if error.
 */

static int lwsstart(void * pio)
{	
   return 0;
}

/* FUNCTION: lwsstop()
 *
 * stop listener processing.
 *
 * RETURNS: Returns 0 if OK, non-zero if error.
 */

static int lwsstop(void * pio)
{

   return 0;
}

/* FUNCTION: lwsstat()
 *
 * stop listener processing.
 *
 * RETURNS: Returns 0 if OK, non-zero if error.
 */

static int lwsstat(void * pio)
{
   return 0;
}

static struct menu_op lwsmenu[]  =
{
   "listener", stooges,  "lite webserver cmds",   
   "lwsstart", lwsstart, "start lite webserver",
   "lwsstop",  lwsstop,  "stop lite webserver",
   "lwsstat",  lwsstat,  "stop lite stats",
   NULL,       0,        NULL,
};

/*
 * die_with_error()
 * 
 * This routine is just called when a blocking error occurs with the example
 * design. It deletes the current task.
 */
void die_with_error(char err_msg[256])
{
  printf("\n%s\n", err_msg);
  osDeleteTask(&Os_Prio_Self);
  
  while(1);
}


/* FUNCTION: prep_http()
 *
 * this is called by prep_modules() in allports.c,
 * at startup time. Install menu etc
 *
 * PARAM1:
 *
 * RETURNS: Returns 0 if OK, non-zero if error.
 */

int prep_http(void)
{
   /* install menu */
   install_menu(&lwsmenu[0]);
   /* create entry for index.htm in VFS */
   prep_httpdata(); 
  
   return 0; 
}

#endif  /* LITE_WEB_STUB and VFS_FILES and HT_EXTDEV */
#endif /* WEBPORT */