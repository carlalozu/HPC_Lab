#include <iostream>
#include <unistd.h> // For gethostname function

int main()
{
    char hostname[256]; // guess for size of hostname

    // Get the host name
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        // Success
        std::cout << "Hello world, this program is running on " << hostname << std::endl;
    }
    else
    {
        // Error
        std::cerr << "Failed to get the host name" << std::endl;
    }

    return 0;
}
