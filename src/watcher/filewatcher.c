#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define BUILD_LIB_SCRIPT "./build_lib.sh"
#define BUILD_TESTS_SCRIPT "./build_tests.sh"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define HOTRELOAD_PID_FILE "/tmp/hotreload.pid"
#define TESTS_PID_FILE "/tmp/tests.pid"

void compile_dll() {
    // Call the shell script to compile the DLL
    if (system(BUILD_LIB_SCRIPT) != 0) {
        printf("Lib Compilation failed.\n");
    } else {
        printf("Lib Compilation successful.\n");

        FILE *hotreload_file = fopen(HOTRELOAD_PID_FILE, "r");

        if (hotreload_file) {
            pid_t hotreload_pid;
            fscanf(hotreload_file, "%d", &hotreload_pid);
            fclose(hotreload_file);
            kill(hotreload_pid, SIGUSR1);
            printf("Signals sent to hotreload (PID: %d)", hotreload_pid);
        } else {
            perror("Error opening Hotreload PID files");
        }
    }

    if (system(BUILD_TESTS_SCRIPT) != 0) {
        printf("Tests Compilation failed.\n");
    } else {
        printf("Tests Compilation successful.\n");
        FILE *tests_file = fopen(TESTS_PID_FILE, "r");
        if (tests_file) {
            pid_t tests_pid;
            fscanf(tests_file, "%d", &tests_pid);
            fclose(tests_file);
            kill(tests_pid, SIGUSR1);
            printf("Signals sent to hotreload (PID: %d)", tests_pid);
        } else {
            perror("Error opening Tests PID files");
        }
    }
}

void watch_directory(const char* directory_to_watch) {
    int fd, wd;
    char buffer[EVENT_BUF_LEN];
    
    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }
    
    wd = inotify_add_watch(fd, directory_to_watch, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd == -1) {
        printf("Cannot watch directory '%s'\n", directory_to_watch);
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0) {
            perror("read");
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event*)&buffer[i];
            if (event->len) {
                if (event->mask & IN_MODIFY) {
                    if (strstr(event->name, ".c") || strstr(event->name, ".cpp") || strstr(event->name, ".h")) {
                        compile_dll();
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

int main(int argc, char* argv[]) {
    char cwd[PATH_MAX];

    if (argc > 1) {
        if (chdir(argv[1]) != 0) {
            perror("chdir() to specified directory failed");
            return 1;
        }
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Watching directory: %s\n", cwd);
        watch_directory(cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    return 0;
}