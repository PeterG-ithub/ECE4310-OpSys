#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    printf("Hi world\n");
    int fp = open("/dev/test_mod02", O_RDWR);
    if(fp < 0)
    {
        printf("Open Error\n");
        return -1;
    }

    char buffer[64];
    read(fp, buffer, 10);
    printf("%s\n\n", buffer);
    return 0;
}
