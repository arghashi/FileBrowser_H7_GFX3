#ifndef INC_FILE_BROWSER_H_
#define INC_FILE_BROWSER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
#include "ff.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PATH_LEN 256
#define MAX_READ_BUFF_SIZE 1024
#define MAX_ITEMS 100

typedef struct {
    char name[MAX_PATH_LEN];
    uint32_t size;
    bool is_dir;
    int item_count;
} FileInfo;

typedef enum {
    MSG_TYPE_INFO,       // پیغام اطلاعاتی
    MSG_TYPE_SUCCESS,    // پیغام موفقیت
    MSG_TYPE_ERROR,      // پیغام خطا
    MSG_TYPE_CONFIRM     // پیغام تاییدیه (برای عملیات حساس)
} MessageType;

typedef struct {
    MessageType msg_type;
    char text[256];      // متن پیغام
    uint32_t confirm_id; // شناسه منحصربفرد برای پیغام‌های تاییدیه
} SystemMessage;

typedef struct {
    char path[256];
    uint32_t totalKB;
    uint32_t usedKB;
    uint32_t freeKB;
} SystemInfo;

typedef enum {
    CMD_NEW_FOLDER,
    CMD_DELETE_ITEM,
    CMD_NEW_FILE,
    CMD_RENAME_ITEM,
    CMD_GET_STORAGE_INFO,
    CMD_FORMAT_SD,
    CMD_READ_FILE,
    CMD_WRITE_FILE,
    CMD_ENTER_FOLDER,
    CMD_EXIT_FOLDER,
    CMD_GO_TO_ROOT
} FileCommandType;

typedef struct {
    FileCommandType type;
    char name[256];     // برای نام فایل/پوشه
    char newName[256];  // برای تغییر نام
    char data[1024];    // برای نوشتن داده در فایل
    bool append;        // حالت نوشتن (افزودن یا بازنویسی)
} FileCommand;

// Queue declarations
extern osMessageQueueId_t fileInfoQueueHandle;
extern osMessageQueueId_t pathQueueHandle;
extern osMessageQueueId_t storageUsageQueueHandle;
extern osMessageQueueId_t fileContentQueueHandle;
extern osMessageQueueId_t systemMessageQueueHandle;
extern osMessageQueueId_t fileCommandQueueHandle;

// API Functions
void SD_init(void);
FRESULT list_dir(const char *path);
void enterFolder(const char *folderName);
void exitFolder(void);
void goToRoot(void);
void formatSD_Card(void);
void createFolder(const char *folderName);
void deleteItem(const char *path);
FRESULT createFile(const char *fileName);
FRESULT writeToFile(const char *fileName, const char *data, bool append);
FRESULT readFile(const char *fileName);
FRESULT renameItem(const char *oldName, const char *newName);
void getCurrentPath(char *buffer, size_t size);
void getStorageInfo(void);
//void getCurrentPath(char *buffer, size_t size); // تابع گمشده
void handleFileOperations(void *argument);
void sendSystemMessage(MessageType type, const char *format, ...);
uint32_t sendConfirmationMessage(const char *format, ...);


#ifdef __cplusplus
}
#endif

#endif /* INC_FILE_BROWSER_H_ */
