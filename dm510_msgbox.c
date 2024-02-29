#include "dm510_msgbox.h"
#include <linux/slab.h>

typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

static msg_t *bottom = NULL;
static msg_t *top = NULL;

int sys_dm510_msgbox_put( char *buffer, int length ) {
  msg_t* msg = kmalloc(sizeof(msg_t), __GFP_KERNEL);
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length);
  kmemdup(msg->message, buffer, length);

  if (bottom == NULL) {
    bottom = msg;
    top = msg;
  } else {
    /* not empty stack */
    msg->previous = top;
    top = msg;
  }
  return 0;
}

int sys_dm510_msgbox_get( char* buffer, int length ) {
  if (top != NULL) {
    msg_t* msg = top;
    int mlength = msg->length;
    top = msg->previous;

    /* copy message */
    kmemdup(buffer, msg->message, mlength);

    /* free memory */
    kfree(msg->message);
    kfree(msg);

    return mlength;
  }
  return -1;


}

int main ( void ) {
  /* test code here */
  return 0;
}