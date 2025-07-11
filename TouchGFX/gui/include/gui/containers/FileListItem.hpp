#ifndef FILELISTITEM_HPP
#define FILELISTITEM_HPP

#include <gui_generated/containers/FileListItemBase.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

class FileListItem : public FileListItemBase
{
public:
    FileListItem();
    virtual ~FileListItem() {}

    virtual void initialize();

    void setup(bool isDirectory, const char* name, uint32_t size);
    void setSelected(bool selected);

//    virtual void handleClickEvent(const touchgfx::ClickEvent& event);

private:
    static const uint16_t NAME_BUFFER_SIZE = 40;
    static const uint16_t SIZE_BUFFER_SIZE = 15;

    touchgfx::Image background;
    touchgfx::Image icon;
    touchgfx::TextAreaWithOneWildcard nameText;
    touchgfx::TextAreaWithOneWildcard sizeText;

    touchgfx::Unicode::UnicodeChar nameBuffer[NAME_BUFFER_SIZE];
    touchgfx::Unicode::UnicodeChar sizeBuffer[SIZE_BUFFER_SIZE];

    bool isSelected;
protected:
};

#endif // FILELISTITEM_HPP
