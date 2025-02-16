// In ValueTreeView.h
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../core/ValueTreeObject.h"

namespace Sirkus::UI {

template<typename T>
   requires std::is_base_of_v<Core::ValueTreeObject, T>
class ValueTreeView : public juce::Component,
                    public juce::ValueTree::Listener {
protected:
    T& model;

public:
    explicit ValueTreeView(T& modelRef)
        : model(modelRef)
    {
        model.getState().addListener(this);
    }

    ~ValueTreeView() override
    {
        model.getState().removeListener(this);
    }

    // Default implementations for ValueTree::Listener
    void valueTreePropertyChanged(juce::ValueTree& tree,
                                const juce::Identifier& property) override
    {
        repaint();
    }

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override {}
    void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override {}
    void valueTreeChildOrderChanged(juce::ValueTree&, int, int) override {}
    void valueTreeParentChanged(juce::ValueTree&) override {}

    T& getModel() { return model; }
    const T& getModel() const { return model; }
};

} // namespace Sirkus::UI
