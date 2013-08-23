
/* Simple rm clone for DOS/WIN32
 *
 * Note: 
 *  1) This rm does not support removing of directory trees.
 *  2) Use the MinGW GCC compiler to compile this file.
 *
 * This file is part of pico]OS. License: modified BSD
 */


#include <io.h>
#include <stdio.h>
#include <string.h>

#define MAXPATHLEN   500


int main(int argc, char *argv[])
{
  struct _finddata_t fn;
  char c, buf[MAXPATHLEN];
  int d, b, p, i, h;

  /* walk over all parameters */  
  d = 0;
  for (p = 1; p < argc; p++)
  {
    /* skip all flag parameters */
    if (argv[p][0] == '-')
      continue;

    /* Copy string, replace slash by backslash. 
       Remember position of last slash. */
    for (b = 0, i = 0; (i < MAXPATHLEN-1) && (argv[p][i] != 0); i++)
    {
      c = argv[p][i];
      if ((c == '/') || (c == '\\'))
      {
        buf[i] = '\\';
        b = i + 1;
      }
      else
      {
        buf[i] = c;
      }
    }
    buf[i] = 0;
    if (b == i)
    {
      strcpy(buf+b, "*.*");
    }

    /* find all files */
    d = 1;
    h = _findfirst(buf, &fn);
    i = h;
    while (i != -1)
    {
      /* copy filename to buffer */
      strcpy(buf+b, fn.name);

      if ((fn.attrib & _A_SUBDIR) == 0)
      {
        /* delete file */
        remove(buf);
      }
      else
      {
        /* delete (empty) directory */
        rmdir(buf);
      }

      i = _findnext(h, &fn);
    }
    _findclose(h);

  }

  if (d == 0)
  {
    printf("\nUnix rm clone for the DOS/WIN32 environment\n");
  }

  return 0;
}
