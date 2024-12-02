#include "fs.h"
#include "disk.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief Basic test for fs_create().
 * 
 * @return 0 on success, -1 on failure.
 */
int basic_test() {
    if (disk_init("test/images/user/create.img", 128) == -1)
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

    // Mount the disk.
    if (fs_mount() == -1)
    {
        printf("\tERROR: Could not mount disk.\n");
        return -1;
    }

    // Create a file.
    if (fs_create("/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file.\n");
        return -1;
    }

    // Create a directory.
    if (fs_create("/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory.\n");
        return -1;
    }

    // Create a file in the directory.
    if (fs_create("/dir1/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file.\n");
        return -1;
    }

    // Create a directory in the directory.
    if (fs_create("/dir1/dir2", 1) == -1)
    {
        printf("\tERROR: Could not create directory.\n");
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

    // Unmount
    fs_unmount();
    return 0;
}

/**
 * @brief Test for fs_create() with duplicate files and directories.
 * 
 * @return 0 on success, -1 on failure.
 */
int duplicate_test() {
    if (disk_init("test/images/user/create.img", 128) == -1)
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

    // Mount the disk.
    if (fs_mount() == -1)
    {
        printf("\tERROR: Could not mount disk.\n");
        return -1;
    }

    // Create a file.
    if (fs_create("/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file1\".\n");
        return -1;
    }

    // Try recreating the file. Expect an error.
    if (fs_create("/file1", 0) != -1)
    {
        printf("\tERROR: File already exists: \"/file1\".\n");
        return -1;
    }

    // Create a directory.
    if (fs_create("/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1\".\n");
        return -1;
    }

    // Try recreating the directory. Expect an error.
    if (fs_create("/dir1", 1) != -1)
    {
        printf("\tERROR: Directory already exists: \"/dir1\".\n");
        return -1;
    }

    // Create a directory in dir1.
    if (fs_create("/dir1/dir2", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir2\".\n");
        return -1;
    }

    // Try recreating the directory. Expect an error.
    if (fs_create("/dir1/dir2", 1) != -1)
    {
        printf("\tERROR: Directory already exists: \"/dir1/dir2\".\n");
        return -1;
    }

    // Create a file in dir2.
    if (fs_create("/dir1/dir2/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/dir2/file2\".\n");
        return -1;
    }

    // Try recreating the file. Expect an error.
    if (fs_create("/dir1/dir2/file2", 0) != -1)
    {
        printf("\tERROR: File already exists: \"/dir1/dir2/file2\".\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    // Unmount
    fs_unmount();


    return 0;
}

/**
 * @brief Test for fs_create() with missing parent directories.
 * 
 * @return 0 on success, -1 on failure.
 */
int advanced_test() {
    if (disk_init("test/images/user/create.img", 1000) == -1)
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

    // Mount the disk.
    if (fs_mount() == -1)
    {
        printf("\tERROR: Could not mount disk.\n");
        return -1;
    }
    
    // Create a file.
    if (fs_create("/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file1\".\n");
        return -1;
    }

    // Create a directory.
    if (fs_create("/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1\".\n");
        return -1;
    }

    // Create a directory with missing parent.
    if (fs_create("/dir2/dir3", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir2/dir3\".\n");
        return -1;
    }

    // Create a file with missing parent.
    if (fs_create("/dir2/dir4/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir2/dir4/file2\".\n");
        return -1;
    }

    // Try to create a file that already exists. Should return an error.
    if (fs_create("/dir2/dir4/file2", 0) != -1)
    {
        printf("\tERROR: File already exists: \"/dir2/dir4/file2\".\n");
        return -1;
    }

    // Create more files and directories.
    if (fs_create("/dir5/file3", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir5/file3\".\n");
        return -1;
    }

    if (fs_create("/dir2/dir6", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir2/dir6\".\n");
        return -1;
    }

    if (fs_create("/dir2/dir7", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir2/dir7\".\n");
        return -1;
    }

    if (fs_create("/dir2/file4", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir2/file4\".\n");
        return -1;
    }

    if (fs_create("/file5", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file5\".\n");
        return -1;
    }

    if (fs_create("/file6", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file6\".\n");
        return -1;
    }

    if (fs_create("/dir2/dir8/dir9", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir2/dir8/dir9\".\n");
        return -1;
    }

    if (fs_create("/dir2/dir8/dir9", 0) != -1)
    {
        printf("\tERROR: Directory already exists: \"/dir2/dir8/dir9\".\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    // Unmount
    fs_unmount();

    return 0;
}


int main()
{
    int total = 3;
    int passed = 0;
    printf("\tTesting fs_create()...\n");

    if (basic_test() == -1)
    {
        printf("\t❌ Test Failed: Basic test.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Basic test.\n");
        passed+=1;
    }

    if (duplicate_test() == -1)
    {
        printf("\t❌ Test Failed: Duplicate test.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Duplicate test.\n");
        passed+=1;
    }

    if (advanced_test() == -1)
    {
        printf("\t❌ Test Failed: Advanced test.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Advanced test.\n");
        passed+=1;
    }

    printf("\t%d/%d Create test(s) passed.\n", passed, total);
    printf("\033[1;33m\tTotal Marks: %d/%d\n\033[0m", passed * 5, total * 5);

    return 0;
}