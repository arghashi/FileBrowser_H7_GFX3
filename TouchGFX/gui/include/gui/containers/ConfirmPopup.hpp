#ifndef CONFIRMPOPUP_HPP
#define CONFIRMPOPUP_HPP

#include <gui_generated/containers/ConfirmPopupBase.hpp>
#include <touchgfx/containers/ModalWindow.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/mixins/ClickListener.hpp>

class ConfirmPopup : public ConfirmPopupBase
{
public:
    ConfirmPopup();
    virtual ~ConfirmPopup() {}

    virtual void initialize();

    void setText(const touchgfx::Unicode::UnicodeChar* text);
    void setAction(touchgfx::GenericCallback<bool>& callback);

    void show();
    void hide();

protected:
    static const uint16_t TEXT_SIZE = 200;

    touchgfx::Box background;
    touchgfx::Box popupBox;
    touchgfx::TextAreaWithOneWildcard messageText;
    touchgfx::Button buttonYes;
    touchgfx::Button buttonNo;
    touchgfx::ClickListener<touchgfx::Button> backgroundClick; // برای کلیک خارج پاپ‌آپ

    touchgfx::Unicode::UnicodeChar textBuffer[TEXT_SIZE];
    touchgfx::GenericCallback<bool>* actionCallback;

    void handleYesClicked();
    void handleNoClicked();
    void handleBackgroundClicked();

//    void setText(const char* text);
//    void setAction(touchgfx::GenericCallback<bool>& action);
//protected:
//
//private:
//    touchgfx::Box background;
//    touchgfx::TextArea messageText;
//    touchgfx::Button btnYes;
//    touchgfx::Button btnNo;
//
//    touchgfx::GenericCallback<bool>* actionCallback;
//
//    void handleYesClicked();
//    void handleNoClicked();
};

#endif // CONFIRMPOPUP_HPP
