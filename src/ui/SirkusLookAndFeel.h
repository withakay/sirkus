#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Sirkus::UI {

class SirkusLookAndFeel : public juce::LookAndFeel_V4
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

private:
    juce::Font mainFont;
    juce::Font boldFont;
    juce::Font lightFont;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SirkusLookAndFeel)
};

} // namespace Sirkus::UI
