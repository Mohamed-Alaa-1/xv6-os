#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main() {
    printf(1, "Initial read count: %d\n", getreadcount());
    char buf[100];
    read(0, buf, 10); // Perform a read
    printf(1, "Read count after one read: %d\n", getreadcount());
    exit();
}
