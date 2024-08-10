#include <iostream>
#include <climits>
#include <unistd.h>

int main() {
    char hostname[HOST_NAME_MAX + 1];
    gethostname(hostname, HOST_NAME_MAX + 1);

    std::cout << hostname << std::endl;

    return 0;
}
