#include "file_browser.h"
#include "cmsis_os2.h"
#include "fatfs.h"
#include <stdarg.h> // برای استفاده از va_list

// External queues
extern osMessageQueueId_t fileInfoQueueHandle;
extern osMessageQueueId_t pathQueueHandle;
extern osMessageQueueId_t storageUsageQueueHandle;
extern osMessageQueueId_t fileContentQueueHandle;
extern osMessageQueueId_t systemMessageQueueHandle;

// Static variables
static FATFS fs;
static char currentPath[MAX_PATH_LEN] = "/";
static char pathStack[10][MAX_PATH_LEN];
static int stackPointer = -1;

// تابع جدید برای ساخت مسیر صحیح
static void buildFullPath(char* fullPath, const char* name) {
    if (currentPath[strlen(currentPath) - 1] == '/') {
        snprintf(fullPath, MAX_PATH_LEN, "%s%s", currentPath, name);
    } else {
        snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, name);
    }
}

// Push directory to stack
static void pushPath(const char *path) {
    if (stackPointer < 9) {
        stackPointer++;
        strncpy(pathStack[stackPointer], path, MAX_PATH_LEN - 1);
        pathStack[stackPointer][MAX_PATH_LEN - 1] = '\0';
    }
}

// Pop directory from stack
static void popPath(void) {
    if (stackPointer >= 0) {
        stackPointer--;
    }
}

// Update current path
static void updateCurrentPath(void) {
    if (stackPointer >= 0) {
        snprintf(currentPath, MAX_PATH_LEN, "%s", pathStack[stackPointer]);
    } else {
        strncpy(currentPath, "/", MAX_PATH_LEN);
    }

    // Send path to UI
    if (osMessageQueuePut(pathQueueHandle, currentPath, 0, 100) != osOK) {
        // Handle queue full error
    }
}

// Initialize SD Card
//void SD_init(void) {
//	FATFS drive;
//    FRESULT res = f_mount(&drive, "", 1);
//    if (res != FR_OK) {
////        const char *errorMsg = "SD Card mount failed!";
////        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
//        sendSystemMessage(MSG_TYPE_ERROR,
//            "SD Card mount failed! Error: %02d", res);
//    } else {
//        pushPath("/");
//        updateCurrentPath();
//        list_dir("/");
//        sendSystemMessage(MSG_TYPE_SUCCESS,
//            "SD Card mounted successfully");
//    }
//}

void SD_init(void) {
    FRESULT res = f_mount(&fs, "", 1); // استفاده از fs ثابت
    if (res != FR_OK) {
        sendSystemMessage(MSG_TYPE_ERROR,
            "SD Card mount failed! Error: %02d", res);
    } else {
        stackPointer = -1;
        pushPath("/");
        updateCurrentPath();
        list_dir("/");
        sendSystemMessage(MSG_TYPE_SUCCESS,
            "SD Card mounted successfully");
    }
}

// List directory contents
FRESULT list_dir(const char *path) {
    DIR dir;
    FILINFO fno;
    FileInfo fileInfo;
    uint16_t itemCount = 0;

    FRESULT res = f_opendir(&dir, path);
    if (res != FR_OK) {
        return res;
    }

    // Clear previous items
    osMessageQueueReset(fileInfoQueueHandle);

    // Add ".." for parent directory
    if (stackPointer > 0) {
        strncpy(fileInfo.name, "..", MAX_PATH_LEN);
        fileInfo.size = 0;
        fileInfo.is_dir = true;
        osMessageQueuePut(fileInfoQueueHandle, &fileInfo, 0, 100);
        itemCount++;
    }

    // Read directory contents
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;

        // Skip hidden files
        if (fno.fname[0] == '.') continue;

        strncpy(fileInfo.name, fno.fname, MAX_PATH_LEN - 1);
        fileInfo.name[MAX_PATH_LEN - 1] = '\0';
        fileInfo.size = fno.fsize;
        fileInfo.is_dir = (fno.fattrib & AM_DIR) ? true : false;
        fileInfo.item_count = itemCount;

        if (osMessageQueuePut(fileInfoQueueHandle, &fileInfo, 0, 100) != osOK) {
            break; // Queue full
        }

        itemCount++;

        if (itemCount >= MAX_ITEMS) break;
    }

    f_closedir(&dir);
    return FR_OK;
}

// Enter folder
//void enterFolder(const char *folderName) {
//    char newPath[MAX_PATH_LEN];
//
//    if (strcmp(folderName, "..") == 0) {
//        if (stackPointer > 0) {
//            popPath();
//        }
//    } else {
//        snprintf(newPath, MAX_PATH_LEN, "%s/%s", currentPath, folderName);
//        pushPath(newPath);
//    }
//
//    updateCurrentPath();
//    list_dir(currentPath);
//}

void enterFolder(const char *folderName) {
    char newPath[MAX_PATH_LEN];

    if (strcmp(folderName, "..") == 0) {
        if (stackPointer > 0) {
            popPath();
        }
    } else {
        buildFullPath(newPath, folderName); // استفاده از تابع جدید
        pushPath(newPath);
    }

    updateCurrentPath();
    list_dir(currentPath);
}

// Exit to parent folder
void exitFolder(void) {
    if (stackPointer <= 0) {
        // Already at root, nothing to do
        return;
    }

    popPath();
    updateCurrentPath();
    list_dir(currentPath);
}

// Go to root directory
void goToRoot(void) {
    while (stackPointer > 0) {
        popPath();
    }
    updateCurrentPath();
    list_dir("/");
}

// Create new folder
void createFolder(const char *folderName) {
//    char fullPath[MAX_PATH_LEN];
//    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, folderName);

    char fullPath[MAX_PATH_LEN];
    buildFullPath(fullPath, folderName); // استفاده از تابع جدید

    FRESULT res = f_mkdir(fullPath);
    if (res == FR_OK) {
        list_dir(currentPath);
//        const char *successMsg = "Folder created!";
//        osMessageQueuePut(successQueueHandle, &successMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_SUCCESS,
            "Folder created!");
    } else {
//        const char *errorMsg = "Create folder failed!";
//        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_ERROR,
            "Create folder failed! Error: %02d", res);
    }
}

// Delete item (file or folder)
void deleteItem(const char *path) {
//    char fullPath[MAX_PATH_LEN];
//    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, path);
    char fullPath[MAX_PATH_LEN];
    buildFullPath(fullPath, path); // استفاده از تابع جدید

    FILINFO fno;
    FRESULT res = f_stat(fullPath, &fno);
    if (res != FR_OK) {
//        const char *errorMsg = "Item not found!";
//        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_ERROR,
            "Item not found! Error: %02d", res);
        return;
    }

    if (fno.fattrib & AM_DIR) {
        res = f_unlink(fullPath);
    } else {
        res = f_unlink(fullPath);
    }

    if (res == FR_OK) {
        list_dir(currentPath);
//        const char *successMsg = "Item deleted!";
//        osMessageQueuePut(successQueueHandle, &successMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_SUCCESS,
            "Item deleted!");
    } else {
//        const char *errorMsg = "Delete failed!";
//        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_ERROR,
            "Delete failed! Error: %02d", res);
    }
}

// Create a new file
FRESULT createFile(const char *fileName) {
//    char fullPath[MAX_PATH_LEN];
//    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, fileName);
    char fullPath[MAX_PATH_LEN];
    buildFullPath(fullPath, fileName); // استفاده از تابع جدید

    FIL file;
    FRESULT res = f_open(&file, fullPath, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK) {
        f_close(&file);
//        const char *successMsg = "File created!";
//        osMessageQueuePut(successQueueHandle, &successMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_SUCCESS,
            "File created!");
        list_dir(currentPath); // Refresh the directory listing
    } else {
        const char *errorMsg;
        if (res == FR_EXIST) {
//            errorMsg = "File already exists!";
            sendSystemMessage(MSG_TYPE_ERROR, "File already exists! Error: %02d", res);
        } else {
//            errorMsg = "File creation failed!";
            sendSystemMessage(MSG_TYPE_ERROR,
                "File creation failed! Error: %02d", res);
        }
//        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
    }
    return res;
}

// Write data to a file (append or overwrite)
FRESULT writeToFile(const char *fileName, const char *data, bool append) {
//    char fullPath[MAX_PATH_LEN];
//    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, fileName);
    char fullPath[MAX_PATH_LEN];
    buildFullPath(fullPath, fileName); // استفاده از تابع جدید

    FIL file;
    FRESULT res;
    UINT bytesWritten;
//    const char *operationMsg;

    if (append) {
        res = f_open(&file, fullPath, FA_OPEN_APPEND | FA_WRITE);
//        operationMsg = "Data appended to file!";
        sendSystemMessage(MSG_TYPE_INFO, "Data appended to file!");
    } else {
        res = f_open(&file, fullPath, FA_CREATE_ALWAYS | FA_WRITE);
//        operationMsg = "File overwritten!";
        sendSystemMessage(MSG_TYPE_INFO, "File overwritten!");
    }

//    if (res != FR_OK) {
////        const char *errorMsg = "Failed to open file!";
////        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
//        sendSystemMessage(MSG_TYPE_ERROR, "Failed to open file! Error: %02d", res);
//        return res;
//    }
//
//    res = f_write(&file, data, strlen(data), &bytesWritten);
//    if (res != FR_OK || bytesWritten != strlen(data)) {
////        const char *errorMsg = "File write failed!";
////        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
//        sendSystemMessage(MSG_TYPE_ERROR, "File write failed! Error: %02d", res);
//    } else {
////        osMessageQueuePut(successQueueHandle, &operationMsg, 0, 0);
//    	sendSystemMessage(MSG_TYPE_INFO, "File overwritten!");
//    }
//
//    f_close(&file);
//    return res;
    if (res != FR_OK) {
        sendSystemMessage(MSG_TYPE_ERROR, "Failed to open file! Error: %02d", res);
        return res;
    }

    res = f_write(&file, data, strlen(data), &bytesWritten);
    if (res != FR_OK || bytesWritten != strlen(data)) {
        sendSystemMessage(MSG_TYPE_ERROR, "File write failed! Error: %02d", res);
    } else {
        // ارسال تنها یک پیام موفقیت
        sendSystemMessage(MSG_TYPE_SUCCESS,
            append ? "Data appended!" : "File overwritten!");
    }

    f_close(&file);
    return res;
}

// Read file content
//FRESULT readFile(const char *fileName) {
//    char fullPath[MAX_PATH_LEN];
//    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, fileName);
//
//    FIL file;
//    char buffer[MAX_READ_BUFF_SIZE];
//    UINT bytesRead;
//    bool eof = false;
//
//    FRESULT res = f_open(&file, fullPath, FA_READ);
//    if (res != FR_OK) {
////        const char *errorMsg = "File open failed!";
////        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
//        sendSystemMessage(MSG_TYPE_ERROR, "File open failed! Error: %02d", res);
//        return res;
//    }
//
//    // Send file name first
//    osMessageQueuePut(fileContentQueueHandle, fileName, 0, 100);
//
//    do {
//        res = f_read(&file, buffer, MAX_READ_BUFF_SIZE - 1, &bytesRead);
//        if (res != FR_OK || bytesRead == 0) {
//            eof = true;
//            break;
//        }
//
//        // Null-terminate the buffer
//        buffer[bytesRead] = '\0';
//
//        // Send chunk to queue
//        if (osMessageQueuePut(fileContentQueueHandle, buffer, 0, 100) != osOK) {
//            break; // Queue full
//        }
//
//    } while (bytesRead == MAX_READ_BUFF_SIZE - 1);
//
//    // Send EOF marker
//    const char *eofMarker = "EOF";
//    osMessageQueuePut(fileContentQueueHandle, eofMarker, 0, 100);
//
//    f_close(&file);
//    return FR_OK;
//}

FRESULT readFile(const char *fileName) {
    char fullPath[MAX_PATH_LEN];
    buildFullPath(fullPath, fileName); // استفاده از تابع جدید

    FIL file;
    char readBuffer[MAX_READ_BUFF_SIZE];
    UINT bytesRead;
    char sendBuffer[MAX_READ_BUFF_SIZE]; // بافر اختصاصی برای ارسال

    FRESULT res = f_open(&file, fullPath, FA_READ);
    if (res != FR_OK) {
        sendSystemMessage(MSG_TYPE_ERROR, "File open failed! Error: %02d", res);
        return res;
    }

    // ارسال نام فایل با کپی کردن
    strncpy(sendBuffer, fileName, sizeof(sendBuffer));
    osMessageQueuePut(fileContentQueueHandle, sendBuffer, 0, 100);

    while (1) {
        res = f_read(&file, readBuffer, sizeof(readBuffer) - 1, &bytesRead);
        if (res != FR_OK || bytesRead == 0) break;

        // کپی داده به بافر ارسال و افزودن null-terminator
        strncpy(sendBuffer, readBuffer, bytesRead);
        sendBuffer[bytesRead] = '\0';

        if (osMessageQueuePut(fileContentQueueHandle, sendBuffer, 0, 100) != osOK) {
            break;
        }
    }

    // ارسال مارکر EOF
    strcpy(sendBuffer, "EOF");
    osMessageQueuePut(fileContentQueueHandle, sendBuffer, 0, 100);

    f_close(&file);
    return FR_OK;
}

// Rename item (file or folder)
FRESULT renameItem(const char *oldName, const char *newName) {
//    char oldPath[MAX_PATH_LEN];
//    char newPath[MAX_PATH_LEN];
//    snprintf(oldPath, MAX_PATH_LEN, "%s/%s", currentPath, oldName);
//    snprintf(newPath, MAX_PATH_LEN, "%s/%s", currentPath, newName);
    char oldPath[MAX_PATH_LEN], newPath[MAX_PATH_LEN];
    buildFullPath(oldPath, oldName); // استفاده از تابع جدید
    buildFullPath(newPath, newName); // استفاده از تابع جدید

    FRESULT res = f_rename(oldPath, newPath);
    if (res == FR_OK) {
        list_dir(currentPath);
        const char *successMsg = "Item renamed!";
//        osMessageQueuePut(successQueueHandle, &successMsg, 0, 0);
        sendSystemMessage(MSG_TYPE_SUCCESS, "Item renamed!");
    } else {
//        const char *errorMsg = "Rename failed!";
//        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
    	sendSystemMessage(MSG_TYPE_ERROR, "Rename failed! Error: %02d", res);
    }
    return res;
}

// Format SD card
//void formatSD_Card(void) {
////    MKFS_PARM opt;
////    opt.fmt = FM_FAT32;
////    opt.n_fat = 1;
////    opt.align = 0;
////    opt.n_root = 0;
////    opt.au_size = 0;
//
//    BYTE work[_MAX_SS]; // Work area for formatting
//
//    FRESULT res = f_mkfs("", FM_FAT32, 0, work, sizeof(work));
//    if (res != FR_OK) {
////        const char *errorMsg = "Format failed!";
////        osMessageQueuePut(errorQueueHandle, &errorMsg, 0, 0);
//        sendSystemMessage(MSG_TYPE_ERROR, "Format failed! Error: %02d", res);
//    } else {
//        // Remount after format
//        f_mount(NULL, "", 0); // Unmount
//        osDelay(100);
//        f_mount(&fs, "", 1);  // Remount
//
//        // Reset directory stack
//        stackPointer = -1;
//        pushPath("/");
//        updateCurrentPath();
//        list_dir("/");
//
////        const char *successMsg = "SD card formatted!";
////        osMessageQueuePut(successQueueHandle, &successMsg, 0, 0);
//        sendSystemMessage(MSG_TYPE_SUCCESS, "SD card formatted!");
//    }
//}

void formatSD_Card(void) {
    BYTE work[_MAX_SS];

    // Unmount first
    f_mount(NULL, "", 0);

    FRESULT res = f_mkfs("", FM_FAT32, 0, work, sizeof(work));
    if (res != FR_OK) {
        sendSystemMessage(MSG_TYPE_ERROR, "Format failed! Error: %02d", res);
    } else {
        // Remount using the same fs object
        res = f_mount(&fs, "", 1);
        if (res == FR_OK) {
            stackPointer = -1;
            pushPath("/");
            updateCurrentPath();
            list_dir("/");
            sendSystemMessage(MSG_TYPE_SUCCESS, "SD formatted successfully!");
        } else {
            sendSystemMessage(MSG_TYPE_ERROR, "Remount failed! Error: %02d", res);
        }
    }
}

// پیاده‌سازی تابع گمشده
void getCurrentPath(char *buffer, size_t size) {
    strncpy(buffer, currentPath, size - 1);
    buffer[size - 1] = '\0';
}

// Get storage information
void getStorageInfo(void) {
    DWORD freeClusters, totalClusters;
    FATFS *fsPtr;

    FRESULT res = f_getfree("", &freeClusters, &fsPtr);
    if (res != FR_OK) {
        return;
    }

    uint32_t totalKB = ((fsPtr->n_fatent - 2) * fsPtr->csize) / 2;
    uint32_t freeKB = (freeClusters * fsPtr->csize) / 2;
    uint32_t usedKB = totalKB - freeKB;

    uint32_t storageInfo[3] = {totalKB, usedKB, freeKB};
    osMessageQueuePut(storageUsageQueueHandle, storageInfo, 0, 100);
}

// File operations task
void handleFileOperations(void *argument) {
    while (1) {
        // Handle file operations requests from GUI
        // This would depend on your specific implementation
        osDelay(10);
    }
}

// تابع ارسال پیغام (مشابه printf)
void sendSystemMessage(MessageType type, const char *format, ...) {
    SystemMessage msg;
    msg.msg_type = type;
    msg.confirm_id = 0; // پیش‌فرض برای پیغام‌های غیرتاییدیه

    va_list args;
    va_start(args, format);
    vsnprintf(msg.text, sizeof(msg.text), format, args);
    va_end(args);

    // ارسال به صف با اولویت بالا
    osMessageQueuePut(systemMessageQueueHandle, &msg, 0, osWaitForever);
}

// تابع ارسال پیغام تاییدیه (با شناسه منحصربفرد)
uint32_t sendConfirmationMessage(const char *format, ...) {
    static uint32_t confirm_counter = 1;
    SystemMessage msg;
    msg.msg_type = MSG_TYPE_CONFIRM;
    msg.confirm_id = confirm_counter++;

    va_list args;
    va_start(args, format);
    vsnprintf(msg.text, sizeof(msg.text), format, args);
    va_end(args);

    osMessageQueuePut(systemMessageQueueHandle, &msg, 0, osWaitForever);
    return msg.confirm_id;
}
