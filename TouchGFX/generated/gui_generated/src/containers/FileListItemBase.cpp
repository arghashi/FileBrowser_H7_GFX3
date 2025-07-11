/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/FileListItemBase.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>

FileListItemBase::FileListItemBase()
{
    setWidth(1024);
    setHeight(50);
    name.setPosition(85, 13, 830, 25);
    name.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    name.setLinespacing(0);
    name.setWideTextAction(WIDE_TEXT_CHARWRAP);
    nameBuffer1[0] = 0;
    name.setWildcard1(nameBuffer1);
    nameBuffer1[0] = 0;
    name.setWildcard2(nameBuffer2);
    name.setTypedText(touchgfx::TypedText(T___SINGLEUSE_XYHE));
    add(name);

    icon.setXY(19, 5);
    icon.setBitmap(touchgfx::Bitmap(BITMAP_FILEEE_ID));
    add(icon);
}

FileListItemBase::~FileListItemBase()
{

}

void FileListItemBase::initialize()
{

}
