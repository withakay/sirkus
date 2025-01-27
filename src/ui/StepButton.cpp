#include "StepButton.h"

namespace sirkus::ui {

StepButton::StepButton() { setOpaque(true); }

void StepButton::paint(juce::Graphics &g) {
  auto bounds = getLocalBounds().toFloat();

  // Background
  g.setColour(enabled ? juce::Colours::darkgreen : juce::Colours::darkgrey);
  g.fillRect(bounds);

  // Selection highlight
  if (selected) {
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.fillRect(bounds);
  }

  // Trigger indicator
  if (triggered) {
    g.setColour(juce::Colours::yellow);
    auto triggerBounds = bounds.reduced(bounds.getWidth() * 0.3f);
    g.fillEllipse(triggerBounds);
  }

  // Border
  g.setColour(juce::Colours::black);
  g.drawRect(bounds, 1.0f);
}

void StepButton::mouseDown(const juce::MouseEvent &event) {
  mouseWasDragged = false;

  if (event.mods.isShiftDown()) {
    listeners.call([this](Listener &l) { l.stepButtonRangeSelect(this); });
  } else {
    enabled = !enabled;
    repaint();

    const auto mods = event.mods; // Capture the mods locally
    listeners.call(
        [this, mods](Listener &l) { l.stepButtonClicked(this, mods); });
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
}

void StepButton::mouseUp(const juce::MouseEvent &event) {
  if (mouseWasDragged) {
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
  mouseWasDragged = false;
}

void StepButton::setEnabled(bool shouldBeEnabled) {
  if (enabled != shouldBeEnabled) {
    enabled = shouldBeEnabled;
    repaint();
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
}

void StepButton::setSelected(bool shouldBeSelected) {
  if (selected != shouldBeSelected) {
    selected = shouldBeSelected;
    repaint();
  }
}

void StepButton::setTriggered(bool shouldBeTriggered) {
  if (triggered != shouldBeTriggered) {
    triggered = shouldBeTriggered;
    repaint();
  }
}

void StepButton::setZoomFactor(float factor) {
  if (std::abs(zoomFactor - factor) > 0.0001f) {
    zoomFactor = factor;
    repaint();
  }
}

void StepButton::addListener(Listener *listener) { listeners.add(listener); }

void StepButton::removeListener(Listener *listener) {
  listeners.remove(listener);
}

} // namespace sirkus::ui
