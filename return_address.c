#include <stdio.h>
#include <stdlib.h>

struct stack_frame {
  struct stack_frame* next;
  void* ret;
};

void ** get_addr_of_ret_addr() {
  /* x86/gcc-specific: this tells gcc that the fp
     variable should be set to the %ebp register
     which keeps the frame pointer */
  register struct stack_frame* fp asm("ebp");
  // the rest just walks through the linked list
  struct stack_frame* frame = fp; // This is the stack frame of get_ebp
  // Move down the stack to the stack frame which called get_ebp
  frame = frame->next;
  // We want to return the *address* of the retur address 
  return &frame->ret;
}

void dummy() {
  void **addr_of_ret_addr;
  addr_of_ret_addr = get_addr_of_ret_addr();
  printf(" My return address is stored at %p\n", get_addr_of_ret_addr);
  // In order to prove we have the right address, we now overwrite it
  // If we see a segfault, we've returned somewhere wrong and it's
  // the right location
  *addr_of_ret_addr = (void *) 0x61616161;
  printf("This should run fine\n");
  return;
}

int main(int argc, char *argv[]) {
  dummy();
  printf("This should never run!\n");
}
