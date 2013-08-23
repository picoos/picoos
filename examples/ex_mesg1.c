/*
 *  pico]OS message example 1
 *
 *  How to use message boxes for inter task communication.
 *
 *  Three tasks are involved into the message communication:
 *    The first task is sending text messages to a second task,
 *    that will process (reformat) the text into either lower or
 *    upper case letters. The second task then sends the message
 *    to a third task that prints the text to the screen.
 *
 *  License:  modified BSD, see license.txt in the picoos root directory.
 *
 */


/* Include source code for pico]OS
 * initialization with nano layer.
 */
#include "ex_init4.c"


/* we need some features to be enabled */
#if POSCFG_FEATURE_SLEEP == 0
#error The feature POSCFG_FEATURE_SLEEP is not enabled!
#endif
#if POSCFG_FEATURE_MSGBOXES == 0
#error The feature POSCFG_FEATURE_MSGBOXES is not enabled!
#endif
#if POSCFG_MSG_MEMORY == 0
#error POSCFG_MSG_MEMORY must be defined to 1 for this example!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


#define MAX_BUF_SIZE    (POSCFG_MSG_BUFSIZE - \
                          (sizeof(UVAR_t) + sizeof(POSTASK_t)))


/* message definition */
typedef struct {
  UVAR_t      msgid;
  POSTASK_t   fwdtask;
  char        buffer[MAX_BUF_SIZE];
} MSG_t;

#define MSGID_TOUPPER   0x01
#define MSGID_TOLOWER   0x02



/* function prototypes */
void printerTask(void *arg);
void processorTask(void *arg);
void fillMessage(MSG_t *msg, UVAR_t type, POSTASK_t fwd, const char *str);



/* This task is started by the first task.
 * It waits for messages and print its contents.
 */
void printerTask(void *arg)
{
  MSG_t *msg;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* wait for a message */
    msg = (MSG_t*) posMessageGet();

    /* It is nearly impossible that posMessageGet fails, but anyway... */
    if (msg == NULL)
      return;

    /* print the content of the buffer,
       regardless of the message type */
    nosPrintf1("printer task: %s", msg->buffer);

    /* free the message buffer again */
    posMessageFree(msg);
  }
}



/* This task is started by the first task.
 * It processes incomming messages.
 * The processed messages are forwarded to
 * an other task that will print the content.
 */ 
void processorTask(void *arg)
{
  MSG_t *msg;
  VAR_t status;
  char  *s;

  /* avoid compiler warning */
  (void) arg;

  for(;;)
  {
    /* wait for a message */
    msg = (MSG_t*) posMessageGet();

    /* It is nearly impossible that posMessageGet fails, but anyway... */
    if (msg == NULL)
      return;

    /* process the message */
    switch (msg->msgid)
    {
      case MSGID_TOLOWER:
      {
        for (s = msg->buffer; *s != 0; s++)
        {
          if (*s >= 'A' && *s <= 'Z')
          {
            *s += 'a' - 'A';
          }
        }
        break;
      }

      case MSGID_TOUPPER:
      {
        for (s = msg->buffer; *s != 0; s++)
        {
          if (*s >= 'a' && *s <= 'z')
          {
            *s -= 'a' - 'A';
          }
        }
        break;
      }

      default:
      {
        /* we don't know how to handle this message */
        posMessageFree(msg);
        msg = NULL;
        break;
      }
    }

    /* forward the message to the printer task */
    if (msg != NULL)
    {
      if (msg->fwdtask != NULL)
      {
        status = posMessageSend(msg, msg->fwdtask);
        if (status != E_OK)
        {
          nosPrint("processorTask: Failed to send message\n");
        }
      }
      else
      {
        posMessageFree(msg);
      }
    }
  }
}



/* helper function: set up a message
 */
void fillMessage(MSG_t *msg, UVAR_t type, POSTASK_t fwd, const char *str)
{
  int  i;

  msg->msgid   = type;
  msg->fwdtask = fwd;

  for (i=0; ((msg->buffer[i] = str[i]) != 0) && (i < MAX_BUF_SIZE); ++i);
}



/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  POSTASK_t proct;
  POSTASK_t printt;
  VAR_t     status;
  MSG_t     *msg;

  (void) arg;

  /* start the printer task */
  printt = nosTaskCreate(
                printerTask,    /* pointer to new task-function            */
                NULL,           /* optional argument for the task-function */
                2,              /* priority level of the new task          */
                0,              /* stack size (0 = default size)           */
                "printer");     /* optional name of the second task        */

  if (printt == NULL)
  {
    nosPrint("Failed to start printer task!\n");
    return;
  }

  /* start the processor task */
  proct = nosTaskCreate(
                processorTask,  /* pointer to new task-function            */
                NULL,           /* optional argument for the task-function */
                3,              /* priority level of the new task          */
                0,              /* stack size (0 = default size)           */
                "processor");   /* optional name of the second task        */

  if (proct == NULL)
  {
    nosPrint("Failed to start processor task!\n");
    return;
  }

  /* send some messages to the processor task */

  msg = (MSG_t*) posMessageAlloc();
  if (msg != NULL)
  {
    fillMessage(msg, MSGID_TOUPPER, printt, "heLlo WorlD!\n");
    status = posMessageSend(msg, proct);
    if (status != E_OK)
    {
      nosPrint("failed to send message 1\n");
    }
  }

  msg = (MSG_t*) posMessageAlloc();
  if (msg != NULL)
  {
    fillMessage(msg, MSGID_TOLOWER, printt, "THIS ARE small LETTERS\n");
    status = posMessageSend(msg, proct);
    if (status != E_OK)
    {
      nosPrint("failed to send message 2\n");
    }
  }
}
