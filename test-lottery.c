#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int main() {
  settickets(30);
  if (fork() == 0) {
    settickets(20);
    while (1); // Process B
  } else {
    if (fork() == 0) {
      settickets(10);
      while (1); // Process C
    } else {
      while (1); // Process A
    }
  }
  exit();
}
