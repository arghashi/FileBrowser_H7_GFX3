#ifndef TEXTINPUTPOPUP_HPP
#define TEXTINPUTPOPUP_HPP

#include <gui_generated/containers/TextInputPopupBase.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/mixins/ClickListener.hpp>
#include <gui/containers/CustomKeyboard.hpp>

class TextInputPopup : public TextInputPopupBase
{
public:
    TextInputPopup();
    virtual ~TextInputPopup() {}


    virtual void initialize();

    void setTitle(const touchgfx::Unicode::UnicodeChar* title);
    void setText(const touchgfx::Unicode::UnicodeChar* text);
    void setAction(touchgfx::GenericCallback<const touchgfx::Unicode::UnicodeChar*>& callback);

    void show();
    void hide();

protected:
    static const uint16_t TITLE_SIZE = 50;
    static const uint16_t TEXT_SIZE = 100;

    touchgfx::Box background;
    touchgfx::Box popupBox;
    touchgfx::TextArea titleText;
    touchgfx::TextAreaWithOneWildcard inputText;
    CustomKeyboard keyboard;
    touchgfx::Button buttonOk;
    touchgfx::Button buttonCancel;
    touchgfx::ClickListener<touchgfx::Button> backgroundClick;

    touchgfx::Unicode::UnicodeChar titleBuffer[TITLE_SIZE];
    touchgfx::Unicode::UnicodeChar textBuffer[TEXT_SIZE];
    touchgfx::GenericCallback<const touchgfx::Unicode::UnicodeChar*>* actionCallback;



    void handleOkClicked();
    void handleCancelClicked();
    void handleBackgroundClicked();
//protected:
//
//private:
//    static const uint16_t BUFFER_SIZE = 100;
//
//    touchgfx::Box background;
//    touchgfx::TextArea titleText;
//    touchgfx::Container inputContainer;
//    touchgfx::TextAreaWithOneWildcard inputText;
//    CustomKeyboard keyboard;
//
//    touchgfx::Unicode::UnicodeChar buffer[BUFFER_SIZE];
//
//    touchgfx::GenericCallback<const char*>* actionCallback;
//
//    void onKeyPressed(const touchgfx::Unicode::UnicodeChar key);
//    void onEnterPressed();
//    void onCancelPressed();
};

#endif // TEXTINPUTPOPUP_HPP
