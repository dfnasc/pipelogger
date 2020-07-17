#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>

char g_buf[4096];
char g_ident[128];
char g_src[1024];

FILE *g_logf = NULL;

void pl_open_pipe() {

   if (!g_logf) {
      g_logf = fopen(g_src, "r");
   }
}

void pl_close_pipe() {
   if (g_logf) {
      fclose(g_logf);
      g_logf = NULL;
   }
}

char* pl_read_pipe() {

   if (g_logf) {
      memset(g_buf, 0, sizeof(g_buf));
      return fgets(g_buf, sizeof(g_buf) - 1, g_logf);
   }

   return NULL;
}

void pl_run(const char *ident, const char *src) {

   int miss_count = 0;

   strncpy(g_ident, ident, sizeof(g_ident));
   strncpy(g_src, src, sizeof(g_src));

   openlog(ident, 0, 0);

   while (1) {

      char *lmsg = NULL;

      pl_open_pipe();

      lmsg = pl_read_pipe();

      if (lmsg) {
         syslog(LOG_INFO, g_buf);
         miss_count = 0;
      } else {
         miss_count++;
         sleep(1);
      }

      if (miss_count >= 3) {
         pl_close_pipe(); // force reopen
      }
   }

   closelog();
}

int main(int argc, const char *argv[]) {

   if (argc != 3) {
      fprintf(stderr, "Usage:\n\tpipelogger <ident> <source file>\n\n");
      exit(1);
   }

   pid_t pid = fork();

   if (pid != 0) {
      exit(0);
   } else {
      pl_run(argv[1], argv[2]);
   }

   return 0;
}
