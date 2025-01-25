#include "StepButton.h"

namespace sirkus::ui {

StepButton::StepButton() {
  setOpaque(true);
  setBufferedToImage(true);
}

void StepButton::setEnabled(bool shouldBeEnabled) {
  if (enabled != shouldBeEnabled) {
    enabled = shouldBeEnabled;
    repaint();
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
}

void StepButton::setTriggered(bool isTriggered) {
  if (triggered != isTriggered) {
    triggered = isTriggered;
    repaint();
  }
}

void StepButton::setNoteValue(int note) {
  if (noteValue != note) {
    noteValue = note;
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
}

void StepButton::setVelocity(int newVelocity) {
  if (velocity != newVelocity) {
    velocity = newVelocity;
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
}

void StepButton::setNoteLength(NoteLength length) {
  if (noteLength != length) {
    noteLength = length;
    listeners.call([this](Listener &l) { l.stepButtonStateChanged(this); });
  }
}

void StepButton::setSelected(bool shouldBeSelected) {
  if (selected != shouldBeSelected) {
    selected = shouldBeSelected;
    repaint();
  }
}

void StepButton::setZoomFactor(float factor) {
  static constexpr float epsilon = 0.0001f;
  if (std::abs(zoomFactor - factor) > epsilon) {
    zoomFactor = factor;
    repaint();
  }
}

void StepButton::paint(juce::Graphics &g) {
  auto bounds = getLocalBounds().toFloat();

  // Background
  g.setColour(enabled ? juce::Colours::darkblue : juce::Colours::darkgrey);
  g.fillRoundedRectangle(bounds, 4.0f);

  // Selection outline
  if (selected) {
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 2.0f);
  }

  // Trigger indicator
  if (triggered) {
    g.setColour(juce::Colours::orange);
    auto indicatorBounds = bounds.reduced(bounds.getWidth() * 0.3f);
    g.fillEllipse(indicatorBounds);
  }

  // Velocity indicator (small bar at bottom)
  if (enabled) {
    g.setColour(juce::Colours::lightblue);
    float velocityWidth = (velocity / 127.0f) * bounds.getWidth();
    g.fillRect(bounds.getX(), bounds.getBottom() - 2.0f, velocityWidth, 2.0f);
  }
}

void StepButton::resized() {
  // Update any child components here if needed
}

void StepButton::mouseDown(const juce::MouseEvent &e) {
  if (e.mods.isLeftButtonDown()) {
    // Handle different click behaviors based on modifiers
    if (e.mods.isShiftDown()) {
      // Shift-click for range selection
      listeners.call([this](Listener &l) { l.stepButtonRangeSelect(this); });
    } else {
      // Toggle enabled state only if no modifiers (except Cmd/Ctrl for
      // multi-select)
      if (!e.mods.isCommandDown() && !e.mods.isCtrlDown()) {
        setEnabled(!enabled);
      }

      // Notify with modifiers for selection handling
      listeners.call(
          [this, &e](Listener &l) { l.stepButtonClicked(this, e.mods); });
    }
  }
}

void StepButton::mouseUp(const juce::MouseEvent &) {
  // Handle any mouse up behavior if needed
}

void StepButton::addListener(Listener *listener) { listeners.add(listener); }

void StepButton::removeListener(Listener *listener) {
  listeners.remove(listener);
}

} // namespace sirkus::ui