/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef __NATPLUGIN3EDITOR_H_136829C6__
#define __NATPLUGIN3EDITOR_H_136829C6__

#include <EditorHeaders.h>

class DetectorInterface;
class NatPlugin3;
class ElectrodeButton;

/**

  User interface for the NatPlugin3 processor.

  @see NatPlugin3

*/

class NatPlugin3Editor : public GenericEditor,
    public ComboBox::Listener
{
public:
    NatPlugin3Editor(GenericProcessor* parentNode, bool useDefaultParameterEditors);

    virtual ~NatPlugin3Editor();

    void buttonEvent(Button* button);

    void comboBoxChanged(ComboBox* c);

    void updateSettings();

    void saveCustomParameters(XmlElement* xml);
    void loadCustomParameters(XmlElement* xml);

	void startAcquisition() override;
	void stopAcquisition() override;

private:

    ScopedPointer<ComboBox> detectorSelector;

    ScopedPointer<UtilityButton> plusButton;

    void addDetector();

    // ScopedPointer<ComboBox> inputChannelSelectionBox;
    // ScopedPointer<ComboBox> outputChannelSelectionBox;

    // ScopedPointer<Label> intputChannelLabel;
    // ScopedPointer<Label> outputChannelLabel;

    OwnedArray<DetectorInterface> interfaces;

    int previousChannelCount;

    Array<Colour> backgroundColours;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NatPlugin3Editor);

};

class DetectorInterface : public Component,
    public ComboBox::Listener,
    public Button::Listener
{
public:
    DetectorInterface(NatPlugin3*, Colour, int);
    ~DetectorInterface();

    void paint(Graphics& g);

    void comboBoxChanged(ComboBox*);
    void buttonClicked(Button*);

    void updateChannels(int);

    void setPhase(int);
    void setInputChan(int);
    void setOutputChan(int);
    void setGateChan(int);

    int getPhase();
    int getInputChan();
    int getOutputChan();
    int getGateChan();

	void setEnableStatus(bool status);

private:

    Colour backgroundColour;

    Path sineWave;
    Font font;

    int idNum;

    NatPlugin3* processor;

    OwnedArray<ElectrodeButton> phaseButtons;

    ScopedPointer<ComboBox> inputSelector;
    ScopedPointer<ComboBox> gateSelector;
    ScopedPointer<ComboBox> outputSelector;

};

#endif  // __NATPLUGIN3EDITOR_H_136829C6__
