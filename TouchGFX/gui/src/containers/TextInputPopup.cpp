#include <gui/containers/TextInputPopup.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

TextInputPopup::TextInputPopup(): actionCallback(0)
{
    // پس‌زمینه نیمه شفاف
    background.setPosition(0, 0, 480, 272);
    background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    background.setAlpha(150);
    background.setTouchable(true);
    add(background);

    // کادر اصلی پاپ‌آپ
    popupBox.setPosition(40, 30, 400, 212);
    popupBox.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
//    popupBox.setCornerRadius(10);
    add(popupBox);

    // عنوان
    titleText.setPosition(50, 40, 380, 30);
    titleText.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    titleText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_AFGW));
//    titleText.setWildcard(titleTextBuffer);
//    titleText.setWildcard(titleTextBuffer);
    add(titleText);

    // ناحیه ورود متن
    inputText.setPosition(50, 80, 380, 40);
    inputText.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    inputText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_4OV4));
//    inputText.setWildcard(textBuffer);
    inputText.setWildcard(inputTextBuffer);
    add(inputText);

    // کیبورد
    keyboard.setPosition(80, 272, 480, 320);
//    keyboard.setBuffer(textBuffer, TEXT_SIZE);
    add(keyboard);



    // دکمه شفاف برای کلیک روی پس‌زمینه
    backgroundClick.setPosition(0, 0, 480, 272);
    backgroundClick.setAlpha(0);
//    backgroundClick.setAction([this](const touchgfx::AbstractButton&) { handleBackgroundClicked(); });
    add(backgroundClick);

    // دکمه تایید
    buttonOk.setPosition(130, 220, 100, 40);
//    buttonOk.setLabelText(touchgfx::TypedText(T_OK_BUTTON));
//    buttonOk.setLabelColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
//    buttonOk.setColor(touchgfx::Color::getColorFromRGB(50, 150, 50));
//    buttonOk.setAction([this](const touchgfx::AbstractButton&) { handleOkClicked(); });
    add(buttonOk);

    // دکمه لغو
    buttonCancel.setPosition(250, 220, 100, 40);
//    buttonCancel.setLabelText(touchgfx::TypedText(T_CANCEL_BUTTON));
//    buttonCancel.setLabelColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
//    buttonCancel.setColor(touchgfx::Color::getColorFromRGB(200, 50, 50));
//    buttonCancel.setAction([this](const touchgfx::AbstractButton&) { handleCancelClicked(); });
    add(buttonCancel);

    // مخفی کردن در ابتدا
    setVisible(false);
    setTouchable(false);
}


void TextInputPopup::initialize()
{
    TextInputPopupBase::initialize();
}

void TextInputPopup::setTitle(const touchgfx::Unicode::UnicodeChar* title) {
    touchgfx::Unicode::strncpy(titleTextBuffer, title, TITLETEXT_SIZE);
    titleText.invalidate();
}

void TextInputPopup::setText(const touchgfx::Unicode::UnicodeChar* text) {
    touchgfx::Unicode::strncpy(inputTextBuffer, text, INPUTTEXT_SIZE);
//    keyboard.setBuffer(textBuffer, TEXT_SIZE);

    inputText.invalidate();
}

void TextInputPopup::setAction(touchgfx::GenericCallback<const touchgfx::Unicode::UnicodeChar*>& callback) {
    actionCallback = &callback;
}

void TextInputPopup::show() {
    setVisible(true);
    setTouchable(true);
    invalidate();
}

void TextInputPopup::hide() {
    setVisible(false);
    setTouchable(false);
    invalidate();
}

void TextInputPopup::handleOkClicked() {
    if (actionCallback && actionCallback->isValid()) {
        actionCallback->execute(keyboard.getBuffer());
    }
    keyboard.clearBuffer();
    hide();
}

void TextInputPopup::handleCancelClicked() {
    if (actionCallback && actionCallback->isValid()) {
        actionCallback->execute(nullptr);
    }
    hide();
}

void TextInputPopup::handleBackgroundClicked() {
    if (actionCallback && actionCallback->isValid()) {
        actionCallback->execute(nullptr);
    }
    // رفتار پیش‌فرض: لغو عملیات
    handleCancelClicked();
}
