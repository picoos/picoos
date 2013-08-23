
/* Simple mkdir clone for DOS/WIN32
 *
 * Note: 
 *  Use the MinGW GCC compiler to compile this file.
 *
 * This file is part of pico]OS. License: modified BSD
 */


#include <io.h>
#include <stdio.h>

#define MAXPATHLEN   500


int main(int argc, char *argv[])
{
  char c, buf[MAXPATHLEN];
  int p, i;
  
  /* skip all flag parameters */
  for (p = 1; p < argc; p++)
  {
    if (argv[p][0] != '-')
      break;
  }
  if (p == argc)
  {
    printf("\nUnix mkdir clone for the DOS/WIN32 environment\n");
    return 0;
  }

  /* Copy string, replace slash by backslash. 
     Remember position of last slash. */
  for (i = 0; (i < MAXPATHLEN-1) && (argv[p][i] != 0); i++)
  {
    c = argv[p][i];
    if (c == '/')
    {
      buf[i] = '\\';
    }
    else
    {
      buf[i] = c;
    }
  }
  buf[i] = 0;

  /* make directory */
  mkdir(buf);

  return 0;
}
