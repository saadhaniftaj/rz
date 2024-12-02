#include "fs.h"
#include "disk.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <zlib.h>


int copy_in(char *local_path, char *fs_path)
{
    // Open the local file.
    FILE *local_file = fopen(local_path, "r");

    if (local_file == NULL)
    {
        printf("\tERROR: Could not open local file.\n");
        return -1;
    }

    // Get the size of the local file.
    fseek(local_file, 0, SEEK_END);
    int local_file_size = ftell(local_file);
    fseek(local_file, 0, SEEK_SET);

    // Allocate a buffer for the local file.
    char *local_file_buffer = calloc(local_file_size, sizeof(char));

    if (local_file_buffer == NULL)
    {
        printf("\tERROR: Could not allocate buffer for local file.\n");
        return -1;
    }
    // Read the local file into the buffer.
    if ((int)fread(local_file_buffer, sizeof(char), local_file_size, local_file) != local_file_size)
    {
        printf("\tERROR: Could not read local file into buffer.\n");
        return -1;
    }

    // Close the local file.
    if (fclose(local_file) == EOF)
    {
        printf("\tERROR: Could not close local file.\n");
        return -1;
    }

    // Write the local file buffer to the FS file.
    if (fs_write(fs_path, local_file_buffer, local_file_size, 0) == -1)
    {
        printf("\tERROR: Could not write local file buffer to FS file.\n");
        return -1;
    }

    // Free the local file buffer.
    free(local_file_buffer);

    return 0;
}


int match_list_outputs(const char *actual_list_file, const char *output_list_file) {
    FILE *actual_list = fopen(actual_list_file, "r");
    FILE *output_list = fopen(output_list_file, "r");

    if (actual_list == NULL || output_list == NULL) {
        perror("\tError opening file");
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
            printf("\tFile names do not match: %s %s\n", actual_name, output_name);
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


int basic_write_test() //checks write via file sizes
{
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

    // Create a buffer of 16 bytes.
    char* buffer = (char*) malloc(16);
    memset(buffer, 'A', 16);
    
    //crate file1
    if (fs_create("/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file1\".\n");
        return -1;
    }

    // Write the buffer to the file.
    if (fs_write("/file1", buffer, 16, 0) == -1)
    {
        printf("\tERROR: Could not write to file: \"/file1\".\n");
        return -1;
    }

    //create dir1
    if (fs_create("/dir1", 1) == -1)
    {
        printf("\tERROR: Could not create directory: \"/dir1\".\n");
        return -1;
    }

    //create file2 in dir1
    if (fs_create("/dir1/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/dir1/file2\".\n");
        return -1;
    }

    // Resize the buffer to 128 bytes.
    buffer = realloc(buffer, 128);
    memset(buffer, 'B', 128);

    // Write the new buffer to the file.
    if (fs_write("/dir1/file2", buffer, 128, 0) == -1)
    {
        printf("\tERROR: Could not write to file: \"/dir1/file2\".\n");
        return -1;
    }

    // List the root directory to check sizes.
    // Redirect STDOUT.
    int fd = open("test/dump/write_test1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

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
    if (match_list_outputs("test/data/write_test1.txt", "test/dump/write_test1.txt") == -1)
    {
        printf("\tERROR: Outputs do not match.\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    // Free the buffer.
    free(buffer);

    // // Remove the disk image.
    // if (system("rm write.img") == -1)
    // {
    //     printf("\tERROR: Could not remove disk image.\n");
    //     return -1;
    // }

    return 0;
}


// this tests writes out two files "write_test2a.txt" and "write_test2b.txt" and checks the sizes of the files
int intermediate_write_test()
{

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

    //create file1
    if (fs_create("/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file1\".\n");
        return -1;
    }

    // Copy in the file.
    //this function copies the contents of the file at local_path to the file at fs_path
    if (copy_in("test/data/write_test2a.txt", "/file1") == -1)
    {
        printf("\tERROR: Could not copy in file: \"/file1\".\n");
        return -1;
    }   


    FILE *file = fopen("test/data/write_test2a.txt", "r");
    if (file == NULL)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    // Get the size of the file.
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // SIZE: 3689  -> Block size: 4096  -> Required blocks: 1

    if (fs_create("/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file2\".\n");
        return -1;
    }

    // Write another file.
    if (copy_in("test/data/write_test2b.txt", "/file2") == -1)
    {
        printf("\tERROR: Could not copy in file: \"/file2\".\n");
        return -1;
    }

    file = fopen("test/data/write_test2b.txt", "r");
    if (file == NULL)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    // Get the size of the file.
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // SIZE: 18655  -> Block size: 4096  -> Required blocks: 5

    // List the root directory to check sizes.
    // Redirect STDOUT.
    int fd = open("test/dump/write_test2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

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
    if (match_list_outputs("test/data/write_test2.txt", "test/dump/write_test2.txt") == -1)
    {
        printf("\tERROR: Outputs do not match.\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    // // Remove the disk image.
    // if (system("rm write.img") == -1)
    // {
    //     printf("\tERROR: Could not remove disk image.\n");
    //     return -1;
    // }

    return 0;
}


//this tests writing a file large pdf file and checks the sizes of the files
int advanced_write_test()
{

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

    // Copy in the file.// without creating the file in the first place
    if (copy_in("test/data/write_test3a.pdf", "/dir1/file1") == -1)
    {
        printf("\tERROR: Could not copy in file: \"/dir1/file1\".\n");
        return -1;
    }

    // open the file
    FILE *file = fopen("test/data/write_test3a.pdf", "r");
    if (file == NULL)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    // Get the size of the file.
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // SIZE: 2,515,914  -> Block Size: 4096  -> Required Blocks: 615

    // List the root directory to check sizes.
    // Redirect STDOUT.
    int fd = open("test/dump/write_test3.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

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

    // Match the output. Should just be: "dir1 2520010"
    // Open the file.
    FILE *output = fopen("test/dump/write_test3.txt", "r");

    if (output == NULL)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    char name[256];
    int size;

    // Read the file.
    if (fscanf(output, "%s %d", name, &size) == EOF)
    {
        printf("\tERROR: Could not read file.\n");
        return -1;
    }

    // Check if the file is correct.
    if (strcmp(name, "dir1") != 0)
    {
        printf("\tERROR: File name is incorrect.\n");
        return -1;
    }
    if (size != 2520010) // == 2515914 + 4096 = 2520010
    {
        printf("\tERROR: File size is incorrect.\n");
        return -1;
    }

    // Close the file.
    if (fclose(output) == EOF)
    {
        printf("\tERROR: Could not close file.\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    // // Remove the disk image.
    // if (system("rm write.img") == -1)
    // {
    //     printf("\tERROR: Could not remove disk image.\n");
    //     return -1;
    // }

    return 0;
}


int append_test()
{

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

    // Write a string to a file.
    char *str1 = "Hello, World!";
    char *str2 = " This is appended data.";
    char *combined_str = "Hello, World! This is appended data.";

    // uint32_t str1_checksum = crc32(0L, (const unsigned char*) str1, strlen(str1));
    // uint32_t combined_checksum = crc32(0L, (const unsigned char*) combined_str, strlen(combined_str));

    // Write the first string to the file. // the file donot exist in first place so it must be created
    if (fs_write("/dir1/dir2/file1", str1, strlen(str1), 0) == -1)
    {
        printf("\tERROR: Could not write to file: \"/dir1/dir2/file1\".\n");
        return -1;
    }

    // Get the checksum of the file.
    // uint32_t fs_checksum = get_checksum("/dir1/dir2/file1");

    // Check if the checksums match.
    // if (str1_checksum != fs_checksum)
    // {
    //     printf("\tERROR: Checksums do not match for file: \"/dir1/dir2/file1\".\n");
    //     return -1;
    // }

    // Append the second string to the file.
    if (fs_write("/dir1/dir2/file1", str2, strlen(str2), 1) == -1)
    {
        printf("\tERROR: Could not append to file: \"/dir1/dir2/file1\".\n");
        return -1;
    }

    // Get the checksum of the file.
    // fs_checksum = get_checksum("/dir1/dir2/file1");

    // Check if the checksums match.
    // if (combined_checksum != fs_checksum)
    // {
    //     printf("\tERROR: Checksums do not match for file: \"/dir1/dir2/file1\".\n");
    //     return -1;
    // }

    // List the root directory to check sizes.
    // Redirect STDOUT.
    int fd = open("test/dump/write_test4.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

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
    // SHould be: "dir1 8228"

    // Open the file.
    FILE *output = fopen("test/dump/write_test4.txt", "r");

    if (output == NULL)
    {
        printf("\tERROR: Could not open file.\n");
        return -1;
    }

    char name[256];
    int size;

    // Read the file.
    if (fscanf(output, "%s %d", name, &size) == EOF)
    {
        printf("\tERROR: Could not read file.\n");
        return -1;
    }

    // Check if the file is correct.
    if (strcmp(name, "dir1") != 0)
    {
        printf("\tERROR: File name is incorrect.\n");
        return -1;
    }

    if (size != 8228)
    {
        printf("\tERROR: File size is incorrect.\n");
        return -1;
    }

    // Close the file.
    if (fclose(output) == EOF)
    {
        printf("\tERROR: Could not close file.\n");
        return -1;
    }

    // Close the disk.
    if (disk_close(0) == -1)
    {
        printf("\tERROR: Could not close disk.\n");
        return -1;
    }

    fs_unmount();

    // // Remove the disk image.
    // if (system("rm write.img") == -1)
    // {
    //     printf("\tERROR: Could not remove disk image.\n");
    //     return -1;
    // }
    
    return 0;
}

int main()
{
    int total = 4;
    int passed = 0;
    printf("\tTesting fs_write()...\n");
    if (basic_write_test() == -1)
    {
        printf("\t❌ Test Failed: Basic Write.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Basic Write.\n");
        passed += 1;
    }

    if (intermediate_write_test() == -1)
    {
        printf("\t❌ Test Failed: Intermediate Write.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Intermediate Write.\n");
        passed += 1;
    }

    if (advanced_write_test() == -1)
    {
        printf("\t❌ Test Failed: Advanced Write.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Advanced Write.\n");
        passed += 1;
    }

    if (append_test() == -1)
    {
        printf("\t❌ Test Failed: Append.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Append.\n");
        passed += 1;
    }

    printf("\t%d/%d Write test(s) passed.\n", passed, total);
    printf("\033[1;33m\tTotal Marks: %d/%d\n\033[0m", passed * 4, total * 4);

    return 0;
}