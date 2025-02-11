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
    void drawRoundThumb(Graphics& g, float x, float y, float diameter, Colour colour, float outlineThickness)
    {
        auto halfThickness = outlineThickness * 0.5f;

        Path p;
        p.addEllipse(x + halfThickness, y + halfThickness, diameter - outlineThickness, diameter - outlineThickness);

        DropShadow(Colours::black, 1, {}).drawForPath(g, p);

        g.setColour(colour);
        g.fillPath(p);

        g.setColour(colour.brighter());
        g.strokePath(p, PathStrokeType(outlineThickness));
    }

    void drawButtonBackground(
        Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool isMouseOverButton,
        bool isButtonDown) override
    {
        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                          .withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f);

        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.1f);

        auto flatOnLeft = button.isConnectedOnLeft();
        auto flatOnRight = button.isConnectedOnRight();
        auto flatOnTop = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        auto width = (float) button.getWidth() - 1.0f;
        auto height = (float) button.getHeight() - 1.0f;

        if (width > 0 && height > 0)
        {
            auto cornerSize = jmin(15.0f, jmin(width, height) * 0.45f);
            auto lineThickness = cornerSize * 0.1f;
            auto halfThickness = lineThickness * 0.5f;

            Path outline;
            outline.addRoundedRectangle(
                0.5f + halfThickness,
                0.5f + halfThickness,
                width - lineThickness,
                height - lineThickness,
                cornerSize,
                cornerSize,
                !(flatOnLeft || flatOnTop),
                !(flatOnRight || flatOnTop),
                !(flatOnLeft || flatOnBottom),
                !(flatOnRight || flatOnBottom));

            auto outlineColour =
                button.findColour(button.getToggleState() ? TextButton::textColourOnId : TextButton::textColourOffId);

            g.setColour(baseColour);
            g.fillPath(outline);

            if (!button.getToggleState())
            {
                g.setColour(outlineColour);
                g.strokePath(outline, PathStrokeType(lineThickness));
            }
        }
    }

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
        bool isButtonDown) override
    {
        auto boxSize = w * 0.7f;

        auto isDownOrDragging =
            component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());

        auto colour =
            component.findColour(TextButton::buttonColourId)
                     .withMultipliedSaturation((component.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
                     .withMultipliedAlpha(component.isEnabled() ? 1.0f : 0.7f);

        drawRoundThumb(
            g,
            x,
            y + (h - boxSize) * 0.5f,
            boxSize,
            colour,
            isEnabled ? ((isButtonDown || isMouseOverButton) ? 1.1f : 0.5f) : 0.3f);

        if (ticked)
        {
            g.setColour(isEnabled ? findColour(TextButton::buttonOnColourId) : Colours::grey);

            auto scale = 9.0f;
            auto trans = AffineTransform::scale(w / scale, h / scale).translated(x - 2.5f, y + 1.0f);

            g.fillPath(LookAndFeel_V4::getTickShape(6.0f), trans);
        }
    }

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
        Slider& slider) override
    {
        auto sliderRadius = (float) (getSliderThumbRadius(slider) - 2);

        auto isDownOrDragging = slider.isEnabled() && (slider.isMouseOverOrDragging() || slider.isMouseButtonDown());

        auto knobColour =
            slider.findColour(Slider::thumbColourId)
                  .withMultipliedSaturation((slider.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
                  .withMultipliedAlpha(slider.isEnabled() ? 1.0f : 0.7f);

        if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
        {
            float kx, ky;

            if (style == Slider::LinearVertical)
            {
                kx = (float) x + (float) width * 0.5f;
                ky = sliderPos;
            }
            else
            {
                kx = sliderPos;
                ky = (float) y + (float) height * 0.5f;
            }

            auto outlineThickness = slider.isEnabled() ? 0.8f : 0.3f;

            drawRoundThumb(g, kx - sliderRadius, ky - sliderRadius, sliderRadius * 2.0f, knobColour, outlineThickness);
        }
        else
        {
            // Just call the base class for the demo
            LookAndFeel_V2::drawLinearSliderThumb(
                g,
                x,
                y,
                width,
                height,
                sliderPos,
                minSliderPos,
                maxSliderPos,
                style,
                slider);
        }
    }

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
        Slider& slider) override
    {
        g.fillAll(slider.findColour(Slider::backgroundColourId));

        if (style == Slider::LinearBar || style == Slider::LinearBarVertical)
        {
            Path p;

            if (style == Slider::LinearBarVertical)
                p.addRectangle((float) x, sliderPos, (float) width, 1.0f + (float) height - sliderPos);
            else
                p.addRectangle((float) x, (float) y, sliderPos - (float) x, (float) height);

            auto baseColour = slider.findColour(Slider::rotarySliderFillColourId)
                                    .withMultipliedSaturation(slider.isEnabled() ? 1.0f : 0.5f)
                                    .withMultipliedAlpha(0.8f);

            g.setColour(baseColour);
            g.fillPath(p);

            auto lineThickness = jmin(15.0f, (float) jmin(width, height) * 0.45f) * 0.1f;
            g.drawRect(slider.getLocalBounds().toFloat(), lineThickness);
        }
        else
        {
            drawLinearSliderBackground(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
            drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }

    void drawLinearSliderBackground(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float /*sliderPos*/,
        float /*minSliderPos*/,
        float /*maxSliderPos*/,
        const Slider::SliderStyle /*style*/,
        Slider& slider) override
    {
        auto sliderRadius = (float) getSliderThumbRadius(slider) - 5.0f;
        Path on, off;

        if (slider.isHorizontal())
        {
            auto iy = (float) y + (float) height * 0.5f - sliderRadius * 0.5f;
            Rectangle<float> r((float) x - sliderRadius * 0.5f, iy, (float) width + sliderRadius, sliderRadius);
            auto onW = r.getWidth() * ((float) slider.valueToProportionOfLength(slider.getValue()));

            on.addRectangle(r.removeFromLeft(onW));
            off.addRectangle(r);
        }
        else
        {
            auto ix = (float) x + (float) width * 0.5f - sliderRadius * 0.5f;
            Rectangle<float> r(ix, (float) y - sliderRadius * 0.5f, sliderRadius, (float) height + sliderRadius);
            auto onH = r.getHeight() * ((float) slider.valueToProportionOfLength(slider.getValue()));

            on.addRectangle(r.removeFromBottom(onH));
            off.addRectangle(r);
        }

        g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
        g.fillPath(on);

        g.setColour(slider.findColour(Slider::trackColourId));
        g.fillPath(off);
    }

    void drawRotarySlider(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        Slider& slider) override
    {
        auto radius = (float) jmin(width / 2, height / 2) - 2.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        if (slider.isEnabled())
            g.setColour(slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour(Colour(0x80808080));

        {
            Path filledArc;
            filledArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, 0.0);
            g.fillPath(filledArc);
        }

        {
            auto lineThickness = jmin(15.0f, (float) jmin(width, height) * 0.45f) * 0.1f;
            Path outlineArc;
            outlineArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, 0.0);
            g.strokePath(outlineArc, PathStrokeType(lineThickness));
        }
    }
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
        bool isButtonDown) override
    {
        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                          .withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f);

        if (isButtonDown || isMouseOverButton)
            baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.1f);

        auto width = (float) button.getWidth() - 1.0f;
        auto height = (float) button.getHeight() - 1.0f;

        if (width > 0 && height > 0)
        {
            g.setGradientFill(ColourGradient::vertical(baseColour, 0.0f, baseColour.darker(0.1f), height));

            g.fillRect(button.getLocalBounds());
        }
    }

    void drawComboBox(
        Graphics& g,
        int width,
        int height,
        bool isButtonDown,
        int buttonX,
        int buttonY,
        int buttonW,
        int buttonH,
        ComboBox& box) override
    {
        Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(box.findColour(ComboBox::backgroundColourId));
        g.fillRect(boxBounds.toFloat());

        auto outlineColour = box.findColour(ComboBox::outlineColourId);
        auto lineThickness = box.isEnabled() ? 1.0f : 0.5f;

        g.setColour(outlineColour);
        g.drawRect(boxBounds.toFloat(), lineThickness);

        Rectangle<int> arrowZone(buttonX, buttonY, buttonW, buttonH);
        Path path;
        path.startNewSubPath((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        path.lineTo((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
        path.lineTo((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, PathStrokeType(2.0f));
    }

    void drawTickBox(
        Graphics& g,
        Component& component,
        float x,
        float y,
        float w,
        float h,
        bool ticked,
        bool isEnabled,
        bool /*isMouseOverButton*/,
        bool /*isButtonDown*/) override
    {
        auto boxSize = w * 0.7f;

        auto isDownOrDragging =
            component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());

        auto colour =
            component.findColour(TextButton::buttonOnColourId)
                     .withMultipliedSaturation((component.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
                     .withMultipliedAlpha(component.isEnabled() ? 1.0f : 0.7f);

        g.setColour(colour);

        Rectangle<float> r(x, y + (h - boxSize) * 0.5f, boxSize, boxSize);
        g.fillRect(r);

        if (ticked)
        {
            auto tickPath = LookAndFeel_V4::getTickShape(6.0f);
            g.setColour(isEnabled ? findColour(TextButton::buttonColourId) : Colours::grey);

            auto transform = RectanglePlacement(RectanglePlacement::centred)
                .getTransformToFit(tickPath.getBounds(), r.reduced(r.getHeight() * 0.05f));

            g.fillPath(tickPath, transform);
        }
    }

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
        Slider& slider) override
    {
        auto sliderRadius = (float) getSliderThumbRadius(slider);

        bool isDownOrDragging = slider.isEnabled() && (slider.isMouseOverOrDragging() || slider.isMouseButtonDown());

        auto knobColour =
            slider.findColour(Slider::rotarySliderFillColourId)
                  .withMultipliedSaturation((slider.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
                  .withMultipliedAlpha(slider.isEnabled() ? 1.0f : 0.7f);

        g.setColour(knobColour);

        if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
        {
            float kx, ky;

            if (style == Slider::LinearVertical)
            {
                kx = (float) x + (float) width * 0.5f;
                ky = sliderPos;
                g.fillRect(Rectangle<float>(kx - sliderRadius, ky - 2.5f, sliderRadius * 2.0f, 5.0f));
            }
            else
            {
                kx = sliderPos;
                ky = (float) y + (float) height * 0.5f;
                g.fillRect(Rectangle<float>(kx - 2.5f, ky - sliderRadius, 5.0f, sliderRadius * 2.0f));
            }
        }
        else
        {
            // Just call the base class for the demo
            LookAndFeel_V2::drawLinearSliderThumb(
                g,
                x,
                y,
                width,
                height,
                sliderPos,
                minSliderPos,
                maxSliderPos,
                style,
                slider);
        }
    }

    void drawRotarySlider(
        Graphics& g,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        Slider& slider) override
    {
        auto diameter = (float) jmin(width, height) - 4.0f;
        auto radius = (diameter / 2.0f) * std::cos(MathConstants<float>::pi / 4.0f);
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        auto baseColour = slider.isEnabled()
                              ? slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 0.8f : 1.0f)
                              : Colour(0x80808080);

        Rectangle<float> r(rx, ry, rw, rw);
        auto transform = AffineTransform::rotation(angle, r.getCentreX(), r.getCentreY());

        auto x1 = r.getTopLeft().getX();
        auto y1 = r.getTopLeft().getY();
        auto x2 = r.getBottomLeft().getX();
        auto y2 = r.getBottomLeft().getY();

        transform.transformPoints(x1, y1, x2, y2);

        g.setGradientFill(ColourGradient(baseColour, x1, y1, baseColour.darker(0.1f), x2, y2, false));

        Path knob;
        knob.addRectangle(r);
        g.fillPath(knob, transform);

        Path needle;
        auto r2 = r * 0.1f;
        needle.addRectangle(r2.withPosition({r.getCentreX() - (r2.getWidth() / 2.0f), r.getY()}));

        g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId));
        g.fillPath(needle, AffineTransform::rotation(angle, r.getCentreX(), r.getCentreY()));
    }
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
