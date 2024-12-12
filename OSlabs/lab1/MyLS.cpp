#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <errno.h>

// Цвета для выделения типов файлов
#define RESET "\033[0m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

// Функция для сортировки имен файлов
int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Печать прав доступа
void print_permissions(mode_t mode) {
    printf("%c", S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' : '-');
    printf("%c", mode & S_IRUSR ? 'r' : '-');
    printf("%c", mode & S_IWUSR ? 'w' : '-');
    printf("%c", mode & S_IXUSR ? 'x' : '-');
    printf("%c", mode & S_IRGRP ? 'r' : '-');
    printf("%c", mode & S_IWGRP ? 'w' : '-');
    printf("%c", mode & S_IXGRP ? 'x' : '-');
    printf("%c", mode & S_IROTH ? 'r' : '-');
    printf("%c", mode & S_IWOTH ? 'w' : '-');
    printf("%c", mode & S_IXOTH ? 'x' : '-');
}

// Печать файла с цветом
void print_with_color(const char *name, mode_t mode) {
    if (S_ISDIR(mode)) {
        printf(BLUE "%s" RESET, name);
    } else if (S_ISLNK(mode)) {
        printf(CYAN "%s" RESET, name);
    } else if (mode & S_IXUSR) {
        printf(GREEN "%s" RESET, name);
    } else {
        printf("%s", name);
    }
}

// Вывод информации о файле в формате `ls -l`
void print_file_info(const char *path, const char *name, struct stat *st) {
    print_permissions(st->st_mode);
    printf(" %ld", st->st_nlink);

    struct passwd *pwd = getpwuid(st->st_uid);
    struct group *grp = getgrgid(st->st_gid);
    printf(" %s %s", pwd ? pwd->pw_name : "?", grp ? grp->gr_name : "?");

    printf(" %5ld", st->st_size);

    char timebuf[64];
    struct tm *tm = localtime(&st->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm);
    printf(" %s ", timebuf);

    if (S_ISLNK(st->st_mode)) {
        char link_target[4096];
        ssize_t len = readlink(path, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(CYAN "%s -> %s" RESET, name, link_target);
        } else {
            print_with_color(name, st->st_mode);
        }
    } else {
        print_with_color(name, st->st_mode);
    }

    printf("\n");
}

// Функция для обработки одиночного файла
void handle_file(const char *path, int long_format) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        perror("lstat");
        return;
    }

    if (long_format) {
        // Исправлено: добавлено преобразование к const char*
        const char *name = strrchr(path, '/');
        print_file_info(path, name ? name + 1 : path, &st);
    } else {
        print_with_color(path, st.st_mode);
        printf("\n");
    }
}

// Функция для обработки директории
void list_directory(const char *path, int show_all, int long_format) {
    struct dirent **namelist;
    int count = scandir(path, &namelist, NULL, alphasort);
    if (count < 0) {
        perror("scandir");
        return;
    }

    for (int i = 0; i < count; ++i) {
        struct dirent *entry = namelist[i];
        if (!show_all && entry->d_name[0] == '.') {
            free(entry);
            continue;
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (lstat(full_path, &st) == -1) {
            perror("lstat");
            free(entry);
            continue;
        }

        if (long_format) {
            print_file_info(full_path, entry->d_name, &st);
        } else {
            print_with_color(entry->d_name, st.st_mode);
            printf("  ");
        }

        free(entry);
    }

    if (!long_format)
        printf("\n");

    free(namelist);
}

// Основная функция
int main(int argc, char *argv[]) {
    int opt, show_all = 0, long_format = 0;

    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'l':
                long_format = 1;
                break;
            case 'a':
                show_all = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-a] [path]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";
    struct stat st;
    if (lstat(path, &st) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    if (S_ISDIR(st.st_mode)) {
        list_directory(path, show_all, long_format);
    } else {
        handle_file(path, long_format);
    }

    return 0;
}
