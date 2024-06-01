#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#define BUILD_LIB_SCRIPT "../buildscripts/build_lib.sh"
#define BUILD_TESTS_SCRIPT "../buildscripts/build_tests.sh"
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define HOTRELOAD_PID_FILE "/tmp/hotreload.pid"
#define TESTS_PID_FILE "/tmp/tests.pid"
#define MAX_WATCHES 1024

int fd;
int wd[MAX_WATCHES];
int wd_count = 0;

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
            printf("Signal sent to hotreload (PID: %d)\n", hotreload_pid);
        } else {
            perror("Error opening Hotreload PID file");
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
            printf("Signal sent to tests (PID: %d)\n", tests_pid);
        } else {
            perror("Error opening Tests PID file");
        }
    }
}

void watch_directory(const char* directory_to_watch) {
    struct dirent *entry;
    DIR *dp = opendir(directory_to_watch);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    wd[wd_count] = inotify_add_watch(fd, directory_to_watch, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd[wd_count] == -1) {
        printf("Cannot watch directory '%s'\n", directory_to_watch);
        perror("inotify_add_watch");
    } else {
        printf("Watching directory: %s\n", directory_to_watch);
        wd_count++;
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_DIR) {
            char path[PATH_MAX];
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                snprintf(path, sizeof(path), "%s/%s", directory_to_watch, entry->d_name);
                watch_directory(path);
            }
        }
    }

    closedir(dp);
}

void handle_events() {
    char buffer[EVENT_BUF_LEN];
    int length, i = 0;

    length = read(fd, buffer, EVENT_BUF_LEN);
    if (length < 0) {
        perror("read");
    }

    while (i < length) {
        struct inotify_event *event = (struct inotify_event*)&buffer[i];
        if (event->len) {
            printf("Event detected: %s ", event->name);
            if (event->mask & IN_MODIFY) {
                printf("IN_MODIFY ");
                if (strstr(event->name, ".c") || strstr(event->name, ".cpp") || strstr(event->name, ".h")) {
                    printf("(relevant file extension)\n");
                    compile_dll();
                }
            } else if (event->mask & IN_CREATE) {
                printf("IN_CREATE ");
                if (event->mask & IN_ISDIR) {
                    printf("(directory)\n");
                    char path[PATH_MAX];
                    snprintf(path, sizeof(path), "%s/%s", event->name, event->name);
                    watch_directory(path);
                } else {
                    printf("(file)\n");
                }
            } else if (event->mask & IN_DELETE) {
                printf("IN_DELETE\n");
            } else {
                printf("\n");
            }
        }
        i += EVENT_SIZE + event->len;
    }
}

int main(int argc, char* argv[]) {
    const char *directories_to_watch[] = {"../src", "../tests"};
    int num_directories = sizeof(directories_to_watch) / sizeof(directories_to_watch[0]);

    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_directories; i++) {
        watch_directory(directories_to_watch[i]);
    }

    while (1) {
        handle_events();
    }

    for (int i = 0; i < wd_count; i++) {
        inotify_rm_watch(fd, wd[i]);
    }
    close(fd);

    return 0;
}
