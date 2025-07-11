#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include "file_browser.h"
#include "main.h"
#include <cstring>

extern osMessageQueueId_t fileInfoQueueHandle;
extern osMessageQueueId_t pathQueueHandle;
extern osMessageQueueId_t storageUsageQueueHandle;
extern osMessageQueueId_t fileContentQueueHandle;
extern osMessageQueueId_t systemMessageQueueHandle;
extern osMessageQueueId_t fileCommandQueueHandle;


Model::Model() : modelListener(nullptr)
{
	SD_init();
}

void Model::tick()
{
    processFileCommands();
    updateFileList();
    updatePath();
    updateStorageInfo();
    processFileContent();
}

void Model::processFileCommands() {
    FileCommand cmd;
    if (osMessageQueueGet(fileCommandQueueHandle, &cmd, NULL, 0) == osOK) {
        switch (cmd.type) {
            case CMD_NEW_FOLDER:
                createFolder(cmd.name);
                break;
            case CMD_DELETE_ITEM:
                deleteItem(cmd.name);
                break;
            case CMD_NEW_FILE:
                createFile(cmd.name);
                break;
            case CMD_RENAME_ITEM:
                renameItem(cmd.name, cmd.newName);
                break;
            case CMD_GET_STORAGE_INFO:
                getStorageInfo();
                break;
            case CMD_FORMAT_SD:
                formatSDCard();
                break;
            case CMD_READ_FILE:
                readFile(cmd.name);
                break;
            case CMD_WRITE_FILE:
                writeToFile(cmd.name, cmd.data, cmd.append);
                break;
            case CMD_ENTER_FOLDER:
                enterFolder(cmd.name);
                break;
            case CMD_EXIT_FOLDER:
                exitFolder();
                break;
            case CMD_GO_TO_ROOT:
                goToRoot();
                break;
            default:
                break;
        }
    }
}

void Model::updateFileList() {
    FileInfo fileInfo;
    while (osMessageQueueGet(fileInfoQueueHandle, &fileInfo, NULL, 0) == osOK) {
//        if (modelListener) {
//            modelListener->addFileItem(fileInfo.name, fileInfo.size, fileInfo.is_dir);
        	 modelListener->updateFileList(fileInfo, fileInfo.item_count);//addFileItem(fileInfo.name, fileInfo.size, fileInfo.is_dir);
//        }
    }
}

void Model::updatePath() {
    char path[MAX_PATH_LEN];
    if (osMessageQueueGet(pathQueueHandle, path, NULL, 0) == osOK) {
        if (modelListener) {
            modelListener->updateCurrentPath(path);
        }
    }
}

void Model::updateStorageInfo() {
    uint32_t storageInfo[3];
    if (osMessageQueueGet(storageUsageQueueHandle, storageInfo, NULL, 0) == osOK) {
        if (modelListener) {
            modelListener->updateStorageInfo(
                storageInfo[0],  // totalKB
                storageInfo[1],  // usedKB
                storageInfo[2]   // freeKB
            );
        }
    }
}

void Model::processFileContent() {
    char buffer[MAX_READ_BUFF_SIZE];
    if (osMessageQueueGet(fileContentQueueHandle, buffer, NULL, 0) == osOK) {
        if (modelListener) {
            if (strcmp(buffer, "EOF") == 0) {
                modelListener->fileReadComplete();
            } else {
                modelListener->addFileContent(buffer);
            }
        }
    }
}


// File Browser Operations Implementation
void Model::createFolder(const char* folderName) {
    ::createFolder(folderName);
}

void Model::deleteItem(const char* path) {
    ::deleteItem(path);
}

void Model::createFile(const char* fileName) {
    ::createFile(fileName);
}

void Model::writeToFile(const char* fileName, const char* data, bool append) {
    ::writeToFile(fileName, data, append);
}

void Model::readFile(const char* fileName) {
    ::readFile(fileName);
}

void Model::renameItem(const char* oldName, const char* newName) {
    ::renameItem(oldName, newName);
}

void Model::enterFolder(const char* folderName) {
    ::enterFolder(folderName);
}

void Model::exitFolder() {
    ::exitFolder();
}

void Model::goToRoot() {
    ::goToRoot();
}

void Model::formatSDCard() {
    ::formatSD_Card();
}

void Model::getStorageInfo() {
    ::getStorageInfo();
}

void Model::getCurrentPath(char* buffer, size_t size) {
    ::getCurrentPath(buffer, size);
}

