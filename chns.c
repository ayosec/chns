#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void copyns(char* pid) {
  DIR *nsdir;
  struct dirent *dirent;
  char nspath[256];

  snprintf(nspath, sizeof(nspath), "/proc/%s/ns", pid);
  nsdir = opendir(nspath);

  if(nsdir == NULL) {
    perror(nspath);
    exit(-2);
  }

  while((dirent = readdir(nsdir)) != NULL) {
    struct stat nsstat;
    char nsfile[256];

    snprintf(nsfile, sizeof(nsfile), "%s/%s", nspath, dirent->d_name);
    if(lstat(nsfile, &nsstat) == 0) {
      if(S_ISLNK(nsstat.st_mode)) {
        int fd = open(nsfile, O_RDONLY);
        if(fd != -1)
          setns(fd, 0);
        else
          perror(nsfile);
      }
    } else {
      perror(nsfile);
    }
  }

}

void setroot(char* pid) {
  char rootpath[256];
  char link[1024];

  snprintf(rootpath, sizeof(rootpath), "/proc/%s/root", pid);
  if(readlink(rootpath, link, sizeof(link)) > 0) {
    if(chroot(link) != 0)
      perror("chroot");
    chdir("/");
  }
}

int main(int argc, char** argv)
{
  if (argc < 3) {
    fprintf(stderr, "Usage: %s PID command\n", argv[0]);
    exit(-1);
  }

  copyns(argv[1]);
  setroot(argv[1]);

  execvp(argv[2], &argv[2]);
  perror(argv[2]);
  return -2;
}
