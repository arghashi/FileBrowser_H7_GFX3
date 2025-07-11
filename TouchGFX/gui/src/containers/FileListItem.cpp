#include <gui/containers/FileListItem.hpp>
#include <touchgfx/Unicode.hpp>
#include <images/BitmapDatabase.hpp>

FileListItem::FileListItem()
{
//	   background.setBitmap(touchgfx::Bitmap(BITMAP_LIST_ITEM_BACKGROUND_ID));
	    background.setXY(0, 0);
	    add(background);

	    icon.setXY(5, 5);
	    add(icon);

	    nameText.setPosition(40, 5, 200, 25);
//	    nameText.setTypedText(touchgfx::TypedText(T_FILE_ITEM_NAME));
	    nameText.setWildcard(nameBuffer);
	    add(nameText);

	    sizeText.setPosition(250, 5, 80, 25);
//	    sizeText.setTypedText(touchgfx::TypedText(T_FILE_ITEM_SIZE));
	    sizeText.setWildcard(sizeBuffer);
	    add(sizeText);

	    setWidth(background.getWidth());
	    setHeight(background.getHeight());
	    isSelected = false;
}

void FileListItem::initialize()
{
    FileListItemBase::initialize();
}


void FileListItem::setup(bool isDirectory, const char* name, uint32_t size) {
    // Convert name to Unicode
    touchgfx::Unicode::fromUTF8((const uint8_t*)name, nameBuffer, NAME_BUFFER_SIZE);

    // Set icon
    if (isDirectory) {
//        icon.setBitmap(touchgfx::Bitmap(BITMAP_FOLDER_ICON_SMALL_ID));
    	icon.setBitmap(touchgfx::Bitmap(BITMAP_FOLDER_ID));
        sizeBuffer[0] = 0; // Clear size for folders
    } else {
        icon.setBitmap(touchgfx::Bitmap(BITMAP_FILEEE_ID));
        touchgfx::Unicode::snprintf(sizeBuffer, SIZE_BUFFER_SIZE, "%lu KB", (size + 1023) / 1024);
    }

    nameText.invalidate();
    sizeText.invalidate();
}

void FileListItem::setSelected(bool selected) {
    isSelected = selected;
//    background.setBitmap(touchgfx::Bitmap(
//        selected ? BITMAP_LIST_ITEM_SELECTED_ID : BITMAP_LIST_ITEM_BACKGROUND_ID
//    ));
    background.invalidate();
}
