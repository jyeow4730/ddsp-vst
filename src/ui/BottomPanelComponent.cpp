/*
Copyright 2022 The DDSP-VST Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "BottomPanelComponent.h"

// constructor - initialise the audio processor in the member initialiser list
// 
BottomPanelComponent::BottomPanelComponent (DDSPAudioProcessor& p) : audioProcessor (p)
{
    std::vector<juce::String> groupNames;                   // group names: Controls, Effect, Envelope
    const auto paramInfos = getSliderParamsInfo();          // from ParamInfo.cpp
    for (const auto& [groupName, infos] : paramInfos)       // e.g. Controls, Info
    {
        groupNames.push_back (groupName);                   // append groupName to the groupNames vector
        sliderGroups[groupName] = std::vector<std::unique_ptr<SliderWithDynamicLabel>>();
        //sliderGroups is a map from groupName to a vector of pointers to sliders with dynamic labels
        // create an empty vector to fill
        
        for (const auto& info : infos)  // for each sub parameter e.g. Pitch Shift, Harmonics etc
        {
            auto slider = std::make_unique<SliderWithDynamicLabel> (info);  // pointer to the slider for info
            addChildComponent (slider.get());                               // add the slider to the UI

            // create slider attachment - connect slider to value in the AudioProcessorValueTreeState
            auto sliderAttach =
                std::make_unique<SliderAttach> (audioProcessor.getValueTree(), info.paramID, slider->getSlider());

            sliderGroups[groupName].push_back (std::move (slider));     // append the slider pointer into sliderGroups
            sliderAttachments.push_back (std::move (sliderAttach));     // append the slider attachment pointer into                                                                            sliderAttachments
        }
    }
    buttonGroup.reset (new RadioButtonGroupGomponent (groupNames));     // reset the radio button group
    addAndMakeVisible (buttonGroup.get());
    buttonGroup->addListener (this);
                                            // addListener defined in RadioButtonGroupComponent.h
    jassert (! groupNames.empty());
    showSliderGroup (groupNames[0]);        // show slider group 'Controls' as the default.
}

BottomPanelComponent::~BottomPanelComponent()
{
    sliderAttachments.clear();
    sliderGroups.clear();

    buttonGroup->removeListener (this);
    buttonGroup = nullptr;
}

void BottomPanelComponent::paint (juce::Graphics& g)
{
    juce::Rectangle<int> localArea (getLocalBounds());
    localArea = localArea.reduced (kPadding, kPadding / 2);
    juce::Path footer;
    footer.addRoundedRectangle (localArea.getX(),
                                localArea.getY(),
                                localArea.getWidth(),
                                localArea.getHeight(),
                                kRoundedRectangleCornerSize,
                                kRoundedRectangleCornerSize,
                                true,
                                true,
                                true,
                                true);
    juce::DropShadow dsBottom (juce::Colours::black.withMultipliedAlpha (0.4f), 4, juce::Point<int> (0, 0));
    dsBottom.drawForPath (g, footer);
    g.setColour (juce::Colour (DDSPColourPalette::kGrey));
    g.fillPath (footer);
}

void BottomPanelComponent::resized()
{
    auto localArea = getLocalBounds();
    buttonGroup->setBounds (localArea.removeFromBottom (48));

    const float sliderWidth = 128.0f;

    for (const auto& [groupName, sliders] : sliderGroups)
    {
        juce::FlexBox knobBox;
        knobBox.flexWrap = juce::FlexBox::Wrap::noWrap;
        knobBox.flexDirection = juce::FlexBox::Direction::row;
        knobBox.justifyContent = juce::FlexBox::JustifyContent::center;

        for (const auto& slider : sliders)
        {
            knobBox.items.add (juce::FlexItem (*slider)
                                   .withWidth (sliderWidth)
                                   .withHeight (sliderWidth)
                                   .withAlignSelf (juce::FlexItem::AlignSelf::center));
        }
        knobBox.performLayout (localArea.toFloat());
    }
}

void BottomPanelComponent::selectionChanged (const juce::String& buttonName) { showSliderGroup (buttonName); }

void BottomPanelComponent::showSliderGroup (const juce::String& groupName)
{
    if (auto it { sliderGroups.find (groupName) }; it != std::end (sliderGroups))
    {
        // Show sliders.
        for (const auto& slider : it->second)
        {
            slider->setVisible (true);
        }
        // Hide other sliders.
        for (const auto& [gName, sliders] : sliderGroups)
        {
            if (gName != groupName)
            {
                for (const auto& slider : sliders)
                {
                    slider->setVisible (false);
                }
            }
        }
    }
    else
        jassert (0 && "slider group not found");
}
