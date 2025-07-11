#ifndef FILEBROWSERVIEW_HPP
#define FILEBROWSERVIEW_HPP

#include <gui_generated/filebrowser_screen/FileBrowserViewBase.hpp>
#include <gui/filebrowser_screen/FileBrowserPresenter.hpp>
#include <gui/containers/FileListItem.hpp>
#include <gui/containers/ConfirmPopup.hpp>
#include <gui/containers/TextInputPopup.hpp>
//#include <touchgfx/widgets/ScrollableContainer.hpp>

class FileBrowserView : public FileBrowserViewBase
{
public:
    FileBrowserView();
    virtual ~FileBrowserView() {}
    virtual void setupScreen();
//    virtual void tearDownScreen();
//    virtual void handleTickEvent();

//    virtual void updateVisibleItems();
//
//    ScrollableContainer scrollFileList;
//    FileListItem fileItems[15]; // فقط 15 آیتم قابل مشاهده

    // Update file list
    virtual void updateFileList(FileInfo items, int count);

    // Update path
    void updateCurrentPath(const char* path);

    // Update storage info
    void updateStorageInfo(uint32_t totalKB, uint32_t usedKB);

    // Show popups
    void showConfirmPopup(const char* message);
    void showTextInputPopup(const char* title, const char* initialText);


//private:
//    uint16_t totalFileCount = 0;
//    uint16_t firstVisibleIndex = 0;
//    FileInfo* fileItemsData = nullptr;

protected:
    // Button handlers
    virtual void handleHomeClicked();
    virtual void handleBackClicked();
    virtual void handleNewFolderClicked();
    virtual void handleNewFileClicked();
    virtual void handleRenameClicked();
    virtual void handleDeleteClicked();
    virtual void handleFormatClicked();

private:
    static const uint16_t MAX_VISIBLE_ITEMS = 15;

    touchgfx::ScrollableContainer scrollContainer;
    FileListItem fileItems[MAX_VISIBLE_ITEMS];

    void updateVisibleItems();

    ConfirmPopup confirmPopup;
    TextInputPopup textInputPopup;

    GenericCallback<const touchgfx::Unicode::UnicodeChar*>* actionCallback;
};

#endif // FILEBROWSERVIEW_HPP
