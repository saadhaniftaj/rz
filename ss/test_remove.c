#include "fs.h"
#include "disk.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


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

int match_list_outputs(const char *actual_list_file, const char *output_list_file) {
    FILE *actual_list = fopen(actual_list_file, "r");
    FILE *output_list = fopen(output_list_file, "r");

    if (actual_list == NULL || output_list == NULL) {
        perror("    Error opening file");
        return -1;
    }

    char actual_name[256], output_name[256];
    int actual_size, output_size;

    while (fscanf(actual_list, "%s %d", actual_name, &actual_size) != EOF) {
        if (fscanf(output_list, "%s %d", output_name, &output_size) == EOF) {
            fclose(actual_list);
            fclose(output_list);
            return -1;  // Output list has fewer lines than the actual list
        }

        if (strcmp(actual_name, output_name) != 0) {
            printf("\tMismatch: %s %s\n", actual_name, output_name);
            fclose(actual_list);
            fclose(output_list);
            return -1;  // Mismatch found
        }

        if (actual_size != output_size) {
            printf("\tSize incorrect for %s. Expected %d, got %d\n", actual_name, actual_size, output_size);
            fclose(actual_list);
            fclose(output_list);
            return -1;  // Size mismatch
        }
    }

    // Check if there are any extra lines in the output file
    if (fscanf(output_list, "%s %d", output_name, &output_size) != EOF) {
        printf("\tExtra line in output: %s %d\n", output_name, output_size);
        fclose(actual_list);
        fclose(output_list);
        return -1;  // Output list has more lines than the actual list
    }

    // Check if there are any extra lines in the actual file
    if (fscanf(actual_list, "%s %d", actual_name, &actual_size) != EOF) {
        printf("\tMissing line in output: %s %d\n", actual_name, actual_size);
        fclose(actual_list);
        fclose(output_list);
        return -1;  // Actual list has more lines than the output list
    }

    fclose(actual_list);
    fclose(output_list);
    return 0;  // All lines match
}


int rm_test_one()
{
    // Initialize the disk.
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

    // Several create calls here
    if (fs_create("/I_love_OS", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/I_love_OS\".\n");
        return -1;
    }

    if (fs_create("/dir2", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir2\".\n");
        return -1;
    }

    // create a file named mic check
    if (fs_create("/mic_check", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/mic_check\".\n");
        return -1;
    }

    // create a file named LUMSU
    if (fs_create("/LUMSU", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/LUMSU\".\n");
        return -1;
    }

    // create a Dir3
    if (fs_create("/dir3", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir3\".\n");
        return -1;
    }

    // create a dir1 in dir3
    if (fs_create("/dir3/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir3/dir1\".\n");
        return -1;
    }

    // create dir1 in dir1 in dir4
    if (fs_create("/dir4/dir1/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir4/dir1/dir1\".\n");
        return -1;
    }

    // Redirect STDOUT.
    int fd = open("test/dump/rm_root.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd == -1)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    if (dup2(fd, 1) == -1)
    {
        printf("\tERROR: Could not redirect STDOUT.\n");
        return -1;
    }

    // List the root directory.
    if (fs_list("/") == -1) //it will list the root dir in the file rm_root.txt
    {
        printf("\tERROR: Could not list root directory.\n");
        return -1;
    }

    // Flush STDOUT to ensure all data is written to the file.
    fflush(stdout);

    // Close STDOUT.
    close(1);

    // Open STDOUT.
    open("/dev/tty", O_WRONLY);

    // Match the output.
    if (match_list_outputs("test/data/list_root.txt", "test/dump/rm_root.txt") == -1)
    {
        printf("\tERROR: \"/\" Outputs do not match even before removing.\n");
        return -1;
    }

    // Remove the file.
    if (fs_remove("/I_love_OS") == -1)
    {
        printf("\tERROR: Could not remove file.\n");
        return -1;
    }

    fd = open("test/dump/rm_root.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd == -1)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    if (dup2(fd, 1) == -1)
    {
        printf("\tERROR: Could not redirect STDOUT.\n");
        return -1;
    }

    // List the root directory.
    if (fs_list("/") == -1) //it will list the root dir in the file rm_root.txt
    {
        printf("\tERROR: Could not list root directory.\n");
        return -1;
    }

    // Flush STDOUT to ensure all data is written to the file.
    fflush(stdout);

    // Close STDOUT.
    close(1);

    // Open STDOUT.
    open("/dev/tty", O_WRONLY);


    // Match the output.
    if (match_list_outputs("test/data/rm_root.txt", "test/dump/rm_root.txt") == -1)
    {
        printf("\tERROR: \"/\" Outputs do not match after removing.\n");
        return -1;
    }

     // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    // // Compare two disks.
    // if (system("cmp test/data/rm_1.txt test/dump/rm_1.txt") != 0)
    // {
    //     printf("\tERROR: Outputs do not match.\n");
    //     return -1;
    // }

    return 0;
}

int rm_test_two()
{
    // Initialize the disk.
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

    //create a directory
    if (fs_create("/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1\".\n");
        return -1;
    }

    //create a nested directory
    if (fs_create("/dir1/dir2", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir2\".\n");
        return -1;
    }

    //create a nested directory
    if (fs_create("/dir1/dir2/dir3", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir2/dir3\".\n");
        return -1;
    }

     //create a nested directory
    if (fs_create("/dir1/dir2/dir4", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir2/dir4\".\n");
        return -1;
    }

    //create a nested directory
    if (fs_create("/dir1/dir2/dir5", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir2/dir5\".\n");
        return -1;
    }

    if (fs_create("/dir1/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/file1\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir2/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/dir2/file2\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir2/dir3/file3", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/dir2/dir3/file3\".\n");
        return -1;
    }

    int fd = open("test/dump/rm_dir2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666); 

    if (fd == -1)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    if (dup2(fd, 1) == -1)
    {
        printf("\tERROR: Could not redirect STDOUT.\n");
        return -1;
    }

     // List the directory.
    if (fs_list("/dir1/dir2") == -1) 
    {
        printf("\tERROR: Could not list root directory.\n");
        return -1;
    }

    // Flush STDOUT to ensure all data is written to the file.
    fflush(stdout);

    // Close STDOUT.
    close(1);

    // Open STDOUT.
    open("/dev/tty", O_WRONLY);

    // Match the output.
    if (match_list_outputs("test/data/before_rm_dir2.txt", "test/dump/rm_dir2.txt") == -1)
    {
        printf("\tERROR: \"/\" Outputs do not match before removing.\n");
        return -1;
    }

    // Remove the dir4
    if (fs_remove("/dir1/dir2/dir4") == -1)
    {
        printf("\tERROR: Could not remove file.\n");
        return -1;
    }

    fd = open("test/dump/rm_dir2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd == -1)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    if (dup2(fd, 1) == -1)
    {
        printf("\tERROR: Could not redirect STDOUT.\n");
        return -1;
    }

    // List the new dir
    if (fs_list("/dir1/dir2") == -1) 
    {
        printf("\tERROR: Could not list root directory.\n");
        return -1;
    }

    // Flush STDOUT to ensure all data is written to the file.
    fflush(stdout);

    // Close STDOUT.
    close(1);

    // Open STDOUT.
    open("/dev/tty", O_WRONLY);

    // Match the output.
    if (match_list_outputs("test/data/after_rm_dir2.txt", "test/dump/rm_dir2.txt") == -1)
    {
        printf("\tERROR: \"/\" Outputs do not match after removing.\n");
        return -1;
    }

     // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    // // Compare two disks.
    // if (system("cmp test/data/rm_dir2.txt test/dump/rm_dir2.txt") != 0)
    // {
    //     printf("\tERROR: Outputs do not match.\n");
    //     return -1;
    // }

    return 0;
}





int rm_test_three() 
{

    /*
    TEST SUMMARY:
    1. Creating the following structure:

    "/"
     |- dir1                        16384
         |- dir11                   8192
             |- file11              0
             |- dir111              4096
                 |- file111         0
         |- dir12                   4096
     |- dir2                        4096
     |- dir3                        4096
         |- file3                   0

    2. Remove dir11

    "/"
     |- dir1                        8192
         |- dir12                   4096
     | - dir2                       4096
     | - dir3                       4096
         |- file3                   0

    3. List "/"

    Expected Output:
    dir1 8192
    dir2 4096
    dir3 4096
    */


    // Initialize the disk.
    if (disk_init("test/images/user/create.img", 512) == -1)
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

    if (fs_create("/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir11", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir11\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir11/file11", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/dir11/file11\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir11/dir111", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir11/dir111\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir11/dir111/file111", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/dir11/dir111/file111\".\n");
        return -1;
    }

    if (fs_create("/dir1/dir12", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1/dir12\".\n");
        return -1;
    }

    if (fs_create("/dir2", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir2\".\n");
        return -1;
    }

    if (fs_create("/dir3", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir3\".\n");
        return -1;
    }

    if (fs_create("/dir3/file3", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir3/file3\".\n");
        return -1;
    }

    if (fs_remove("/dir1/dir11") == -1)
    {
        printf("\tERROR: Could not remove directory: \"/dir1/dir11\".\n");
        return -1;
    }

    int fd = open("test/dump/rm_dir11.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (fd == -1)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    if (dup2(fd, 1) == -1)
    {
        printf("\tERROR: Could not redirect STDOUT.\n");
        return -1;
    }

    if (fs_list("/") == -1)
    {
        printf("\tERROR: Could not list root directory.\n");
        return -1;
    }

    // Flush STDOUT to ensure all data is written to the file.
    fflush(stdout);

    // Close STDOUT.
    close(1);

    // Open STDOUT.
    open("/dev/tty", O_WRONLY);

    // Match the output.
    if (match_list_outputs("test/data/rm_dir11.txt", "test/dump/rm_dir11.txt") == -1)
    {
        printf("\tERROR: \"/\" Outputs do not match after removing.\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    return 0;
}


int main()
{
    printf("\tTesting fs_remove()...\n");

    int total = 3;
    int passed = 0;

    if (duplicate_test() != -1)// duplicate test of create must pass for this to pass
    {
        
        if (rm_test_one())
        {
            printf("\tERROR: Remove test one failed.\n");
            fs_unmount();
        }
        else
        {
            printf("\t✅ Test Passed: Remove File.\n");
            passed+=1;
        }

        if (rm_test_two())
        {
            printf("\tERROR: Remove test two failed.\n");
            fs_unmount();
        }
        else
        {
            printf("\t✅ Test Passed: Remove Directory.\n");
            passed+=1;
        }

        if (rm_test_three())
        {
            printf("\tERROR: Remove test three failed.\n");
            fs_unmount();
        }
        else
        {
            printf("\t✅ Test Passed: Remove Directory with files.\n");
            passed+=1;
        }
    }
    else
    {
        printf("\t❌ ERROR: Prerequisite [Duplicate Test] failed.\n");
        fs_unmount();
        passed = 0;
    }

    printf("\t%d/%d Remove test(s) passed.\n", passed, total);
    printf("\033[1;33m\tTotal Marks: %d/%d\n\033[0m", passed * 5, total * 5);

    // system("rm test/dump/*");

    return 0;
}
