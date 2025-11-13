#include <classes/Class_Include.hpp>

namespace n_storage {

    double get_free_storage()
    {
        wchar_t buffer[512];
        //get strings for drives and prepare a buffer
        DWORD len = GetLogicalDriveStringsW(511, buffer);

        if (len == 0)
            return -1.0;// if it cant find any we return -1

        double totalFreeBytes = 0.0;
        wchar_t* drive = buffer;
        while (*drive)
            //iterate trough the drive strings and retrieve the free diskspace
        {
            ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytesForDrive;
            if (GetDiskFreeSpaceExW(drive, &freeBytesAvailable, &totalBytes, &totalFreeBytesForDrive))
            {
                totalFreeBytes += static_cast<double>(totalFreeBytesForDrive.QuadPart);
            }
            // move to the next drivestring and add the free space to the last one
            drive += wcslen(drive) + 1;
        }
        return totalFreeBytes / 1'000'000'000.0;// we return the result in gb, decimal
    }


}