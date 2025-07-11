#ifndef FILEBROWSERPRESENTER_HPP
#define FILEBROWSERPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
#include <touchgfx/hal/Types.hpp>

extern"C"{
#include "file_browser.h"
#include "main.h"
}

using namespace touchgfx;

class FileBrowserView;

class FileBrowserPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    FileBrowserPresenter(FileBrowserView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~FileBrowserPresenter() {};

    // File operations
    void goToRoot();
    void exitFolder();
    void createNewFolder(const char* name);
    void createNewFile(const char* name);
    void deleteItem();
    void renameItem(const char* newName);
    void formatSD();

    // UI interactions
    void showConfirmPopup(const char* message);
    void showTextInputPopup(const char* title, const char* initialText);

    // Model callbacks
    virtual void updateFileList(FileInfo items, int count);
    virtual void currentPathUpdated(const char* path);
    virtual void storageInfoUpdated(uint32_t totalKB, uint32_t usedKB);



private:
    FileBrowserPresenter();

    FileBrowserView& view;
};

#endif // FILEBROWSERPRESENTER_HPP
