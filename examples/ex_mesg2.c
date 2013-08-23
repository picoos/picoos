/*
 *  pico]OS message example 2
 *
 *  How to use message boxes for inter task communication by using
 *  the function posMessageWait for "non-blocking" message receives.
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
#if POSCFG_FEATURE_GETTASK == 0
#error The feature POSCFG_FEATURE_GETTASK is not enabled!
#endif
#if POSCFG_FEATURE_MSGBOXES == 0
#error The feature POSCFG_FEATURE_MSGBOXES is not enabled!
#endif
#if POSCFG_FEATURE_MSGWAIT == 0
#error The feature POSCFG_FEATURE_MSGWAIT is not enabled!
#endif
#if POSCFG_MSG_MEMORY == 0
#error POSCFG_MSG_MEMORY must be defined to 1 for this example!
#endif
#if POSCFG_FEATURE_JIFFIES == 0
#error The feature POSCFG_FEATURE_JIFFIES is not enabled!
#endif
#if NOSCFG_FEATURE_CONOUT == 0
#error The feature NOSCFG_FEATURE_CONOUT is not enabled!
#endif


#define MAX_BUF_SIZE    (POSCFG_MSG_BUFSIZE - \
                          (sizeof(UVAR_t) + sizeof(POSTASK_t)))


/* message definition */
typedef struct {
  UVAR_t        msgid;
  union {
    int         number;
    POSTASK_t   taskh;
  } payload;
} MSG_t;

#define MSGID_PRINTNUMBER   0x01
#define MSGID_EXITTASK      0x02
#define MSGID_ACKNOWLEDGE   0x03


/* function prototypes */
void receiverTask(void *arg);



/* This task is started by the first task.
 * It waits for messages and processes its contents.
 */
void receiverTask(void *arg)
{
  POSTASK_t dest;
  MSG_t     *msg;
  JIF_t     interval;
  JIF_t     expjif, sampjif;
  JIF_t     jiffiesRemain;

  /* avoid compiler warning */
  (void) arg;

  /* set interval to 1 second */
  interval = (JIF_t) MS(1000);

  /* initialize jiffies to expire */
  expjif = jiffies + interval;

  for(;;)
  {
    sampjif = jiffies;
    if (POS_TIMEAFTER(sampjif, expjif))
    {
      msg = NULL;
    }
    else
    {
      /* wait for a message (with timeout) */
      jiffiesRemain = expjif - sampjif;
      msg = (MSG_t*) posMessageWait((UINT_t)jiffiesRemain);
    }

    /* Test for timeout. This timeout happens every second,
       independently of the timing when messages are received. */
    if (msg == NULL)
    {
      nosPrint("timeout\n");

      /* restart "jiffie timer" */
      expjif += interval;
      continue;
    }

    /* process the valid message */
    switch (msg->msgid)
    {
      case MSGID_EXITTASK:
      {
        /* Send an acknowledge back. We re-use the message buffer. */

        /* remember destination task handle */
        dest = msg->payload.taskh;

        /* build and send acknowledge message */
        msg->msgid = MSGID_ACKNOWLEDGE;
        msg->payload.taskh = posTaskGetCurrent();
        posMessageSend(msg, dest);

        /* exit this task */
        return;
      }

      case MSGID_PRINTNUMBER:
      {
        /* print the number stored in the message */
        nosPrintf1("number: %i\n", msg->payload.number);
        posMessageFree(msg);
        break;
      }

      default:
      {
        /* we don't know how to handle this message, so we drop it */
        posMessageFree(msg);
        break;
      }

    } /* end switch(msg->msgid) */
  } /* end for(;;) */
}



/* This is the first function that is called in the multitasking context.
 * (See file ex_init4.c for how to setup pico]OS).
 */
void firsttask(void *arg)
{
  POSTASK_t rxtask;
  VAR_t     status;
  MSG_t     *msg;
  int       i;

  /* avoid compiler warning */
  (void) arg;

  /* start the receiver task */
  rxtask = nosTaskCreate(
                receiverTask,   /* pointer to new task-function            */
                NULL,           /* optional argument for the task-function */
                2,              /* priority level of the new task          */
                0,              /* stack size (0 = default size)           */
                "receiver");    /* optional name of the second task        */

  if (rxtask == NULL)
  {
    nosPrint("Failed to start receiver task!\n");
    return;
  }


  /* send 10 messages to the printer task */
  for (i=0; i<10; ++i)
  {
    /* send a message */
    msg = (MSG_t*) posMessageAlloc();
    if (msg != NULL)
    {
      msg->msgid = MSGID_PRINTNUMBER;
      msg->payload.number = i;

      status = posMessageSend(msg, rxtask);
      if (status != E_OK)
      {
        nosPrint("failed to send number message\n");
      }
    }

    /* wait a "random" time */
    posTaskSleep(2*HZ + ((HZ/3) * (i%3)));
  }

  /* send a message that will stop the receiver task */
  msg = (MSG_t*) posMessageAlloc();
  if (msg != NULL)
  {
    msg->msgid = MSGID_EXITTASK;
    msg->payload.taskh = posTaskGetCurrent();

    status = posMessageSend(msg, rxtask);
    if (status != E_OK)
    {
      nosPrint("failed to send exit message\n");
    }
  }

  /* wait for acknowledge */
  for(;;)
  {
    msg = posMessageWait(MS(1000));
    if (msg == NULL)
    {
      /* timeout */
      nosPrint("failed to stop receiver task!\n");
      break;
    }
    if ((msg->msgid == MSGID_ACKNOWLEDGE) &&
        (msg->payload.taskh == rxtask))
    {
      /* got acknowledge from receiver task */
      posMessageFree(msg);
      nosPrint("receiver task successfully stopped\n");
      break;
    }
    posMessageFree(msg);
  }
}
