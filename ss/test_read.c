#include "fs.h"
#include "disk.h"

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int copy_out(char *fs_path, char *local_path)
{
    // Open the local file.
    FILE *local_file = fopen(local_path, "a");

    if (local_file == NULL)
    {
        printf("\tERROR: Could not open local file.\n");
        return -1;
    }

    // Create a buffer for the FS file.
    char *fs_file_buffer = calloc(BLOCK_SIZE, sizeof(char));

    if (fs_file_buffer == NULL)
    {
        printf("\tERROR: Could not allocate buffer for FS file.\n");
        return -1;
    }

    off_t offset = 0;
    while (1)
    {
        // Read the FS file into the buffer.
        int bytes_read = fs_read(fs_path, fs_file_buffer, BLOCK_SIZE, offset);

        if (bytes_read == -1)
        {
            printf("\tERROR: Could not read FS file into buffer.\n");
            return -1;
        }

        // Write the buffer to the local file.
        if ((int)fwrite(fs_file_buffer, sizeof(char), bytes_read, local_file) != bytes_read)
        {
            printf("\tERROR: Could not write buffer to local file.\n");
            return -1;
        }

        // If we read less than BLOCK_SIZE bytes, we've reached the end of the file.
        if (bytes_read < BLOCK_SIZE)
        {
            break;
        }

        // Otherwise, continue reading.
        offset += bytes_read;
    }

    // Close the local file.
    if (fclose(local_file) == EOF)
    {
        printf("\tERROR: Could not close local file.\n");
        return -1;
    }

    // Free the FS file buffer.
    free(fs_file_buffer);

    return 0;
}

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


bool is_metadata_line(const char *line) {
    // Check for common metadata fields in PDFs.
    const char *metadata_keywords[] = {
        "/Producer", "/Creator", "/CreationDate", "/ModDate", "/Author", "/Title"
    };

    for (size_t i = 0; i < sizeof(metadata_keywords) / sizeof(metadata_keywords[0]); i++) // gives ptrs to elements in arr
    {
        if (strstr(line, metadata_keywords[i]) != NULL)// checks if metadata_keywords[i] is a substring of line
        {
            return true;
        }
    }
    return false;
}

void extract_pdf_content(FILE *file, FILE *output) {
    char buffer[4096];
    bool in_stream = false;

    // Note that the actual contents of the pdf file are in between the "stream" and "endstream" lines
    // so we need to skip the metadata lines outside of the streams
    // we are only interested in the content of the streams rather than metadata (dates , authors etc)
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, "stream") != NULL) {
            in_stream = true;
            continue; // Skip the "stream" line
        }
        if (strstr(buffer, "endstream") != NULL) {
            in_stream = false;
            continue; // Skip the "endstream" line
        }

        // Skip metadata lines outside of streams
        if (!in_stream && is_metadata_line(buffer)) {
            continue;
        }

        // Write content to the output file
        if (in_stream) {
            fputs(buffer, output);
        }
    }
}

bool are_files_equal(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (!f1 || !f2) {
        printf("\tError opening one of the files.\n");
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return false;
    }

    // Create temporary files to store extracted content
    FILE *temp1 = tmpfile();
    FILE *temp2 = tmpfile();

    if (!temp1 || !temp2) {
        printf("\tError creating temporary files.\n");
        fclose(f1);
        fclose(f2);
        return false;
    }

    // Extract content without metadata
    extract_pdf_content(f1, temp1);
    extract_pdf_content(f2, temp2);

    fclose(f1);
    fclose(f2);

    // Compare the extracted content
    rewind(temp1);
    rewind(temp2);

    char ch1, ch2;
    bool is_equal = true;

    while (true) {
        ch1 = fgetc(temp1);
        ch2 = fgetc(temp2);

        if (ch1 != ch2) {
            is_equal = false;
            break;
        }

        if (ch1 == EOF && ch2 == EOF) {
            break;
        }

        if (ch1 == EOF || ch2 == EOF) {
            is_equal = false;
            break;
        }
    }

    fclose(temp1);
    fclose(temp2);

    return is_equal;
}


int basic_read_test()
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

    // Create a buffer of 128 bytes.
    char* buffer = (char*) malloc(128);
    memset(buffer, 'A', 128);
    
    //crate file1
    if (fs_create("/file1", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file1\".\n");
        return -1;
    }

    // Write the buffer to the file.
    if (fs_write("/file1", buffer, 128, 0) == -1)
    {
        printf("\tERROR: Could not write to file: \"/file1\".\n");
        return -1;
    }


    //now read the same file into another buffer
    char* read_buffer = (char*) malloc(128);

    if (fs_read("/file1", read_buffer, 128, 0) == -1)
    {
        printf("\tERROR: Could not read from file: \"/file1\".\n");
        return -1;
    }

    //compare the two buffers
    if (memcmp(buffer, read_buffer, 128) != 0)
    {
        printf("\tERROR: Write Buffers and Read Buffers do not match.\n");
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
    // if (system("cmp test/data/test_1.txt test/dump/test_1.txt") != 0)
    // {
    //     printf("\tERROR: Disk images do not match.\n");
    //     return -1;
    // }

    return 0;
}

int basic_read_test_2()
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

    // Create a buffer of 4096 + 512 bytes.// takes 2 blocks
    char* buffer = (char*) malloc(4096+512);
    memset(buffer, 'A', 4096+512);
    
    //crate file1
    if (fs_create("/file2", 0) == -1)
    {
        printf("\tERROR: Could not create file: \"/file2\".\n");
        return -1;
    }

    // Write the buffer to the file.
    if (fs_write("/file2", buffer, 4096+512, 0) == -1)
    {
        printf("\tERROR: Could not write to file: \"/file2\".\n");
        return -1;
    }


    //now read the same file into another buffer
    char* read_buffer = (char*) malloc(4096+512);

    if (fs_read("/file2", read_buffer, 4096+512, 0) == -1)
    {
        printf("\tERROR: Could not read from file: \"/file1\".\n");
        return -1;
    }

    //compare the two buffers
    if (memcmp(buffer, read_buffer, 4096+512) != 0)
    {
        printf("\tERROR: Write Buffers and Read Buffers do not match.\n");
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
    // if (system("cmp test/data/test_1.txt test/dump/test_1.txt") != 0)
    // {
    //     printf("\tERROR: Disk images do not match.\n");
    //     return -1;
    // }

    return 0;
}



int medium_read_test()// testing the appended data
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
    char *str1 = "I have enjoyed the OS course.";
    char *str2 = " I have learned a lot about file systems.";
    char *combined_str = "I have enjoyed the OS course. I have learned a lot about file systems.";

    // Write the first string to the file. // the file donot exist in first place so it must be created
    if (fs_write("/dir1/dir2/file1", str1, strlen(str1), 0) == -1)
    {
        printf("\tERROR: Could not write to file: \"/dir1/dir2/file1\".\n");
        return -1;
    }

    // Append the second string to the file.
    if (fs_write("/dir1/dir2/file1", str2, strlen(str2), 1) == -1)
    {
        printf("\tERROR: Could not append to file: \"/dir1/dir2/file1\".\n");
        return -1;
    }

    //now read the stored data into a buffer str
    char* read_buffer = (char*) malloc(strlen(combined_str));

    if (fs_read("/dir1/dir2/file1", read_buffer, strlen(combined_str), 0) == -1)
    {
        printf("\tERROR: Could not read from file: \"/dir1/dir2/file1\".\n");
        return -1;
    }

    //compare the two buffers
    if (memcmp(combined_str, read_buffer, strlen(combined_str)) != 0)
    {
        printf("\tERROR: Write Buffers and Read Buffers do not match.\n");
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
    // if (system("cmp test/data/random.pdf test/dump/random.pdf") != 0)
    // {
    //     printf("\tERROR: Disk images do not match.\n");
    //     return -1;
    // }

    return 0;
}

int advanced_read_test()
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

    // //now read the file into buffer from fs
    // char* read_buffer = (char*) malloc(file_size);

    //now read the file via copy_out
    if (copy_out("/dir1/file1", "test/dump/read_test2.pdf"))
    {
        printf("\tERROR: Could not copy out file.\n");
        return -1;
    }

    //now compare the contents of both files line by line
    const char *file1 = "test/data/write_test3a.pdf";
    const char *file2 = "test/dump/read_test2.pdf";

    if (are_files_equal(file1, file2))
     {
        // printf("\t PDF files are equal (ignoring metadata).\n");
    } else {
        printf("\tERROR: PDF files are not equal.\n");
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
    // if (system("cmp test/data/test_2.txt test/dump/test_2.txt") != 0)
    // {
    //     printf("\tERROR: Disk images do not match.\n");
    //     return -1;
    // }
    return 0;
}

int main()
{
    printf("\tTesting fs_read()...\n");

    double total = 4;
    double passed = 0;

    if (basic_read_test())
    {
        printf("\t❌ Test Failed: Basic Read.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Basic Read.\n");
        passed += 1;
    }

    if (basic_read_test_2())
    {
        printf("\t❌ Test Failed: Basic Read 2.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Basic Read 2.\n");
        passed += 1;
    }

    if (medium_read_test())
    {
        printf("\t❌ Test Failed: Medium Read.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Medium Read.\n");
        passed += 1;
    }

    if (advanced_read_test())
    {
        printf("\t❌ Test Failed: Advanced Read.\n");
        fs_unmount();
    }
    else
    {
        printf("\t✅ Test Passed: Advanced Read.\n");
        passed += 1;
    }

    // system("rm test/dump/*");

    printf("\t%.0f/%.0f Read test(s) passed.\n", passed, total);
    printf("\033[1;33m\tTotal Marks: %.0f/%.0f\n\033[0m", passed * 7.5, total * 7.5);

    return 0;
}