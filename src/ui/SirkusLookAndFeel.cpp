#include "SirkusLookAndFeel.h"

namespace Sirkus::UI {

SirkusLookAndFeel::SirkusLookAndFeel()
{
    // Set up default fonts
    // Note: Using system fonts that are likely to be available
    // TODO: Bundle custom fonts with the plugin for consistent look across
    // systems


    // Set default sans-serif font
    setDefaultSansSerifTypefaceName(mainFont.getTypefaceName());
}

juce::Font SirkusLookAndFeel::getLabelFont(juce::Label& label)
{
    // Use bold font for headers, light font for values
    if (label.getProperties()["isHeader"])
        return boldFont;
    if (label.getProperties()["isValue"])
        return lightFont;

    return mainFont;
}

juce::Font SirkusLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    // Scale font size with button height, but keep it reasonable
    const float fontSize = juce::jmin(13.0f, buttonHeight * 0.6f);
    return boldFont.withHeight(fontSize);
}

juce::Font SirkusLookAndFeel::getComboBoxFont(juce::ComboBox& box)
{
    // Scale font size with component height
    const float fontSize = juce::jmin(13.0f, box.getHeight() * 0.6f);
    return mainFont.withHeight(fontSize);
}

juce::Font SirkusLookAndFeel::getPopupMenuFont()
{
    return mainFont;
}

juce::Font SirkusLookAndFeel::getSliderPopupFont(juce::Slider&)
{
    return mainFont;
}

} // namespace Sirkus::UI
