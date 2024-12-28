
#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int
main(int argc, char *argv[])
{
    int pid1, pid2, pid3;
    struct pstat st;

    printf(1, "Lottery scheduling test starting...\n");

    // First process - 30 tickets
    pid1 = fork();
    if(pid1 < 0) {
        printf(1, "fork failed\n");
        exit();
    }
    if(pid1 == 0) {
        if(settickets(30) < 0) {
            printf(1, "settickets failed\n");
            exit();
        }
        while(1); // Spin
    }

    // Second process - 20 tickets
    pid2 = fork();
    if(pid2 < 0) {
        printf(1, "fork failed\n");
        kill(pid1);
        exit();
    }
    if(pid2 == 0) {
        if(settickets(20) < 0) {
            printf(1, "settickets failed\n");
            exit();
        }
        while(1); // Spin
    }

    // Third process - 10 tickets
    pid3 = fork();
    if(pid3 < 0) {
        printf(1, "fork failed\n");
        kill(pid1);
        kill(pid2);
        exit();
    }
    if(pid3 == 0) {
        if(settickets(10) < 0) {
            printf(1, "settickets failed\n");
            exit();
        }
        while(1); // Spin
    }

    // Parent monitors children
    int i;
    for(i = 0; i < 50; i++) {
        if(getpinfo(&st) != 0) {
            printf(1, "getpinfo failed\n");
            goto cleanup;
        }
        
        // Print process info every few iterations
        if(i % 10 == 0) {
            printf(1, "\nIteration %d:\n", i);
            int j;
            for(j = 0; j < NPROC; j++) {
                if(st.inuse[j] && (st.pid[j] == pid1 || 
                                 st.pid[j] == pid2 || 
                                 st.pid[j] == pid3)) {
                    printf(1, "PID: %d, tickets: %d, ticks: %d\n",
                           st.pid[j], st.tickets[j], st.ticks[j]);
                }
            }
        }
        sleep(100);  // Wait a bit between checks
    }

cleanup:
    // Kill child processes
    kill(pid1);
    kill(pid2);
    kill(pid3);

    // Wait for all children to exit
    while(wait() > 0);
    
    exit();
}
