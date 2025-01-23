#pragma once

#include <array>
#include <string>
#include <vector>

namespace sirkus {

class Scale {
public:
    // Common scale types
    enum class Type {
        Major,
        Minor,
        HarmonicMinor,
        MelodicMinor,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Locrian,
        PentatonicMajor,
        PentatonicMinor,
        Blues,
        Chromatic,
        Custom
    };
    
    // Predefined scales (intervals from root)
    static constexpr std::array<uint8_t, 7> MAJOR_SCALE = {0, 2, 4, 5, 7, 9, 11};
    static constexpr std::array<uint8_t, 7> MINOR_SCALE = {0, 2, 3, 5, 7, 8, 10};
    static constexpr std::array<uint8_t, 7> HARMONIC_MINOR = {0, 2, 3, 5, 7, 8, 11};
    static constexpr std::array<uint8_t, 7> MELODIC_MINOR = {0, 2, 3, 5, 7, 9, 11};
    static constexpr std::array<uint8_t, 7> DORIAN = {0, 2, 3, 5, 7, 9, 10};
    static constexpr std::array<uint8_t, 7> PHRYGIAN = {0, 1, 3, 5, 7, 8, 10};
    static constexpr std::array<uint8_t, 7> LYDIAN = {0, 2, 4, 6, 7, 9, 11};
    static constexpr std::array<uint8_t, 7> MIXOLYDIAN = {0, 2, 4, 5, 7, 9, 10};
    static constexpr std::array<uint8_t, 7> LOCRIAN = {0, 1, 3, 5, 6, 8, 10};
    static constexpr std::array<uint8_t, 5> PENTATONIC_MAJOR = {0, 2, 4, 7, 9};
    static constexpr std::array<uint8_t, 5> PENTATONIC_MINOR = {0, 3, 5, 7, 10};
    static constexpr std::array<uint8_t, 6> BLUES = {0, 3, 5, 6, 7, 10};
    static constexpr std::array<uint8_t, 12> CHROMATIC = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    
    Scale();
    explicit Scale(Type type, uint8_t root = 0);
    Scale(const std::vector<uint8_t>& degrees, uint8_t root = 0);
    
    // Scale manipulation
    void setType(Type type);
    void setRoot(uint8_t root);
    void setCustomDegrees(const std::vector<uint8_t>& degrees);
    
    // Scale info
    Type getType() const { return type; }
    uint8_t getRoot() const { return root; }
    const std::vector<uint8_t>& getDegrees() const { return degrees; }
    std::string getName() const;
    
    // Note quantization
    uint8_t quantizeUp(uint8_t note) const;
    uint8_t quantizeDown(uint8_t note) const;
    uint8_t quantizeNearest(uint8_t note) const;
    uint8_t quantizeRandom(uint8_t note) const;
    
private:
    void updateDegrees();
    static std::vector<uint8_t> applyRootToScale(const uint8_t* scale, size_t size, uint8_t root);
    
    Type type{Type::Major};
    uint8_t root{0};  // 0-11 representing C through B
    std::vector<uint8_t> degrees;  // Current scale degrees with root applied
};

} // namespace sirkus