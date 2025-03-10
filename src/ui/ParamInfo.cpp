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

#include "ui/ParamInfo.h"

// This code defines which parameters get added to the AudioProcessorValueTreeState.

std::map<juce::String, std::vector<ParamInfo>> getSliderParamsInfo()
{
    // map with a string key and ParamInfo (structure) value
    static const std::map<juce::String, std::vector<ParamInfo>> ret = {
        // Model control params.
        { "Controls",
          { { "PitchShift", "Pitch Shift", 0, 65532, 0, 1, true, 1, "cents" },              // ParamInfo struct defined in ParamInfo.h
            { "HarmonicGain", "Harmonics", 0.0f, 1.0f, 1.0f, 0.01f, false, 2, "" },
            { "NoiseGain", "Noise", 0.0f, 1.0f, 1.0f, 0.01f, false, 2, "" },
            { "OutputGain", "Output Gain", -60.0f, 0.0f, 0.0f, 0.01f, false, 2, "dB" } } },
#if JucePlugin_IsSynth
        // Synth mode params. (Only an option for the Synth plugin)
        { "Envelope",
          { { "Attack", "Attack", 0.1f, 3.0f, 0.1f, 0.01f, false, 2, "s" },
            { "Decay", "Decay", 0.1f, 3.0f, 0.0f, 0.01f, false, 2, "s" },
            { "Sustain", "Sustain", 0.0f, 1.0f, 1.0f, 0.01f, false, 2, "" },
            { "Release", "Release", 0.1f, 5.0f, 1.2f, 0.01f, false, 2, "s" } } },
#endif
        { "Effects",
          { { "ReverbSize", "Reverb Size", 0.0f, 1.0f, 0.4f, 0.01f, false, 2, "" },
            { "ReverbDamping", "Reverb Damp", 0.0f, 1.0f, 0.1f, 0.01f, false, 2, "" },
            { "ReverbWet", "Reverb Wet", 0.0f, 1.0f, 0.0f, 0.01f, false, 2, "" } } }
    };
    return ret;
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

        // Add input gain and input pitch parameters
    layout.add (std::make_unique<juce::AudioParameterFloat> ("InputGain", "Input Gain", -0.5f, 0.5f, 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("InputPitch", "Input Pitch", -0.5f, 0.5f, 0.0f));
    
    // Add instrument choice parameter

    juce::StringArray instrumentList;
    instrumentList.add ("Flute");
    instrumentList.add ("Violin");
    instrumentList.add ("Trumpet");
    instrumentList.add ("Saxophone");
    instrumentList.add ("Bassoon");
    instrumentList.add ("Clarinet");
    instrumentList.add ("Melodica");
    instrumentList.add ("Sitar");
    instrumentList.add ("Trombone");
    instrumentList.add ("Tuba");
    instrumentList.add ("Vowels");
    
    layout.add (std::make_unique<juce::AudioParameterChoice> ("InstrumentChoice", "Instrument Choice", instrumentList, 0));
    

    // Scene-related params.
    const auto paramInfos = getSliderParamsInfo();          // auto: automatic type, returns the map created in                                                                        getSliderParamsInfo()
    
    // Iterate over each key-value pair in paramInfos
    for (const auto& [groupName, infos] : paramInfos)
    {
        // Iterate over element in the ParamInfo structures
        for (const auto& info : infos)
        {
            // If the parameter is an integer value, add an AudioParameterInt parameter, if not add an AudioParameterFloat
            if (info.isIntParam)
            {
                layout.add (std::make_unique<juce::AudioParameterInt> (info.paramID,
                                                                       info.paramName,
                                                                       static_cast<int> (info.rangeMin),
                                                                       static_cast<int> (info.rangeMax),
                                                                       static_cast<int> (info.defaultValue)));
            }
            else
            {
                layout.add (std::make_unique<juce::AudioParameterFloat> (
                    info.paramID, info.paramName, info.rangeMin, info.rangeMax, info.defaultValue));
            }
        }
    }

    return layout;  // returns a ParameterLayout type to be used to initialise the AudioProcessorValueTreeState in                                PluginProcessor.cpp
}
