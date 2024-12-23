#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int fd;
  char buf[100];
  
  // Open a file
  fd = open("README", 0);
  
  // Read from file
  read(fd, buf, 1);
  read(fd, buf, 1);
  
  // Get and print read count
  printf(1, "Read count: %d\n", getreadcount());
  
  close(fd);
  exit();
}
