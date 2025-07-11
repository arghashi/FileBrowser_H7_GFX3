#include <gui/filebrowser_screen/FileBrowserView.hpp>
#include <gui/filebrowser_screen/FileBrowserPresenter.hpp>

FileBrowserPresenter::FileBrowserPresenter(FileBrowserView& v)
    : view(v)
{

}

void FileBrowserPresenter::activate()
{
    // Request initial data
//    model->requestFileList();
//    model->requestStorageInfo();
}

void FileBrowserPresenter::deactivate()
{

}

// File operations
void FileBrowserPresenter::goToRoot() {
    model->goToRoot();
}

void FileBrowserPresenter::exitFolder() {
    model->exitFolder();
}

void FileBrowserPresenter::createNewFolder(const char* name) {
    model->createFolder(name);
}

void FileBrowserPresenter::createNewFile(const char* name) {
    model->createFile(name);
}

void FileBrowserPresenter::deleteItem() {
    // Get selected item from model
//    const char* selected = model->getSelectedItem();
//    if (selected) {
//        model->deleteItem(selected);
//    }
}

void FileBrowserPresenter::renameItem(const char* newName) {
    // Get selected item from model
//    const char* selected = model->getSelectedItem();
//    if (selected) {
//        model->renameItem(selected, newName);
//    }
}

void FileBrowserPresenter::formatSD() {
    model->formatSDCard();
}

// UI interactions
void FileBrowserPresenter::showConfirmPopup(const char* message) {
    view.showConfirmPopup(message);
}

void FileBrowserPresenter::showTextInputPopup(const char* title, const char* initialText) {
    view.showTextInputPopup(title, initialText);
}

// Model callbacks
void FileBrowserPresenter::updateFileList(FileInfo items, int count) {
    view.updateFileList(items, count);
}

void FileBrowserPresenter::currentPathUpdated(const char* path) {
    view.updateCurrentPath(path);
}

void FileBrowserPresenter::storageInfoUpdated(uint32_t totalKB, uint32_t usedKB) {
    view.updateStorageInfo(totalKB, usedKB);
}

//void FileBrowserPresenter::folderSelected(const char* name) {
//    // ارسال فرمان به سیستم فایل
//    FileCommand cmd;
//    cmd.type = CMD_ENTER_FOLDER;
//    strncpy(cmd.name, name, sizeof(cmd.name));
//    osMessageQueuePut(fileCommandQueueHandle, &cmd, 0, 0);
//}
//
//void FileBrowserPresenter::handleFileListUpdate(FileInfo* items, uint16_t count) {
//    // تخصیص حافظه در SDRAM
//    static FileInfo* fileListData = (FileInfo*)sdram_malloc(MAX_ITEMS * sizeof(FileInfo));
//    memcpy(fileListData, items, count * sizeof(FileInfo));
//
//    view.updateFileList(fileListData, count);
//}
