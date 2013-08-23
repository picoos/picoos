
#include <picoos.h>


/* set CPU speed (in Hz) */
#define CPU_SPEED   400000000L


/*-------------------------------------------------------------------------*/


/* prototypes of local functions */
void c_start(void);
void init_segments(void);

/* external functions */
extern void u_print(const char *str);
extern void p_asm_setupTimerIrq(unsigned int timebase_ticks);
extern int  main(void);


/*-------------------------------------------------------------------------
 *  C segment initialization for the GNU -Compiler
 */

#ifdef GCC

extern unsigned char  __BSS_START__;
extern unsigned char  __BSS_END__;

void init_segments()
{
  unsigned int i;
  char *c;

  /* setup C data segments */
  i = (unsigned int)&__BSS_END__ - (unsigned int)&__BSS_START__;
  c = (char*) &__BSS_START__;
  for ( ; i > 0; i--)
  {
    *c = 0;
    c++;
  }
}

#endif


/*-------------------------------------------------------------------------
 *  C segment initialization for MetaWare's HighC/C++ -Compiler
 */

#ifdef MW

#define MAGIC ((const char*)"INI")

struct Entry {
  void *address; /* void* so debugger wont auto-dereference */
  long count;    /* If negative, fill in zeroes for abs(count) bytes */
  char data[1];
};

/* This is the header for version 2. No compression. */
struct I {
  char magic[3];
  unsigned char version;
  struct Entry entry[1];
};

/* This is the header for version 3 and later, which permits compression. */
struct I2 {
  char magic[3];
  unsigned char version;
  void (*decompress)(const char *src, unsigned src_len, char *dest, unsigned *destlen);
  struct Entry entry[1];
};

/* To check for presence of initdat, use _einitdat, since _initdat may
   happen to be based at 0 */
extern	char  _einitdat[];
#pragma weak  _einitdat
extern struct I2  _initdat;
#pragma weak  _initdat


void clearmem(void *adr, int count)
{
  char *c = (char*) adr;
  while (count > 0)
  {
    *c = 0;
    c++;
    count--;
  }
}

void _initcopy()
{
  struct Entry *ep;

  if (_einitdat == 0)
  {
	return; /* failure: no initdat section */
  }
  if (_initdat.magic[0] != MAGIC[0] ||
      _initdat.magic[1] != MAGIC[1] ||
      _initdat.magic[2] != MAGIC[2])
  {
    return;  /* failure: bad magic number */
  }
  if (_initdat.version > 3)
  {
    return;  /* failure: bad version number */
  }

  if (_initdat.version >= 3)
  {
    ep = _initdat.entry;
  }
  else
  {
    ep = ((struct I*)((void*)&_initdat))->entry;
  }
  while (! (ep->count == 0 && ep->address == 0))
  {
    if (ep->count == 0)
    {
      ep = (struct Entry*) (((char*)ep) + sizeof(long) + sizeof(void*));
    }
    else
    if (ep->count <= 0)
    {
      /* Fill in .bss section */
      unsigned size = -ep->count;
      clearmem(ep->address,size);
      ep = (struct Entry *)(ep->data);
    }
    else
    {
      /* failure: compressed segments are not supported here */
      return; 
    }
  }
}

void init_segments()
{
  _initcopy();
}

#endif


/*-------------------------------------------------------------------------*/


/*
 *  C Entry function, called from start.s
 */
void c_start()
{
  /* initialize data segments */
  init_segments();

  /* Setup a timer interrupt.
   * The timer is not yet started, this will be done
   * in p_pos_startFirstContext() in arch_a.s.
   */
  p_asm_setupTimerIrq(CPU_SPEED / HZ);

  /* call main program (test.c) */
  main();
}

