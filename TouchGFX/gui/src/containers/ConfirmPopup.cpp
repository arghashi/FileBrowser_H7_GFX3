#include <gui/containers/ConfirmPopup.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

ConfirmPopup::ConfirmPopup()
{
    // پس‌زمینه نیمه شفاف
    background.setPosition(0, 0, 480, 272);
    background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    background.setAlpha(150);
    background.setTouchable(true);
    add(background);

    // کادر اصلی پاپ‌آپ
    popupBox.setPosition(50, 70, 380, 132);
    popupBox.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
//    popupBox.setCornerRadius(10);
    add(popupBox);

    // متن پیغام
    messageText.setPosition(70, 90, 340, 60);
    messageText.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
//    messageText.setTypedText(touchgfx::TypedText(T_CONFIRM_MESSAGE));
    messageText.setWildcard(textBuffer);
    add(messageText);

    // دکمه بله
    buttonYes.setPosition(110, 160, 100, 40);
//    buttonYes.setLabelText(touchgfx::TypedText(T_YES_BUTTON));
//    buttonYes.setLabelColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
//    buttonYes.setColor(touchgfx::Color::getColorFromRGB(50, 150, 50));
//    buttonYes.setAction([this](const touchgfx::AbstractButton&) { handleYesClicked(); });
    add(buttonYes);

    // دکمه خیر
    buttonNo.setPosition(270, 160, 100, 40);
//    buttonNo.setLabelText(touchgfx::TypedText(T_NO_BUTTON));
//    buttonNo.setLabelColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
//    buttonNo.setColor(touchgfx::Color::getColorFromRGB(200, 50, 50));
//    buttonNo.setAction([this](const touchgfx::AbstractButton&) { handleNoClicked(); });
    add(buttonNo);

    // دکمه شفاف برای کلیک روی پس‌زمینه
    backgroundClick.setPosition(0, 0, 480, 272);
    backgroundClick.setAlpha(0);
//    backgroundClick.setAction([this](const touchgfx::AbstractButton&) { handleBackgroundClicked(); });
    add(backgroundClick);

    // مخفی کردن در ابتدا
    setVisible(false);
    setTouchable(false);
}

void ConfirmPopup::initialize()
{
    ConfirmPopupBase::initialize();
}


void ConfirmPopup::setText(const touchgfx::Unicode::UnicodeChar* text) {
    touchgfx::Unicode::strncpy(textBuffer, text, TEXT_SIZE);
    messageText.invalidate();
}

void ConfirmPopup::setAction(touchgfx::GenericCallback<bool>& callback) {
    actionCallback = &callback;
}

void ConfirmPopup::show() {
    setVisible(true);
    setTouchable(true);
    invalidate();
}

void ConfirmPopup::hide() {
    setVisible(false);
    setTouchable(false);
    invalidate();
}

void ConfirmPopup::handleYesClicked() {
    if (actionCallback && actionCallback->isValid()) {
        actionCallback->execute(true);
    }
    hide();
}

void ConfirmPopup::handleNoClicked() {
    if (actionCallback && actionCallback->isValid()) {
        actionCallback->execute(false);
    }
    hide();
}

void ConfirmPopup::handleBackgroundClicked() {
    // رفتار پیش‌فرض: کلیک روی پس‌زمینه معادل "خیر" است
    handleNoClicked();
}
