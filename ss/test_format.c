#include "fs.h"
#include "disk.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int small_test()
{
    union block test_block;
    test_block.superblock.s_blocks_count = 16;
    test_block.superblock.s_inodes_count = 16;
    test_block.superblock.s_block_bitmap = 1;
    test_block.superblock.s_inode_bitmap = 2;
    test_block.superblock.s_inode_table_block_start = 3;
    test_block.superblock.s_data_blocks_start = 4;

    if (disk_init("test/images/user/format.img", 16) == -1)
    {
        printf("\tERROR: Could not initialize disk.\n");
        return -1;
    }

    // Format the disk.
    if (fs_format() == -1)
    {
        printf("\tERROR: Could not format disk.\n");
        return -1;
    }

    union block block;

    // Read the superblock.
    if (disk_read(0, block.data) == -1)
    {
        printf("\tERROR: Could not read superblock.\n");
        return -1;
    }

    // Match the superblock values.
    if (block.superblock.s_blocks_count != test_block.superblock.s_blocks_count ||
        block.superblock.s_inodes_count != test_block.superblock.s_inodes_count ||
        block.superblock.s_block_bitmap != test_block.superblock.s_block_bitmap ||
        block.superblock.s_inode_bitmap != test_block.superblock.s_inode_bitmap ||
        block.superblock.s_inode_table_block_start != test_block.superblock.s_inode_table_block_start ||
        block.superblock.s_data_blocks_start != test_block.superblock.s_data_blocks_start)
    {
        printf("\tERROR: Superblock values do not match.\n");
        return -1;
    }

    // Close STDOUT.
    // close(1);

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    // Open STDOUT.
    // open("/dev/tty", O_WRONLY);
}

int medium_test()
{
    
    union block test_block;
    test_block.superblock.s_blocks_count = 100;
    test_block.superblock.s_inodes_count = 100;
    test_block.superblock.s_block_bitmap = 1;
    test_block.superblock.s_inode_bitmap = 2;
    test_block.superblock.s_inode_table_block_start = 3;
    test_block.superblock.s_data_blocks_start = 5;

    if (disk_init("test/images/user/format.img", 100) == -1)
    {
        printf("\tERROR: Could not initialize disk.\n");
        return -1;
    }

    // Format the disk.
    if (fs_format() == -1)
    {
        printf("\tERROR: Could not format disk.\n");
        return -1;
    }

    union block block;

    // Read the superblock.
    if (disk_read(0, block.data) == -1)
    {
        printf("\tERROR: Could not read superblock.\n");
        return -1;
    }

    // Match the superblock values.
    if (block.superblock.s_blocks_count != test_block.superblock.s_blocks_count ||
        block.superblock.s_inodes_count != test_block.superblock.s_inodes_count ||
        block.superblock.s_block_bitmap != test_block.superblock.s_block_bitmap ||
        block.superblock.s_inode_bitmap != test_block.superblock.s_inode_bitmap ||
        block.superblock.s_inode_table_block_start != test_block.superblock.s_inode_table_block_start ||
        block.superblock.s_data_blocks_start != test_block.superblock.s_data_blocks_start)
    {
        printf("\tERROR: Superblock values do not match.\n");
        return -1;
    }

    // // Close STDOUT.
    // close(1);

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    // // Open STDOUT.
    // open("/dev/tty", O_WRONLY);
}

int large_test()
{

    union block test_block;
    test_block.superblock.s_blocks_count = 1000;
    test_block.superblock.s_inodes_count = 1000;
    test_block.superblock.s_block_bitmap = 1;
    test_block.superblock.s_inode_bitmap = 2;
    test_block.superblock.s_inode_table_block_start = 3;
    test_block.superblock.s_data_blocks_start = 19;

    if (disk_init("test/images/user/format.img", 1000) == -1)
    {
        printf("\tERROR: Could not initialize disk.\n");
        return -1;
    }

    // Format the disk.
    if (fs_format() == -1)
    {
        printf("\tERROR: Could not format disk.\n");
        return -1;
    }

    union block block;

    // Read the superblock.
    if (disk_read(0, block.data) == -1)
    {
        printf("\tERROR: Could not read superblock.\n");
        return -1;
    }

    // Match the superblock values.
    if (block.superblock.s_blocks_count != test_block.superblock.s_blocks_count ||
        block.superblock.s_inodes_count != test_block.superblock.s_inodes_count ||
        block.superblock.s_block_bitmap != test_block.superblock.s_block_bitmap ||
        block.superblock.s_inode_bitmap != test_block.superblock.s_inode_bitmap ||
        block.superblock.s_inode_table_block_start != test_block.superblock.s_inode_table_block_start ||
        block.superblock.s_data_blocks_start != test_block.superblock.s_data_blocks_start)
    {
        printf("\tERROR: Superblock values do not match.\n");
        return -1;
    }

    // Close STDOUT.
    // close(1);

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    // Open STDOUT.
    // open("/dev/tty", O_WRONLY);
}

int main()
{
    int total = 3;
    int passed = 0;

    printf("\tTesting fs_format()...\n");
    if (small_test() == -1)
    {
        printf("\t❌ Test Failed: Small Format.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Small Format.\n");
        passed += 1;
    }

    if (medium_test() == -1)
    {
        printf("\t❌ Test Failed: Medium Format.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Medium Format.\n");
        passed += 1;
    }

    if (large_test() == -1)
    {
        printf("\t❌ Test Failed: Large Format.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Large Format.\n");
        passed += 1;
    }

    printf("\t%d/%d Format test(s) passed.\n", passed, total);
    printf("\033[1;33m\tTotal Marks: %d/%d\n\033[0m", passed * 3, total * 3);

    return 0;
}