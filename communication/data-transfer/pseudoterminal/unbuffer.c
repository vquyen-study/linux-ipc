/**
 * 
 * WARNING!!!! THIS SAMPLES HAS NOT TESTED YET. 
 *  
 */

#define _XOPEN_SOURCE 600
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>  /* Type definitions used by many programs       */
#include <stdio.h>      /* Standard I/O functions                       */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls             */
#include <errno.h>      /* Declares errno and defines error constants   */
#include <string.h>     /* Commonly used string-handling functions      */
#include <sys/select.h> /*  Implement select                            */

#define MAX_SNAME 1000
#define BUF_SIZE 256
#define MAX_SNAME 1000

struct termios ttyOrig;

static void ttyReset(void);

void err_exit(char *str);
int ptyMasterOpen(char *slaveName, size_t snLen);
pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen,
                const struct termios *slaveTermios, 
                const struct winsize *slaveWS);
int ttySetRaw(int fd, struct termios *prevTermios);

int main(int argc, char **argv)
{
    char slaveName[MAX_SNAME];
    int masterFd;
    fd_set inFds;
    char buf[BUF_SIZE];
    ssize_t numRead;
    struct winsize ws;

    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
        err_exit("tcgetattr");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &ws) < 0)
        err_exit("TIOCGWINSZ error");

    switch (ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws)) {
    case -1:
        err_exit("ptyFork");

    case 0:     /* Child executes command given in argv[1]... */
        execvp(argv[1], &argv[1]);
        err_exit("execvp");

    default:    /* Parent relays data between terminal and pty master */

        ttySetRaw(STDIN_FILENO, &ttyOrig);
        if (atexit(ttyReset) != 0)
            err_exit("atexit");

        for (;;) {
            FD_ZERO(&inFds);
            FD_SET(STDIN_FILENO, &inFds);
            FD_SET(masterFd, &inFds);
            if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1)
                err_exit("select");

            if (FD_ISSET(STDIN_FILENO, &inFds)) {
                numRead = read(STDIN_FILENO, buf, BUF_SIZE);
                if (numRead <= 0)
                    exit(EXIT_SUCCESS);

                if (write(masterFd, buf, numRead) != numRead)
                    err_exit("partial/failed write (masterFd)");
            }

            if (FD_ISSET(masterFd, &inFds)) {
                numRead = read(masterFd, buf, BUF_SIZE);
                if (numRead <= 0)
                    exit(EXIT_SUCCESS);

                if (write(STDOUT_FILENO, buf, numRead) != numRead)
                    err_exit("partial/failed write (STDOUT_FILENO)");
            }
        }
    }
    return 0;
}

int ttySetRaw(int fd, struct termios *prevTermios)
{
    struct termios t;

    if (tcgetattr(fd, &t) == -1)
        return -1;

    if (prevTermios != NULL)
        *prevTermios = t;

    t.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
                        /* Noncanonical mode, disable signals, extended
                           input processing, and echoing */

    t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR |
                      INPCK | ISTRIP | IXON | PARMRK);
                        /* Disable special handling of CR, NL, and BREAK.
                           No 8th-bit stripping or parity error handling.
                           Disable START/STOP output flow control. */

    t.c_oflag &= ~OPOST;                /* Disable all output processing */

    t.c_cc[VMIN] = 1;                   /* Character-at-a-time input */
    t.c_cc[VTIME] = 0;                  /* with blocking */

    if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
        return -1;

    return 0;
}

void err_exit(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

/* Reset terminal mode on program exit */
static void ttyReset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1)
        err_exit("tcsetattr");
}

int ptyMasterOpen(char *slaveName, size_t snLen)
{
    int masterFd, savedErrno;
    char *p;

    masterFd = posix_openpt(O_RDWR | O_NOCTTY);         /* Open pty master */
    if (masterFd == -1)
        return -1;

    if (grantpt(masterFd) == -1) {              /* Grant access to slave pty */
        savedErrno = errno;
        close(masterFd);                        /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    if (unlockpt(masterFd) == -1) {             /* Unlock slave pty */
        savedErrno = errno;
        close(masterFd);                        /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    p = ptsname(masterFd);                      /* Get slave pty name */
    if (p == NULL) {
        savedErrno = errno;
        close(masterFd);                        /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    if (strlen(p) < snLen) {
        strncpy(slaveName, p, snLen);
    } else {                    /* Return an error if buffer too small */
        close(masterFd);
        errno = EOVERFLOW;
        return -1;
    }

    return masterFd;
}

pid_t ptyFork(int *masterFd, char *slaveName, size_t snLen,
        const struct termios *slaveTermios, const struct winsize *slaveWS)
{
    int mfd, slaveFd, savedErrno;
    pid_t childPid;
    char slname[MAX_SNAME];

    mfd = ptyMasterOpen(slname, MAX_SNAME);
    if (mfd == -1)
        return -1;

    if (slaveName != NULL) {            /* Return slave name to caller */
        if (strlen(slname) < snLen) {
            strncpy(slaveName, slname, snLen);

        } else {                        /* 'slaveName' was too small */
            close(mfd);
            errno = EOVERFLOW;
            return -1;
        }
    }

    childPid = fork();

    if (childPid == -1) {               /* fork() failed */
        savedErrno = errno;             /* close() might change 'errno' */
        close(mfd);                     /* Don't leak file descriptors */
        errno = savedErrno;
        return -1;
    }

    if (childPid != 0) {                /* Parent */
        *masterFd = mfd;                /* Only parent gets master fd */
        return childPid;                /* Like parent of fork() */
    }

    /* Child falls through to here */

    if (setsid() == -1)                 /* Start a new session */
        err_exit("ptyFork:setsid");

    close(mfd);                         /* Not needed in child */

    slaveFd = open(slname, O_RDWR);     /* Becomes controlling tty */
    if (slaveFd == -1)
        err_exit("ptyFork:open-slave");

#ifdef TIOCSCTTY                        /* Acquire controlling tty on BSD */
    if (ioctl(slaveFd, TIOCSCTTY, 0) == -1)
        err_exit("ptyFork:ioctl-TIOCSCTTY");
#endif

    if (slaveTermios != NULL)           /* Set slave tty attributes */
        if (tcsetattr(slaveFd, TCSANOW, slaveTermios) == -1)
            err_exit("ptyFork:tcsetattr");

    if (slaveWS != NULL)                /* Set slave tty window size */
        if (ioctl(slaveFd, TIOCSWINSZ, slaveWS) == -1)
            err_exit("ptyFork:ioctl-TIOCSWINSZ");

    /* Duplicate pty slave to be child's stdin, stdout, and stderr */

    if (dup2(slaveFd, STDIN_FILENO) != STDIN_FILENO)
        err_exit("ptyFork:dup2-STDIN_FILENO");
    if (dup2(slaveFd, STDOUT_FILENO) != STDOUT_FILENO)
        err_exit("ptyFork:dup2-STDOUT_FILENO");
    if (dup2(slaveFd, STDERR_FILENO) != STDERR_FILENO)
        err_exit("ptyFork:dup2-STDERR_FILENO");

    if (slaveFd > STDERR_FILENO)        /* Safety check */
        close(slaveFd);                 /* No longer need this fd */

    return 0;                           /* Like child of fork() */
}