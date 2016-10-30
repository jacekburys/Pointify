#include <stdio.h>

#define INFO(format, ...) fprintf(stdout, "[INFO]\t" format "\n", ## __VA_ARGS__)
#define ERROR(format, ...) fprintf(stderr, "[ERROR]\t" format "\n", ## __VA_ARGS__)
#define FATAL(format, ...) { fprintf(stderr, "[FATAL]\t" format "\n", ## __VA_ARGS__); exit(1); }

