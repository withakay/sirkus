#include "StepControls.h"

namespace sirkus::ui {

StepControls::StepControls() {
  setupNoteControls();
  setupVelocityControls();
  setupNoteLengthControls();
  setOpaque(true);
}

void StepControls::setupNoteControls() {
  noteLabel = std::make_unique<juce::Label>();
  noteLabel->setText("Note", juce::dontSendNotification);
  addAndMakeVisible(noteLabel.get());

  noteSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                              juce::Slider::TextBoxRight);
  noteSlider->setRange(0, 127, 1);
  noteSlider->setValue(noteValue, juce::dontSendNotification);
  noteSlider->onValueChange = [this] {
    setNoteValue(static_cast<int>(noteSlider->getValue()));
  };
  addAndMakeVisible(noteSlider.get());
}

void StepControls::setupVelocityControls() {
  velocityLabel = std::make_unique<juce::Label>();
  velocityLabel->setText("Velocity", juce::dontSendNotification);
  addAndMakeVisible(velocityLabel.get());

  velocitySlider = std::make_unique<juce::Slider>(
      juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
  velocitySlider->setRange(0, 127, 1);
  velocitySlider->setValue(velocity, juce::dontSendNotification);
  velocitySlider->onValueChange = [this] {
    setVelocity(static_cast<int>(velocitySlider->getValue()));
  };
  addAndMakeVisible(velocitySlider.get());
}

void StepControls::setupNoteLengthControls() {
  noteLengthLabel = std::make_unique<juce::Label>();
  noteLengthLabel->setText("Length", juce::dontSendNotification);
  addAndMakeVisible(noteLengthLabel.get());

  noteLengthCombo = std::make_unique<juce::ComboBox>();
  updateNoteLengthComboBox();
  noteLengthCombo->setSelectedItemIndex(
      3, juce::dontSendNotification); // Default to 1/16
  noteLengthCombo->onChange = [this] {
    setNoteLength(
        static_cast<NoteLength>(noteLengthCombo->getSelectedItemIndex()));
  };
  addAndMakeVisible(noteLengthCombo.get());
}

void StepControls::updateNoteLengthComboBox() {
  noteLengthCombo->clear(juce::dontSendNotification);
  noteLengthCombo->addItem("1/128",
                           static_cast<int>(NoteLength::OneTwentyEighth) + 1);
  noteLengthCombo->addItem("1/64", static_cast<int>(NoteLength::Sixty4th) + 1);
  noteLengthCombo->addItem("1/32", static_cast<int>(NoteLength::Thirty2nd) + 1);
  noteLengthCombo->addItem("1/16", static_cast<int>(NoteLength::Sixteen) + 1);
  noteLengthCombo->addItem("1/8", static_cast<int>(NoteLength::Eighth) + 1);
  noteLengthCombo->addItem("1/4", static_cast<int>(NoteLength::Quarter) + 1);
  noteLengthCombo->addItem("1/2", static_cast<int>(NoteLength::Half) + 1);
  noteLengthCombo->addItem("1", static_cast<int>(NoteLength::Whole) + 1);
  noteLengthCombo->addItem("2", static_cast<int>(NoteLength::TwoBars) + 1);
  noteLengthCombo->addItem("4", static_cast<int>(NoteLength::FourBars) + 1);
  noteLengthCombo->addItem("8", static_cast<int>(NoteLength::EightBars) + 1);
  noteLengthCombo->addItem("16", static_cast<int>(NoteLength::SixteenBars) + 1);
}

void StepControls::setNoteValue(int note) {
  note = juce::jlimit(0, 127, note);
  if (noteValue != note) {
    noteValue = note;
    noteSlider->setValue(noteValue, juce::dontSendNotification);
    listeners.call(
        [this](Listener &l) { l.noteValueChanged(this, noteValue); });
  }
}

void StepControls::setVelocity(int vel) {
  vel = juce::jlimit(0, 127, vel);
  if (velocity != vel) {
    velocity = vel;
    velocitySlider->setValue(velocity, juce::dontSendNotification);
    listeners.call([this](Listener &l) { l.velocityChanged(this, velocity); });
  }
}

void StepControls::setNoteLength(NoteLength length) {
  if (noteLength != length) {
    noteLength = length;
    noteLengthCombo->setSelectedId(static_cast<int>(noteLength) + 1,
                                   juce::dontSendNotification);
    listeners.call(
        [this](Listener &l) { l.noteLengthChanged(this, noteLength); });
  }
}

void StepControls::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::darkgrey);
}

void StepControls::resized() {
  auto bounds = getLocalBounds().reduced(10);
  const int labelWidth = 60;
  const int controlHeight = 30;
  const int spacing = 10;

  // Note controls
  noteLabel->setBounds(
      bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
  bounds.removeFromLeft(spacing);
  noteSlider->setBounds(bounds.removeFromLeft(200).withHeight(controlHeight));
  bounds.removeFromLeft(spacing * 2);

  // Velocity controls
  velocityLabel->setBounds(
      bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
  bounds.removeFromLeft(spacing);
  velocitySlider->setBounds(
      bounds.removeFromLeft(200).withHeight(controlHeight));
  bounds.removeFromLeft(spacing * 2);

  // Note length controls
  noteLengthLabel->setBounds(
      bounds.removeFromLeft(labelWidth).withHeight(controlHeight));
  bounds.removeFromLeft(spacing);
  noteLengthCombo->setBounds(
      bounds.removeFromLeft(100).withHeight(controlHeight));
}

void StepControls::addListener(Listener *listener) { listeners.add(listener); }

void StepControls::removeListener(Listener *listener) {
  listeners.remove(listener);
}

} // namespace sirkus::ui