#ifndef CUSTOMKEYBOARD_HPP
#define CUSTOMKEYBOARD_HPP

#include <gui_generated/containers/CustomKeyboardBase.hpp>
#include <touchgfx/widgets/Keyboard.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <gui/containers/KeyboardLayout.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/containers/KeyboardKeyMapping.hpp>

class CustomKeyboard : public CustomKeyboardBase
{
public:
    CustomKeyboard();
    virtual ~CustomKeyboard() {}

    /*
     * Override setTouchable to also affect the keyboard object inside this
     * CustomKeyboard.
     */
    void setTouchable(bool touch);
    
    Unicode::UnicodeChar *getBuffer ();

    virtual void clearBuffer ();

    virtual void initialize();

    private:
    /*
     * The size of the buffer that is used by the keyboard.
     * The size determines how much text the keyboard can contain in its textfield.
     */
    static const uint8_t BUFFER_SIZE = 18;

    /**
     * The keyboard which this CustomKeyboard wraps.
     */
    Keyboard keyboard;

    /**
     * The buffer used by the keyboard for text input.
     */
    Unicode::UnicodeChar buffer[BUFFER_SIZE];

    /**
     * Used to display text on top of the button for changing keyboard mode.
     */
    TextArea modeBtnTextArea;

    /**
     * Callback for the capslock button.
     */
    Callback<CustomKeyboard> capslockPressed;

    /**
     * Callback for the backspace button.
     */
    Callback<CustomKeyboard> backspacePressed;

    /**
     * Callback for the keyboard mode button.
     */
    Callback<CustomKeyboard> modePressed;

    /**
     * Callback for when keys are pressed on the keyboard.
     */
    Callback<CustomKeyboard, Unicode::UnicodeChar> keyPressed;

    /**
     * True if the keyboard should show alpha keys, false for numeric.
     */
    bool alphaKeys;

    /**
     * True if the keyboard should show upper-case keys.
     */
    bool uppercaseKeys;

    /**
     * True if the input position in the text field, and hence the buffer, is at the beginning.
     */
    bool firstCharacterEntry;

    /*
     * Sets the correct key mappings of the keyboard according to alpha/numeric and upper-case/lower-case.
     */
    void setKeyMappingList();

    /**
     * Callback handler for the backspace button.
     */
    void backspacePressedHandler();

    /**
     * Callback handler for the caps-lock button.
     */
    void capslockPressedHandler();

    /**
     * Callback handler for the mode button.
     */
    void modePressedHandler();

    /**
     * Callback handler for key presses.
     * @param keyChar The UnicodeChar for the key that was pressed.
     */
    void keyPressedhandler(Unicode::UnicodeChar keyChar);
    
protected:
};

#endif // CUSTOMKEYBOARD_HPP
