#include "fs.h"
#include "disk.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <unistd.h>
#include <fcntl.h>



int main()
{
    // Initialize the disk.
    if (disk_init("test/images/temp.img", 128) == -1)
    {
        printf("ERROR: Could not initialize disk.\n");
        return -1;
    } else {
        printf("Disk initialized successfully.\n");
    }

    // Format the disk.
    if (fs_format() == -1)
    {
        printf("ERROR: Could not format disk.\n");
        return -1;
    } else {
        printf("Disk formatted successfully.\n");
    }

    // Mount the disk.
    if (fs_mount() == -1)
    {
        printf("ERROR: Could not mount disk.\n");
        return -1;
    } else {
        printf("Disk mounted successfully.\n");
    }

    // ----------------------------------------- Your code goes here ----------------------------------------- //
    // You can now use the file system functions. Use this space to test the functions you have implemented.



    

    // Close the disk.
    int log = 1;
    if (disk_close(log) == -1)
    {
        printf("ERROR: Could not close disk.\n");
        return -1;
    }

    return 0;
}