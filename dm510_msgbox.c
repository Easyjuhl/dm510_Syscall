#include "dm510_msgbox.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

static msg_t *bottom = NULL;
static msg_t *top = NULL;

int sys_dm510_msgbox_put( char *buffer, int length ) {
  if(!access_ok(buffer, length) || length < 0) {
    return -EFAULT;
  }
  msg_t* msg = kmalloc(sizeof(msg_t), GFP_KERNEL);
  if(msg == NULL) {
    return -ENOMEM;
  }
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length, GFP_KERNEL);
  if(msg->message == NULL) {
    return -ENOMEM;
  }
  
  unsigned long flags;

  local_irq_save(flags);  
  copy_from_user(msg->message, buffer, length);

  if (bottom == NULL) {
    bottom = msg;
    top = msg;
  } else {
    /* not empty stack */
    msg->previous = top;
    top = msg;
  }
  local_irq_restore(flags);
  return 0;
}

int sys_dm510_msgbox_get( char* buffer, int length ) {
  if (top != NULL) {
    if(!access_ok(buffer, length)) {
        return -EFAULT;
    }
    msg_t* msg = top;
    int mlength = msg->length;
    if(mlength != length) {
        return -EINVAL;
    }

    unsigned long flags;

    local_irq_save(flags);
    top = msg->previous;

    /* copy message */
    copy_to_user(buffer, msg->message, mlength);
    local_irq_restore(flags);
    /* free memory */
    kfree(msg->message);
    kfree(msg);

    return mlength;
  }
  return -ENODATA;
}