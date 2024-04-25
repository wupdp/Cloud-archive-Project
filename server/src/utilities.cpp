#include "../include/utilities.h"
#include <unistd.h>
#include <limits.h>

std::string get_current_dir() {
    char temp[PATH_MAX];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}