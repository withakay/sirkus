#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Sirkus::UI {

using namespace juce;

//==============================================================================
/** Custom Look And Feel subclasss.

    Simply override the methods you need to, anything else will be inherited from the base class.
    It's a good idea not to hard code your colours, use the findColour method along with appropriate
    ColourIds so you can set these on a per-component basis.
*/
struct CustomLookAndFeel : public juce::LookAndFeel_V4
{
    void drawRoundThumb(Graphics& g, float x, float y, float diameter, Colour colour, float outlineThickness);

    void drawButtonBackground(
        Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawTickBox(
        Graphics& g,
        Component& component,
        float x,
        float y,
        float w,
        float h,
        bool ticked,
        bool isEnabled,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawLinearSliderThumb(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        Slider::SliderStyle style,
        Slider& slider) override;

    void drawLinearSlider(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        Slider::SliderStyle style,
        Slider& slider) override;

    void drawLinearSliderBackground(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const Slider::SliderStyle style,
        Slider& slider) override;

    void drawRotarySlider(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        Slider& slider) override;
};

//==============================================================================
/** Another really simple look and feel that is very flat and square.

    This inherits from CustomLookAndFeel above for the linear bar and slider backgrounds.
*/
struct SquareLookAndFeel : public CustomLookAndFeel
{
    void drawButtonBackground(
        Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawTickBox(
        Graphics& g,
        Component& component,
        float x,
        float y,
        float w,
        float h,
        bool ticked,
        bool isEnabled,
        bool isMouseOverButton,
        bool isButtonDown) override;

    void drawLinearSliderThumb(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        Slider::SliderStyle style,
        Slider& slider) override;

    void drawRotarySlider(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        Slider& slider) override;
};

class SirkusLookAndFeel : public SquareLookAndFeel
{
public:
    SirkusLookAndFeel();
    ~SirkusLookAndFeel() override = default;

    //==============================================================================
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getSliderPopupFont(juce::Slider&) override;

    void drawComboBox(
        Graphics& g,
        int width,
        int height,
        bool isButtonDown,
        int buttonX,
        int buttonY,
        int buttonW,
        int buttonH,
        ComboBox& box) override;

private:
#if JUCE_MAC
    juce::Font mainFont = juce::Font(juce::FontOptions("SF Pro Display", 13.0f, juce::Font::plain));
    juce::Font boldFont = juce::Font(juce::FontOptions("SF Pro Display", 13.0f, juce::Font::bold));
    juce::Font lightFont = juce::Font(juce::FontOptions("SF Pro Text", 13.0f, juce::Font::plain));
#elif JUCE_WINDOWS
    mainFont = juce::Font(juce::FontOptions("Segoe UI", 13.0f, juce::Font::plain));
    boldFont = juce::Font(juce::FontOptions("Segoe UI"", 13.0f, juce::Font::bold));
    lightFont = juce::Font(juce::FontOptions("Segoe UI Light", 13.0f, juce::Font::plain));
#else
    mainFont = juce::Font(juce::FontOptions("Liberation Sans", 13.0f, juce::Font::plain));
    boldFont = juce::Font(juce::FontOptions("Liberation Sans", 13.0f, juce::Font::bold));
    lightFont = juce::Font(juce::FontOptions("Liberation Sans Light", 13.0f, juce::Font::plain));
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusLookAndFeel)
};

} // namespace Sirkus::UI
