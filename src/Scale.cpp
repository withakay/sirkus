#include "Scale.h"
#include <JuceHeader.h>
#include <algorithm>

namespace sirkus {

Scale::Scale() : degrees(MAJOR_SCALE.begin(), MAJOR_SCALE.end()) {
}

Scale::Scale(Type scaleType, uint8_t rootNote) : type(scaleType), root(rootNote % 12) {
    updateDegrees();
}

Scale::Scale(const std::vector<uint8_t>& scaleNotes, uint8_t rootNote)
    : type(Type::Custom), root(rootNote % 12), degrees(scaleNotes) {
    // Ensure degrees are sorted and unique
    std::sort(degrees.begin(), degrees.end());
    auto last = std::unique(degrees.begin(), degrees.end());
    degrees.erase(last, degrees.end());
    
    // Ensure all degrees are within 0-11
    for (auto& note : degrees) {
        note %= 12;
    }
}

void Scale::setType(Type newType) {
    type = newType;
    updateDegrees();
}

void Scale::setRoot(uint8_t newRoot) {
    root = newRoot % 12;
    updateDegrees();
}

void Scale::setCustomDegrees(const std::vector<uint8_t>& newDegrees) {
    type = Type::Custom;
    degrees = newDegrees;
    
    // Ensure degrees are sorted and unique
    std::sort(degrees.begin(), degrees.end());
    auto last = std::unique(degrees.begin(), degrees.end());
    degrees.erase(last, degrees.end());
    
    // Ensure all degrees are within 0-11
    for (auto& note : degrees) {
        note %= 12;
    }
}

std::string Scale::getName() const {
    std::string rootName;
    switch (root) {
        case 0: rootName = "C"; break;
        case 1: rootName = "C#"; break;
        case 2: rootName = "D"; break;
        case 3: rootName = "D#"; break;
        case 4: rootName = "E"; break;
        case 5: rootName = "F"; break;
        case 6: rootName = "F#"; break;
        case 7: rootName = "G"; break;
        case 8: rootName = "G#"; break;
        case 9: rootName = "A"; break;
        case 10: rootName = "A#"; break;
        case 11: rootName = "B"; break;
        default: rootName = "?"; break;
    }
    
    std::string typeName;
    switch (type) {
        case Type::Major: typeName = "Major"; break;
        case Type::Minor: typeName = "Minor"; break;
        case Type::HarmonicMinor: typeName = "Harmonic Minor"; break;
        case Type::MelodicMinor: typeName = "Melodic Minor"; break;
        case Type::Dorian: typeName = "Dorian"; break;
        case Type::Phrygian: typeName = "Phrygian"; break;
        case Type::Lydian: typeName = "Lydian"; break;
        case Type::Mixolydian: typeName = "Mixolydian"; break;
        case Type::Locrian: typeName = "Locrian"; break;
        case Type::PentatonicMajor: typeName = "Major Pentatonic"; break;
        case Type::PentatonicMinor: typeName = "Minor Pentatonic"; break;
        case Type::Blues: typeName = "Blues"; break;
        case Type::Chromatic: typeName = "Chromatic"; break;
        case Type::Custom: typeName = "Custom"; break;
    }
    
    return rootName + " " + typeName;
}

uint8_t Scale::quantizeUp(uint8_t note) const {
    if (degrees.empty()) return note;
    
    const uint8_t noteInOctave = note % 12;
    const uint8_t octave = note / 12;
    
    // Find first degree greater than or equal to note
    auto it = std::lower_bound(degrees.begin(), degrees.end(), noteInOctave);
    if (it == degrees.end()) {
        // Wrap to next octave
        return (octave + 1) * 12 + degrees.front();
    }
    return octave * 12 + *it;
}

uint8_t Scale::quantizeDown(uint8_t note) const {
    if (degrees.empty()) return note;
    
    const uint8_t noteInOctave = note % 12;
    const uint8_t octave = note / 12;
    
    // Find first degree greater than note
    auto it = std::upper_bound(degrees.begin(), degrees.end(), noteInOctave);
    if (it == degrees.begin()) {
        // Wrap to previous octave
        return (octave - 1) * 12 + degrees.back();
    }
    --it;
    return octave * 12 + *it;
}

uint8_t Scale::quantizeNearest(uint8_t note) const {
    if (degrees.empty()) return note;
    
    const uint8_t up = quantizeUp(note);
    const uint8_t down = quantizeDown(note);
    
    return (up - note < note - down) ? up : down;
}

uint8_t Scale::quantizeRandom(uint8_t note) const {
    if (degrees.empty()) return note;
    
    const uint8_t up = quantizeUp(note);
    const uint8_t down = quantizeDown(note);
    
    return juce::Random::getSystemRandom().nextBool() ? up : down;
}

void Scale::updateDegrees() {
    const uint8_t* scaleData;
    size_t scaleSize;
    
    switch (type) {
        case Type::Major:
            scaleData = MAJOR_SCALE.data();
            scaleSize = MAJOR_SCALE.size();
            break;
        case Type::Minor:
            scaleData = MINOR_SCALE.data();
            scaleSize = MINOR_SCALE.size();
            break;
        case Type::HarmonicMinor:
            scaleData = HARMONIC_MINOR.data();
            scaleSize = HARMONIC_MINOR.size();
            break;
        case Type::MelodicMinor:
            scaleData = MELODIC_MINOR.data();
            scaleSize = MELODIC_MINOR.size();
            break;
        case Type::Dorian:
            scaleData = DORIAN.data();
            scaleSize = DORIAN.size();
            break;
        case Type::Phrygian:
            scaleData = PHRYGIAN.data();
            scaleSize = PHRYGIAN.size();
            break;
        case Type::Lydian:
            scaleData = LYDIAN.data();
            scaleSize = LYDIAN.size();
            break;
        case Type::Mixolydian:
            scaleData = MIXOLYDIAN.data();
            scaleSize = MIXOLYDIAN.size();
            break;
        case Type::Locrian:
            scaleData = LOCRIAN.data();
            scaleSize = LOCRIAN.size();
            break;
        case Type::PentatonicMajor:
            scaleData = PENTATONIC_MAJOR.data();
            scaleSize = PENTATONIC_MAJOR.size();
            break;
        case Type::PentatonicMinor:
            scaleData = PENTATONIC_MINOR.data();
            scaleSize = PENTATONIC_MINOR.size();
            break;
        case Type::Blues:
            scaleData = BLUES.data();
            scaleSize = BLUES.size();
            break;
        case Type::Chromatic:
            scaleData = CHROMATIC.data();
            scaleSize = CHROMATIC.size();
            break;
        case Type::Custom:
            return; // Keep existing custom degrees
    }
    
    degrees = applyRootToScale(scaleData, scaleSize, root);
}

std::vector<uint8_t> Scale::applyRootToScale(const uint8_t* scale, size_t size, uint8_t root) {
    std::vector<uint8_t> result;
    result.reserve(size);
    
    for (size_t i = 0; i < size; ++i) {
        result.push_back((scale[i] + root) % 12);
    }
    
    std::sort(result.begin(), result.end());
    return result;
}

} // namespace sirkus