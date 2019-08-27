
//Dependencies
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "debug.h"

extern UART_HandleTypeDef huart3;
extern void  dputchar(int chr);

/**
 * @brief Display the contents of an array
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @param[in] prepend String to prepend to the left of each line
 * @param[in] data Pointer to the data array
 * @param[in] length Number of bytes to display
 **/

void debugDisplayArray(FILE *stream,
   const char_t *prepend, const void *data, size_t length)
{
   uint_t i;

   //Dump the contents of the array
   for(i = 0; i < length; i++)
   {
      //Beginning of a new line?
      if((i % 16) == 0)
      {
         TRACE_PRINTF("%s", prepend);
      }

      //Display current data byte
      TRACE_PRINTF("%02" PRIX8 " ", *((const uint8_t *) data + i));

      //End of current line?
      if((i % 16) == 15 || i == (length - 1))
      {
         TRACE_PRINTF("\r\n");
      }
   }
}

/**
 * @brief Write character to stream
 * @param[in] c The character to be written
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @return On success, the character written is returned. If a writing
 *   error occurs, EOF is returned
 **/

int_t fputc(int_t c, FILE *stream)
{
   //Standard output or error output?
   if(stream == stdout || stream == stderr)
   {
      //Transmit data
      dputchar(c);

      //On success, the character written is returned
      return c;
   }
   //Unknown output?
   else
   {
      //If a writing error occurs, EOF is returned
      return EOF;
   }
}