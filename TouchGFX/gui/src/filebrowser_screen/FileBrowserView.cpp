#include <gui/filebrowser_screen/FileBrowserView.hpp>
#include <touchgfx/Unicode.hpp>


FileBrowserView::FileBrowserView()
{
    // Setup scroll container
    scrollContainer.setPosition(0, 40, 480, 200);
    scrollContainer.setScrollbarsColor(touchgfx::Color::getColorFromRGB(200, 200, 200));

    // Add file items to scroll container
    for (int i = 0; i < MAX_VISIBLE_ITEMS; i++) {
        fileItems[i].setXY(0, i * 40);
        scrollContainer.add(fileItems[i]);
    }

    add(scrollContainer);
    add(confirmPopup);
    add(textInputPopup);
}

void FileBrowserView::setupScreen()
{
    FileBrowserViewBase::setupScreen();

    add(list);
//    list.setHeight(0);



    // Set button actions
//    btnHome.setAction([this]() { handleHomeClicked(); });
//    btnBack.setAction([this]() { handleBackClicked(); });
//    btnNewFolder.setAction([this]() { handleNewFolderClicked(); });
//    btnNewFile.setAction([this]() { handleNewFileClicked(); });
//    btnRename.setAction([this]() { handleRenameClicked(); });
//    btnDelete.setAction([this]() { handleDeleteClicked(); });
//    btnFormat.setAction([this]() { handleFormatClicked(); });
}

void FileBrowserView::showConfirmPopup(const char* message) {
    // تبدیل به Unicode
    touchgfx::Unicode::UnicodeChar unicodeMsg[200];
    touchgfx::Unicode::fromUTF8((const uint8_t*)message, unicodeMsg, 200);

    // تنظیم متن و نمایش پاپ‌آپ
    confirmPopup.setText(unicodeMsg);
//    confirmPopup.setAction([this](bool confirmed) {
//        if (confirmed) {
//            presenter->confirmAction();
//        }
//    });
    confirmPopup.show();
}

void FileBrowserView::showTextInputPopup(const char* title, const char* initialText) {
    // تبدیل به Unicode
    touchgfx::Unicode::UnicodeChar unicodeTitle[50];
    touchgfx::Unicode::fromUTF8((const uint8_t*)title, unicodeTitle, 50);


    touchgfx::Unicode::UnicodeChar unicodeText[100];
    touchgfx::Unicode::fromUTF8((const uint8_t*)initialText, unicodeText, 100);

    // تنظیم متن و نمایش پاپ‌آپ
    textInputPopup.setTitle(unicodeTitle);
    textInputPopup.setText(unicodeText);

//    textInputPopup.setAction([this](const touchgfx::Unicode::UnicodeChar* text) {
//        if (text) {
//            char utf8Buffer[300];
//            touchgfx::Unicode::toUTF8(text, (uint8_t*)utf8Buffer, 300);
//            presenter->textInputConfirmed(utf8Buffer);
//        }
//    });
    textInputPopup.show();
}



void FileBrowserView::updateFileList(FileInfo items, int count) {
    // For simplicity, we'll just show first MAX_VISIBLE_ITEMS items
    uint16_t itemsToShow = (count > MAX_VISIBLE_ITEMS) ? MAX_VISIBLE_ITEMS : count;

    for (int i = 0; i < 10; i++) {
    	list.add(fileItems[i]);
        fileItems[i].setup(items.is_dir, items.name, items.size);
    }

    // Hide extra items
    for (int i = itemsToShow; i < MAX_VISIBLE_ITEMS; i++) {
        fileItems[i].setVisible(false);
    }

    scrollContainer.invalidate();
}

void FileBrowserView::updateCurrentPath(const char* path) {
    touchgfx::Unicode::UnicodeChar unicodePath[100];
    touchgfx::Unicode::fromUTF8((const uint8_t*)path, unicodePath, 100);
    txtPath.setWildcard(unicodePath);
    txtPath.invalidate();
}

void FileBrowserView::updateStorageInfo(uint32_t totalKB, uint32_t usedKB) {
    touchgfx::Unicode::UnicodeChar infoBuffer[50];
    touchgfx::Unicode::snprintf(infoBuffer, 50, "استفاده: %lu/%lu KB", usedKB, totalKB);
    txtStorageInfo.setWildcard(infoBuffer);
    txtStorageInfo.invalidate();
    
    // Update progress bar
    if (totalKB > 0) {
        int progress = (usedKB * 100) / totalKB;
        progressStorage.setValue(progress);
    }
}

// Button handlers implementation
void FileBrowserView::handleHomeClicked() {
    presenter->goToRoot();
}

void FileBrowserView::handleBackClicked() {
    presenter->exitFolder();
}

void FileBrowserView::handleNewFolderClicked() {
    presenter->showTextInputPopup("New folder name", "");
}

void FileBrowserView::handleNewFileClicked() {
    presenter->showTextInputPopup("New file name", "");
}

void FileBrowserView::handleRenameClicked() {
    // Get selected item
    // Implementation depends on your selection logic
    presenter->showTextInputPopup("Rename", "Current name");
}

void FileBrowserView::handleDeleteClicked() {
    presenter->showConfirmPopup("Are you sure you want to delete this item?");
}

void FileBrowserView::handleFormatClicked() {
    presenter->showConfirmPopup("Are you sure you want to format the SD card? All data will be erased!!");
}




//void FileBrowserView::tearDownScreen()
//{
//    FileBrowserViewBase::tearDownScreen();
//}
//
//
//void FileBrowserView::handleTickEvent() {
//    // هر 100 میلی‌ثانیه اسکرول را چک کن
//    static uint8_t tickCount = 0;
//    if(++tickCount >= 10) {
//        tickCount = 0;
//        updateVisibleItems();
//    }
//}
//
//
//void FileBrowserView::updateVisibleItems() {
//    int scrollPos = scrollFileList.getScrolledY();
//    int newFirstIndex = scrollPos / ITEM_HEIGHT;
//
//    if(newFirstIndex != firstVisibleIndex) {
//        firstVisibleIndex = newFirstIndex;
//
//        // فقط آیتم‌های مرئی را آپدیت کن
//        for(int i=0; i<15; i++) {
//            int itemIndex = firstVisibleIndex + i;
//            if(itemIndex < totalFileCount) {
//                fileItems[i].setup(
//                    fileItemsData[itemIndex].is_dir,
//                    fileItemsData[itemIndex].name,
//                    fileItemsData[itemIndex].size
//                );
//            }
//        }
//    }
//}
