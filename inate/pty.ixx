module;
#include <fcntl.h>
export module pty;
import std;
/** opens a pty 
 * 
 * /param flags flags used to open the pty
 * /returns file descriptor 
 * /retval -1 failure occured, check errno
 */
export int openpt(int flags = O_RDWR | O_NOCTTY); 
