#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pcre.h>
#include <string.h>

int is_valid_regex(const char* regex_pattern) {
    pcre *re;
    const char *errptr;
    int erroroffset;
    re = pcre_compile(regex_pattern, 0, &errptr, &erroroffset, NULL);
    if (re == NULL) {
        fprintf(stderr, "%s", errptr);
        return 0;
    }
    pcre_free(re);
    return 1;
}

void search_regex_in_file(const char* filename, const char* regex_pattern) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return;
    }
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char* file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("map failed");
        close(fd);
        return;
    }
    pcre *re;
    const char *errptr;
    int erroroffset;
    re = pcre_compile(regex_pattern, 0, &errptr, &erroroffset, NULL);
    int ovector[250];
    int offset = 0;
    while (offset < file_size && pcre_exec(re, NULL, file_content, file_size, offset, 0, ovector, 30) >= 0) {
        int line_start = ovector[0];
        int line_end = ovector[1];
        int line_number = 1;
        while (line_start > 0 && file_content[line_start - 1] != '\n') {
            --line_start;
        }
        for (int i = 0; i < line_start; ++i) {
            if (file_content[i] == '\n') {
                ++line_number;
            }
        }
        while (line_end < file_size && file_content[line_end] != '\n') {
            ++line_end;
        }
        printf("%s:%d: %.*s\n", filename, line_number, line_end - line_start, file_content + line_start);
        offset = line_end + 1;
    }

    pcre_free(re);
    munmap(file_content, file_size);
    close(fd);
}

void search_regex_in_directory(const char* dir, const char* regex_pattern) {
    DIR* directory = opendir(dir);
    if (directory == NULL) {
        perror("opendir failed");
        exit(EXIT_FAILURE);
    }
    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_REG) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
            search_regex_in_file(path, regex_pattern);
        } else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
            search_regex_in_directory(path, regex_pattern);
        }
    }

    closedir(directory);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    }
    const char* regex_pattern = argv[1];
    const char* dir = argv[2];
    if (!is_valid_regex(regex_pattern)) {
        exit(EXIT_FAILURE);
    }
    search_regex_in_directory(dir, regex_pattern);

    return 0;
}

