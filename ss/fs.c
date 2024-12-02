#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "fs.h"

static int MOUNT_FLAG = 0;
static union block SUPERBLOCK;
static union block BLOCK_BITMAP;
static union block INODE_BITMAP;

int fs_format()
{   
    // TODO: Implement this function
    return 0;
}

int fs_mount()
{
    //TODO: Implement this function
    return 0;
}

void fs_unmount()
{
    if (MOUNT_FLAG == 0)
    {
        printf("\tError: Disk is not mounted.\n");
        return;
    }
    // Set the mount flag to 0
    MOUNT_FLAG = 0;
}

int fs_create(char *path, int is_directory)
{
    //TODO: Implement this function
    return -1; // set the return value accordingly
}

int fs_remove(char *path)
{
    //TODO: Implement this function
    return 0;
}

int fs_read(char *path, void *buf, size_t count, off_t offset)
{
    //TODO: Implement this function
    return 0;
}

int fs_write(char *path, void *buf, size_t count, int append)
{
    //TODO: Implement this function
    return 0;
}

int fs_list(char *path)
{
    //TODO: Implement this function
    return 0;
}

void fs_stat()
{
    if (MOUNT_FLAG == 0)
    {
        printf("\tError: Disk is not mounted.\n");
        return;
    }

    printf("Superblock:\n");
    printf("    Blocks: %d\n", SUPERBLOCK.superblock.s_blocks_count);
    printf("    Inodes: %d\n", SUPERBLOCK.superblock.s_inodes_count);
    printf("    Inode Table Block Start: %d\n", SUPERBLOCK.superblock.s_inode_table_block_start);
    printf("    Data Blocks Start: %d\n", SUPERBLOCK.superblock.s_data_blocks_start);
}