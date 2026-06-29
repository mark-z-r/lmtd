module;
#define _XOPEN_SOURCE
#include <stdlib.h>
module pty;


int openpt(int flags){
  return posix_openpt(flags);
}

