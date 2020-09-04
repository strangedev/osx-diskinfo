#include <stdio.h>
#include <sys/disk.h>
#include <sys/ioctl.h>
#include <fcntl.h>

typedef struct
{
    char const* device_path;
    uint32_t block_size;
    uint64_t block_count;
    uint64_t size;
} DiskInfo;

int getDiskInfo(DiskInfo* disk_info, char const* device_path)
{
    uint32_t dev = open(device_path, O_RDONLY);

    if (dev == -1) {
        perror("Failed to open disk");
        return 1;
    }

    disk_info->device_path = device_path;
    ioctl(dev, DKIOCGETBLOCKCOUNT, &disk_info->block_count);
    ioctl(dev, DKIOCGETBLOCKSIZE, &disk_info->block_size);
    disk_info->size = disk_info->block_size * disk_info->block_count;

    return 0;
}

void printDiskInfo(DiskInfo const* disk_info)
{
    printf("Device Path: %s\n", disk_info->device_path);
    printf("Block Size: %u\n", disk_info->block_size);
    printf("Block Count: %llu\n", disk_info->block_count);
    printf("Size: %llu\n", disk_info->size);
    printf("Overwrite CMD: sudo dd if=/dev/urandom of=%s bs=%u count=%llu\n", disk_info->device_path, disk_info->block_size, disk_info->block_count);
}

typedef struct
{
    char const* device_path;
} Args;

int parseArgs(Args* args, int const argc, char const** argv)
{
    if (argc != 2){
        perror("Failed to parse args");
        return 1;
    }
    args->device_path = argv[1];
    return 0;
}

int main(int const argc, char const** argv)
{
    Args args;
    if (parseArgs(&args, argc, argv))
        return 1;
    
    DiskInfo disk_info;
    if (getDiskInfo(&disk_info, args.device_path))
        return 1;

    printDiskInfo(&disk_info);
    return 0;
}
