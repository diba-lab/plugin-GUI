/*
	------------------------------------------------------------------

	This file is part of the Open Ephys GUI
	Copyright (C) 2016 Open Ephys

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
#include "NatPlugin.h"
#include "../../UI/UIComponent.h"
#include "../../AccessClass.h"

#include <exception>


const String NatPlugin::m_unusedNameString("xxx-UNUSED-OPEN-EPHYS-xxx");

NatPlugin::NatPlugin(const String& name)
	: sourceNode(0)
	, destNode(0)
	, isEnabled(true)
	, wasConnected(false)
	, nextAvailableChannel(0)
	, saveOrder(-1)
	, loadOrder(-1)
	, currentChannel(-1)
	, editor(nullptr)
	, parametersAsXml(nullptr)
	, sendSampleCount(true)
	, m_processorType(PROCESSOR_TYPE_UTILITY)
	, m_name(name)
	, m_isParamsWereLoaded(false)
{
	settings.numInputs = settings.numOutputs = 0;
	m_lastProcessTime = Time::getHighResolutionTicks();
}


NatPlugin::~NatPlugin()
{
}


AudioProcessorEditor* NatPlugin::createEditor()
{
	editor = new GenericEditor(this, true);
	return editor;
}


void NatPlugin::setNodeId(int id)
{
	nodeId = id;

	if (editor != 0)
	{
		editor->updateName();
	}
}


Parameter* NatPlugin::getParameterByName(String name)
{
	const int numParameters = getNumParameters();
	// doesn't work
	for (int i = 0; i < numParameters; ++i)
	{
		const auto parameter = parameters[i];
		const String parameterName = parameter->getName();

		if (parameterName.compare(name) == 0) // fails at this point
			return parameter;//parameters.getReference(i);
	}

	Parameter* nullParam = new Parameter("VOID", false, -1);

	return nullParam;
}


Parameter* NatPlugin::getParameterObject(int parameterIndex) const
{
	return parameters[parameterIndex];
}


void NatPlugin::setParameter(int parameterIndex, float newValue)
{
	editor->updateParameterButtons(parameterIndex);
	std::cout << "Setting parameter" << std::endl;

	if (currentChannel >= 0)
		parameters[parameterIndex]->setValue(newValue, currentChannel);
}


const String NatPlugin::getParameterName(int parameterIndex)
{
	return parameters[parameterIndex]->getName();
}


const String NatPlugin::getParameterText(int parameterIndex)
{
	return parameters[parameterIndex]->getDescription();
}


var NatPlugin::getParameterVar(int parameterIndex, int parameterChannel)
{
	const auto parameter = parameters[parameterIndex];
	return parameter->operator[] (parameterChannel);
}


void NatPlugin::prepareToPlay(double sampleRate_, int estimatedSamplesPerBlock)
{
}


void NatPlugin::releaseResources()
{
	// use the disable() function instead
	// releaseResources() is called by Juce at unpredictable times
	// disable() is only called by the ProcessorGraph at the end of acquisition
}


int NatPlugin::getNextChannel(bool increment)
{
	int chan = nextAvailableChannel;

	//std::cout << "Next channel: " << chan << ", num inputs: " << getNumInputs() << std::endl;

	if (increment)
		nextAvailableChannel++;

	if (chan < getNumInputs())
		return chan;
	else
		return -1;
}


void NatPlugin::resetConnections()
{
	nextAvailableChannel = 0;

	wasConnected = false;
}


void NatPlugin::setSourceNode(NatPlugin* sn)
{
	//std::cout << "My name is " << getName() << ". Setting source node." << std::endl;

	if (!isSource())
	{
		//	std::cout << " I am not a source." << std::endl;

		if (sn != 0)
		{
			//	std::cout << " The source is not blank." << std::endl;

			if (!sn->isSink())
			{
				//		std::cout << " The source is not a sink." << std::endl;
				if (sourceNode != sn)
				{
					//			std::cout << " The source is new and named " << sn->getName() << std::endl;

					if (this->isMerger())
						setMergerSourceNode(sn);
					else
						sourceNode = sn;

					sn->setDestNode(this);
				}
				else
				{
					//			std::cout << "  The source node is not new." << std::endl;
				}
			}
			else
			{
				//		std::cout << " The source is a sink." << std::endl;
				sourceNode = 0;
			}

		}
		else
		{
			//		std::cout << " The source is blank." << std::endl;
			sourceNode = 0;
		}
	}
	else
	{
		//	std::cout << " I am a source. I can't have a source node." << std::endl;

		if (sn != 0)
			sn->setDestNode(this);
	}
}


void NatPlugin::setDestNode(NatPlugin* dn)
{
	//	std::cout << "My name is " << getName() << ". Setting dest node." << std::endl;

	if (!isSink())
	{
		//	std::cout << "  I am not a sink." << std::endl;

		if (dn != 0)
		{
			//		std::cout << "  The dest node is not blank." << std::endl;
			if (!dn->isSource())
			{
				//		std::cout << "  The dest node is not a source." << std::endl;

				if (destNode != dn)
				{
					//		std::cout << "  The dest node is new and named " << dn->getName() << std::endl;
					//
					if (this->isSplitter())
						setSplitterDestNode(dn);
					else
						destNode = dn;

					dn->setSourceNode(this);
				}
				else
				{
					//		std::cout << "  The dest node is not new." << std::endl;
				}
			}
			else
			{
				//	std::cout << "  The dest node is a source." << std::endl;

				destNode = 0;
			}
		}
		else
		{
			//	std::cout << "  The dest node is blank." << std::endl;

			destNode = 0;
		}
	}
	else
	{
		//std::cout << "  I am a sink, I can't have a dest node." << std::endl;
		//if (dn != 0)
		//	dn->setSourceNode(this);
	}
}


void NatPlugin::clearSettings()
{
	//std::cout << "Generic processor clearing settings." << std::endl;

	settings.originalSource = 0;
	settings.numInputs = 0;
	settings.numOutputs = 0;

	// std::cout << "Record status size = " << recordStatus.size() << std::endl;

	if (m_recordStatus.size() < dataChannelArray.size())
		m_recordStatus.resize(dataChannelArray.size());

	if (m_monitorStatus.size() < dataChannelArray.size())
		m_monitorStatus.resize(dataChannelArray.size());

	for (int i = 0; i < dataChannelArray.size(); ++i)
	{
		// std::cout << channels[i]->getRecordState() << std::endl;
		m_recordStatus.set(i, dataChannelArray[i]->getRecordState());
		m_monitorStatus.set(i, dataChannelArray[i]->isMonitored());
	}

	dataChannelArray.clear();
	eventChannelArray.clear();
	spikeChannelArray.clear();
	configurationObjectArray.clear();
	clearChannelCreationCounts();
}


void NatPlugin::update()
{
	std::cout << getName() << " updating settings." << std::endl;

	// ---- RESET EVERYTHING ---- ///
	clearSettings();

	if (sourceNode != 0) // copy settings from source node
	{
		// everything is inherited except numOutputs
		settings = sourceNode->settings;
		settings.numInputs = settings.numOutputs;
		settings.numOutputs = settings.numInputs;

		for (int i = 0; i < sourceNode->dataChannelArray.size(); ++i)
		{
			DataChannel* sourceChan = sourceNode->dataChannelArray[i];
			DataChannel* ch = new DataChannel(*sourceChan);


			if (i < m_recordStatus.size())
			{
				ch->setRecordState(m_recordStatus[i]);
				ch->setMonitored(m_monitorStatus[i]);
			}

			ch->addToHistoricString(getName());
			dataChannelArray.add(ch);
		}

		for (int i = 0; i < sourceNode->eventChannelArray.size(); ++i)
		{
			EventChannel* sourceChan = sourceNode->eventChannelArray[i];
			EventChannel* ch = new EventChannel(*sourceChan);
			ch->eventMetaDataLock = true;
			eventChannelArray.add(ch);
		}
		for (int i = 0; i < sourceNode->spikeChannelArray.size(); ++i)
		{
			SpikeChannel* sourceChan = sourceNode->spikeChannelArray[i];
			SpikeChannel* ch = new SpikeChannel(*sourceChan);
			ch->eventMetaDataLock = true;
			spikeChannelArray.add(ch);

		}
		for (int i = 0; i < sourceNode->configurationObjectArray.size(); ++i)
		{
			ConfigurationObject* sourceChan = sourceNode->configurationObjectArray[i];
			ConfigurationObject* ch = new ConfigurationObject(*sourceChan);
			configurationObjectArray.add(ch);
		}
	}
	else // generate new settings
	{

		createDataChannels(); //Only sources can create data channels
		settings.numOutputs = dataChannelArray.size();
		std::cout << getName() << " setting num outputs to " << settings.numOutputs << std::endl;

		for (int i = 0; i < dataChannelArray.size(); i++)
		{
            if (i < m_recordStatus.size())
            {
                dataChannelArray[i]->setRecordState(m_recordStatus[i]);
                dataChannelArray[i]->setMonitored(m_monitorStatus[i]);
            }
            else if (isSource())
            {
                dataChannelArray[i]->setRecordState(true);
            }
		}
	}

	//Any processor, not only sources, can add new event and spike channels. It's best to do it in their dedicated methods
	createEventChannels();
	createSpikeChannels();
	createConfigurationObjects();

	if (this->isSink())
	{
		settings.numOutputs = 0;
	}

	updateSettings(); // allow processors to change custom settings

	updateChannelIndexes();

	m_needsToSendTimestampMessages.clear();
	m_needsToSendTimestampMessages.insertMultiple(-1, false, getNumSubProcessors());

	// required for the ProcessorGraph to know the
	// details of this processor:
	setPlayConfigDetails(getNumInputs(),  // numIns
		getNumOutputs(), // numOuts
		44100.0,         // sampleRate
		128);            // blockSize

	editor->update(); // allow the editor to update its settings
}

void NatPlugin::updateChannelIndexes(bool updateNodeID)
{
	//Recreate the channel indexes
	dataChannelMap.clear();
	eventChannelMap.clear();
	spikeChannelMap.clear();
	unsigned int nChans;

	nChans = dataChannelArray.size();
	for (int i = 0; i < nChans; i++)
	{
		DataChannel* channel = dataChannelArray[i];
		if (updateNodeID)
		{
			channel->m_nodeID = nodeId;
			channel->m_nodeIdx = i;
			channel->m_currentNodeName = getName();
			channel->m_currentNodeType = getName(); //Fix when the ability to name individual processors is implemented
		}
		uint32 sourceID = getProcessorFullId(channel->getSourceNodeID(), channel->getSubProcessorIdx());
		dataChannelMap[sourceID][channel->getSourceIndex()] = i;
	}
	nChans = eventChannelArray.size();
	for (int i = 0; i < nChans; i++)
	{
		EventChannel* channel = eventChannelArray[i];
		if (updateNodeID)
		{
			channel->m_nodeID = nodeId;
			channel->m_nodeIdx = i;
			channel->m_currentNodeName = getName();
			channel->m_currentNodeType = getName(); //Fix when the ability to name individual processors is implemented
		}
		uint32 sourceID = getProcessorFullId(channel->getSourceNodeID(), channel->getSubProcessorIdx());
		eventChannelMap[sourceID][channel->getSourceIndex()] = i;
	}
	nChans = spikeChannelArray.size();
	for (int i = 0; i < nChans; i++)
	{
		SpikeChannel* channel = spikeChannelArray[i];
		if (updateNodeID)
		{
			channel->m_nodeID = nodeId;
			channel->m_nodeIdx = i;
			channel->m_currentNodeName = getName();
			channel->m_currentNodeType = getName(); //Fix when the ability to name individual processors is implemented
		}
		uint32 sourceID = getProcessorFullId(channel->getSourceNodeID(), channel->getSubProcessorIdx());
		spikeChannelMap[sourceID][channel->getSourceIndex()] = i;
	}
}

void NatPlugin::createDataChannels()
{
	createDataChannelsByType(DataChannel::HEADSTAGE_CHANNEL);
	createDataChannelsByType(DataChannel::AUX_CHANNEL);
	createDataChannelsByType(DataChannel::ADC_CHANNEL);
}

void NatPlugin::createDataChannelsByType(DataChannel::DataChannelTypes type)
{
	int nSub = getNumSubProcessors();
	for (int sub = 0; sub < nSub; sub++)
	{
		int nChans = getDefaultNumDataOutputs(type, sub);
		for (int i = 0; i < nChans; i++)
		{
			DataChannel* chan = new DataChannel(type, getSampleRate(sub), this, sub);
			chan->setBitVolts(getBitVolts(sub));
			chan->addToHistoricString(getName());
			chan->m_nodeID = nodeId;
			dataChannelArray.add(chan);
		}
	}
}

void NatPlugin::createEventChannels()
{
	int nSub = getNumSubProcessors();
	for (int sub = 0; sub < nSub; sub++)
	{
		Array<DefaultEventInfo> events;
		getDefaultEventInfo(events, sub);
		int nChans = events.size();
		for (int i = 0; i < nChans; i++)
		{
			const DefaultEventInfo& info = events[i];
			if (info.type != EventChannel::INVALID && info.nChannels > 0 && info.length > 0)
			{
				EventChannel* chan = new EventChannel(info.type, info.nChannels, info.length, info.sampleRate, this, sub);
				chan->m_nodeID = nodeId;
				if (info.name.isNotEmpty())
					chan->setName(info.name);
				if (info.description.isNotEmpty())
					chan->setDescription(info.description);
				if (info.identifier.isNotEmpty())
					chan->setIdentifier(info.identifier);
				eventChannelArray.add(chan);
			}
		}
	}
}

void NatPlugin::getDefaultEventInfo(Array<DefaultEventInfo>& events, int subproc) const
{
	events.clear();
}

void NatPlugin::createSpikeChannels() {};
void NatPlugin::createConfigurationObjects() {};

void NatPlugin::setAllChannelsToRecord()
{
	m_recordStatus.resize(dataChannelArray.size());

	for (int i = 0; i < dataChannelArray.size(); ++i)
	{
		m_recordStatus.set(i, true);
	}

	// std::cout << "Setting all channels to record for source." << std::endl;
}


void NatPlugin::setRecording(bool state)
{
	GenericEditor* ed = getEditor();
	if (state)
	{
		if (ed != 0)
			ed->startRecording();

		startRecording();
		if (isGeneratesTimestamps())
		{
			m_needsToSendTimestampMessages.clearQuick();
			m_needsToSendTimestampMessages.insertMultiple(-1, true, getNumSubProcessors());
		}
	}
	else
	{
		if (ed != 0)
			ed->stopRecording();

		stopRecording();
		m_needsToSendTimestampMessages.clearQuick();
		m_needsToSendTimestampMessages.insertMultiple(-1, false, getNumSubProcessors());
	}
}


void NatPlugin::enableEditor()
{
	GenericEditor* ed = getEditor();

	if (ed != 0)
		ed->editorStartAcquisition();
}


void NatPlugin::disableEditor()
{
	GenericEditor* ed = getEditor();

	if (ed != nullptr)
		ed->editorStopAcquisition();
}


/** Used to get the number of samples in a given buffer, for a given channel. */
uint32 NatPlugin::getNumSamples(int channelNum) const
{
	int sourceNodeId = 0;
	int subProcessorId = 0;
	int nSamples = 0;

	if (channelNum >= 0
		&& channelNum < dataChannelArray.size())
	{
		sourceNodeId = dataChannelArray[channelNum]->getSourceNodeID();
		subProcessorId = dataChannelArray[channelNum]->getSubProcessorIdx();
	}
	else
	{
		return 0;
	}

	// std::cout << "Requesting samples for channel " << channelNum << " with source node " << sourceNodeId << std::endl;
	uint32 sourceID = getProcessorFullId(sourceNodeId, subProcessorId);
	try
	{
		nSamples = numSamples.at(sourceID);
	}
	catch (...)
	{
		return 0;
	}

	//std::cout << nSamples << " were found." << std::endl;

	return nSamples;
}


/** Used to get the timestamp for a given buffer, for a given source node. */
juce::uint64 NatPlugin::getTimestamp(int channelNum) const
{
	int sourceNodeId = 0;
	int subProcessorIdx = 0;
	int64 ts = 0;

	if (channelNum >= 0
		&& channelNum < dataChannelArray.size())
	{
		sourceNodeId = dataChannelArray[channelNum]->getSourceNodeID();
		subProcessorIdx = dataChannelArray[channelNum]->getSubProcessorIdx();
	}
	else
	{
		return 0;
	}

	uint32 sourceID = getProcessorFullId(sourceNodeId, subProcessorIdx);
	try
	{
		ts = timestamps.at(sourceID);
	}
	catch (...)
	{
		return 0;
	}

	return ts;
}

uint32 NatPlugin::getNumSourceSamples(uint16 processorID, uint16 subProcessorIdx) const
{
	return getNumSourceSamples(getProcessorFullId(processorID, subProcessorIdx));
}

uint32 NatPlugin::getNumSourceSamples(uint32 fullSourceID) const
{
	uint32 nSamples;
	try
	{
		nSamples = numSamples.at(fullSourceID);
	}
	catch (...)
	{
		return 0;
	}
	return nSamples;
}

juce::uint64 NatPlugin::getSourceTimestamp(uint16 processorID, uint16 subProcessorIdx) const
{
	return getSourceTimestamp(getProcessorFullId(processorID, subProcessorIdx));
}

juce::uint64 NatPlugin::getSourceTimestamp(uint32 fullSourceID) const
{
	juce::uint64 ts;
	try
	{
		ts = timestamps.at(fullSourceID);
	}
	catch (...)
	{
		return 0;
	}
	return ts;
}


/** Used to set the timestamp for a given buffer, for a given channel. */
void NatPlugin::setTimestampAndSamples(juce::uint64 timestamp, uint32 nSamples, int subProcessorIdx)
{

	MidiBuffer& eventBuffer = *m_currentMidiBuffer;
	//std::cout << "Setting timestamp to " << timestamp << std:;endl;

	HeapBlock<char> data;
	size_t dataSize = SystemEvent::fillTimestampAndSamplesData(data, this, subProcessorIdx, timestamp, nSamples);


	eventBuffer.addEvent(data, dataSize, 0);

	uint32 sourceID = getProcessorFullId(nodeId, subProcessorIdx);

	//since the processor generating the timestamp won't get the event, add it to the map
	timestamps[sourceID] = timestamp;
	numSamples[sourceID] = nSamples;

	if (m_needsToSendTimestampMessages[subProcessorIdx] && nSamples > 0)
	{
		HeapBlock<char> data;
		size_t dataSize = SystemEvent::fillTimestampSyncTextData(data, this, subProcessorIdx, timestamp, false);

		eventBuffer.addEvent(data, dataSize, 0);

		m_needsToSendTimestampMessages.set(subProcessorIdx, false);
	}
}


int NatPlugin::processEventBuffer()
{
	//
	// This loops through all events in the buffer, and uses the BUFFER_SIZE
	// events to determine the number of samples in the current buffer. If
	// there are multiple such events, the one with the highest number of
	// samples will be used.
	// This approach is not ideal, as it will become a problem if we allow
	// the sample rate to change at different points in the signal chain.
	//
	int numRead = 0;

	MidiBuffer& eventBuffer = *m_currentMidiBuffer;

	if (eventBuffer.getNumEvents() > 0)
	{
		MidiBuffer::Iterator i(eventBuffer);

		const uint8* dataptr;
		int dataSize;

		int samplePosition = -1;

		while (i.getNextEvent(dataptr, dataSize, samplePosition))
		{
			//TODO: remove the mask when the probe system is implemented
			if (static_cast<EventType>(*(dataptr + 0) & 0x7F) == SYSTEM_EVENT && static_cast<SystemEventType>(*(dataptr + 1) == TIMESTAMP_AND_SAMPLES))
			{
				uint16 sourceNodeID = *reinterpret_cast<const uint16*>(dataptr + 2);
				uint16 sourceSubProcessorIdx = *reinterpret_cast<const uint16*>(dataptr + 4);
				uint32 sourceID = getProcessorFullId(sourceNodeID, sourceSubProcessorIdx);

				juce::uint64 timestamp = *reinterpret_cast<const juce::uint64*>(dataptr + 8);
				uint32 nSamples = *reinterpret_cast<const uint32*>(dataptr + 16);
				numSamples[sourceID] = nSamples;
				timestamps[sourceID] = timestamp;
			}
			//set the "recorded" bit on the first byte. This will go away when the probe system is implemented.
			//doing a const cast is always a bad idea, but there's no better way to do this until whe change the event record system
			if (nodeId < 900) //If the processor is not a specialized one
				*const_cast<uint8*>(dataptr + 0) = *(dataptr + 0) | 0x80;
		}
	}

	return numRead;
}


int NatPlugin::checkForEvents(bool checkForSpikes)
{
	if (m_currentMidiBuffer->getNumEvents() > 0)
	{
		//Since adding events to the buffer inside this loop could be dangerous, create a temporal event buffer
		//so any call to addEvent will operate on it;
		MidiBuffer temporalEventBuffer;
		MidiBuffer* originalEventBuffer = m_currentMidiBuffer;
		m_currentMidiBuffer = &temporalEventBuffer;
		// int m = midiMessages.getNumEvents();
		//std::cout << m << " events received by node " << getNodeId() << std::endl;

		MidiBuffer::Iterator i(*originalEventBuffer);
		MidiMessage message(0xf4);

		int samplePosition = 0;
		i.setNextSamplePosition(samplePosition);

		while (i.getNextEvent(message, samplePosition))
		{
			uint16 sourceId = EventBase::getSourceID(message);
			uint16 subProc = EventBase::getSubProcessorIdx(message);
			uint16 index = EventBase::getSourceIndex(message);
			if (EventBase::getBaseType(message) == EventType::PROCESSOR_EVENT)
			{
				int eventIndex = getEventChannelIndex(index, sourceId, subProc);
				if (eventIndex >= 0)
					handleEvent(eventChannelArray[eventIndex], message, samplePosition);
			}
			else if (EventBase::getBaseType(message) == EventType::SYSTEM_EVENT && SystemEvent::getSystemEventType(message) == SystemEventType::TIMESTAMP_SYNC_TEXT)
			{
				handleTimestampSyncTexts(message);
			}
			else if (checkForSpikes && EventBase::getBaseType(message) == EventType::SPIKE_EVENT)
			{
				int spikeIndex = getSpikeChannelIndex(index, sourceId, subProc);
				if (spikeIndex >= 0)
					handleSpike(spikeChannelArray[index], message, samplePosition);
			}
		}
		//Restore the original buffer pointer and, if some new event has been added here, copy it to the original buffer
		m_currentMidiBuffer = originalEventBuffer;
		if (temporalEventBuffer.getNumEvents() > 0)
			m_currentMidiBuffer->addEvents(temporalEventBuffer, 0, -1, 0);

		return 0;
	}

	return -1;
}

void NatPlugin::addEvent(int channelIndex, const Event* event, int sampleNum)
{
	addEvent(eventChannelArray[channelIndex], event, sampleNum);
}

void NatPlugin::addEvent(const EventChannel* channel, const Event* event, int sampleNum)
{
	size_t size = channel->getDataSize() + channel->getTotalEventMetaDataSize() + EVENT_BASE_SIZE;
	HeapBlock<char> buffer(size);
	event->serialize(buffer, size);
	m_currentMidiBuffer->addEvent(buffer, size, sampleNum >= 0 ? sampleNum : 0);
}

void NatPlugin::addSpike(int channelIndex, const SpikeEvent* event, int sampleNum)
{
	addSpike(spikeChannelArray[channelIndex], event, sampleNum);
}

void NatPlugin::addSpike(const SpikeChannel* channel, const SpikeEvent* event, int sampleNum)
{
	size_t size = channel->getDataSize() + channel->getTotalEventMetaDataSize() + SPIKE_BASE_SIZE + channel->getNumChannels()*sizeof(float);
	HeapBlock<char> buffer(size);
	event->serialize(buffer, size);
	m_currentMidiBuffer->addEvent(buffer, size, sampleNum >= 0 ? sampleNum : 0);
}


void NatPlugin::processBlock(AudioSampleBuffer& buffer, MidiBuffer& eventBuffer)
{
	m_currentMidiBuffer = &eventBuffer;
	processEventBuffer(); // extract buffer sizes and timestamps,
	// set flag on all TTL events to zero

	m_lastProcessTime = Time::getHighResolutionTicks();
	process(buffer);

}

const DataChannel* NatPlugin::getDataChannel(int index) const
{
	return dataChannelArray[index];
}

const EventChannel* NatPlugin::getEventChannel(int index) const
{
	return eventChannelArray[index];
}

const SpikeChannel* NatPlugin::getSpikeChannel(int index) const
{
	return spikeChannelArray[index];
}

const ConfigurationObject* NatPlugin::getConfigurationObject(int index) const
{
	return configurationObjectArray[index];
}

int NatPlugin::getTotalDataChannels() const
{
	return dataChannelArray.size();
}

int NatPlugin::getTotalEventChannels() const
{
	return eventChannelArray.size();
}

int NatPlugin::getTotalSpikeChannels() const
{
	return spikeChannelArray.size();
}

int NatPlugin::getTotalConfigurationObjects() const
{
	return configurationObjectArray.size();
}

int NatPlugin::getDataChannelIndex(int channelIdx, int processorID, int subProcessorIdx) const
{
	uint32 sourceID = getProcessorFullId(processorID, subProcessorIdx);
	try
	{
		return dataChannelMap.at(sourceID).at(channelIdx);
	}
	catch (...)
	{
		return -1;
	}
}

int NatPlugin::getEventChannelIndex(int channelIdx, int processorID, int subProcessorIdx) const
{
	uint32 sourceID = getProcessorFullId(processorID, subProcessorIdx);
	try
	{
		return eventChannelMap.at(sourceID).at(channelIdx);
	}
	catch (...)
	{
		return -1;
	}
}

int NatPlugin::getEventChannelIndex(const Event* event) const
{
	return getEventChannelIndex(event->getSourceIndex(), event->getSourceID(), event->getSubProcessorIdx());
}

int NatPlugin::getSpikeChannelIndex(int channelIdx, int processorID, int subProcessorIdx) const
{
	uint32 sourceID = getProcessorFullId(processorID, subProcessorIdx);
	try
	{
		return spikeChannelMap.at(sourceID).at(channelIdx);
	}
	catch (...)
	{
		return -1;
	}
}

int NatPlugin::getSpikeChannelIndex(const SpikeEvent* event) const
{
	return getSpikeChannelIndex(event->getSourceIndex(), event->getSourceID(), event->getSubProcessorIdx());
}


/////// ---- LOADING AND SAVING ---- //////////


void NatPlugin::saveToXml(XmlElement* parentElement)
{
	parentElement->setAttribute("NodeId", nodeId);

	saveCustomParametersToXml(parentElement);

	// loop through the channels

	for (int i = 0; i < dataChannelArray.size(); ++i)
	{
		if (!isSplitter() && !isMerger())
			saveChannelParametersToXml(parentElement, i, InfoObjectCommon::DATA_CHANNEL);
	}

	for (int i = 0; i < eventChannelArray.size(); ++i)
	{
		if (!isSplitter() && !isMerger())
			saveChannelParametersToXml(parentElement, i, InfoObjectCommon::EVENT_CHANNEL);
	}

	for (int i = 0; i < spikeChannelArray.size(); ++i)
	{
		if (!isSplitter() && !isMerger())
			saveChannelParametersToXml(parentElement, i, InfoObjectCommon::SPIKE_CHANNEL);
	}

	// Save editor parameters:
	XmlElement* editorChildNode = parentElement->createNewChildElement("EDITOR");
	getEditor()->saveEditorParameters(editorChildNode);
}


void NatPlugin::saveCustomParametersToXml(XmlElement* parentElement)
{
}

void NatPlugin::saveChannelParametersToXml(XmlElement* parentElement, int channelNumber, InfoObjectCommon::InfoObjectType type)
{
	XmlElement* channelInfo;
	if (type == InfoObjectCommon::DATA_CHANNEL)
	{
		channelInfo = parentElement->createNewChildElement("CHANNEL");
		channelInfo->setAttribute("name", String(channelNumber));
		channelInfo->setAttribute("number", channelNumber);

		bool p, r, a;

		getEditor()->getChannelSelectionState(channelNumber, &p, &r, &a);

		XmlElement* selectionState = channelInfo->createNewChildElement("SELECTIONSTATE");
		selectionState->setAttribute("param", p);
		selectionState->setAttribute("record", r);
		selectionState->setAttribute("audio", a);
	}
	else if (type == InfoObjectCommon::EVENT_CHANNEL)
	{
		channelInfo = parentElement->createNewChildElement("EVENTCHANNEL");
		channelInfo->setAttribute("name", String(channelNumber));
		channelInfo->setAttribute("number", channelNumber);

	}
	else if (type == InfoObjectCommon::SPIKE_CHANNEL)
	{
		channelInfo = parentElement->createNewChildElement("SPIKECHANNEL");
		channelInfo->setAttribute("name", String(channelNumber));
		channelInfo->setAttribute("number", channelNumber);
	}
	saveCustomChannelParametersToXml(channelInfo, channelNumber, type);

	// deprecated parameter configuration:
	//std::cout <<"Creating Parameters" << std::endl;
	// int maxsize = parameters.size();
	// String parameterName;
	// String parameterValue;
	// XmlElement* parameterChildNode;

	// // save any attributes that belong to "Parameter" objects
	// for (int n = 0; n < maxsize; n++)
	// {
	//     parameterName = getParameterName(n);

	//     parameterChildNode = channelParent->createNewChildElement("PARAMETER");
	//     parameterChildNode->setAttribute("name", parameterName);

	//     var parameterVar = getParameterVar(n, channelNumber-1);
	//     parameterValue = parameterVar.toString();
	//     parameterChildNode->addTextElement(parameterValue);
	// }
}

void NatPlugin::saveCustomChannelParametersToXml(XmlElement* channelInfo, int channelNum, InfoObjectCommon::InfoObjectType type)
{
}


void NatPlugin::loadFromXml()
{
	update(); // make sure settings are updated
	if (parametersAsXml != nullptr)
	{
        if (!m_isParamsWereLoaded)
        {
            std::cout << "Loading parameters for " << m_name << std::endl;

            // use parametersAsXml to restore state
            loadCustomParametersFromXml();

            // load editor parameters
            forEachXmlChildElement(*parametersAsXml, xmlNode)
            {
                if (xmlNode->hasTagName("EDITOR"))
                {
                    getEditor()->loadEditorParameters(xmlNode);
                }
            }

            forEachXmlChildElement(*parametersAsXml, xmlNode)
            {
                if (xmlNode->hasTagName("CHANNEL"))
                {
                    loadChannelParametersFromXml(xmlNode, InfoObjectCommon::DATA_CHANNEL);
                }
                else if (xmlNode->hasTagName("EVENTCHANNEL"))
                {
                    loadChannelParametersFromXml(xmlNode, InfoObjectCommon::EVENT_CHANNEL);
                }
                else if (xmlNode->hasTagName("SPIKECHANNEL"))
                {
                    loadChannelParametersFromXml(xmlNode, InfoObjectCommon::SPIKE_CHANNEL);
                }
            }
        }
	}

	m_isParamsWereLoaded = true;
}


void NatPlugin::loadChannelParametersFromXml(XmlElement* channelInfo, InfoObjectCommon::InfoObjectType type)
{
	int channelNum = channelInfo->getIntAttribute("number");

	if (type == InfoObjectCommon::DATA_CHANNEL)
	{
		forEachXmlChildElement(*channelInfo, subNode)
		{
			if (subNode->hasTagName("SELECTIONSTATE"))
			{
				getEditor()->setChannelSelectionState(channelNum,
					subNode->getBoolAttribute("param"),
					subNode->getBoolAttribute("record"),
					subNode->getBoolAttribute("audio"));
			}
		}
	}

	loadCustomChannelParametersFromXml(channelInfo, type);
}


void NatPlugin::loadCustomParametersFromXml() { }
void NatPlugin::loadCustomChannelParametersFromXml(XmlElement* channelInfo, InfoObjectCommon::InfoObjectType type) { }

void NatPlugin::reset() {}

void NatPlugin::setCurrentProgramStateInformation(const void* data, int sizeInBytes) {}
void NatPlugin::setStateInformation(const void* data, int sizeInBytes) {}

void NatPlugin::getCurrentProgramStateInformation(MemoryBlock& destData) {}
void NatPlugin::getStateInformation(MemoryBlock& destData) {}

void NatPlugin::changeProgramName(int index, const String& newName) {}
void NatPlugin::setCurrentProgram(int index) {}

void NatPlugin::setCurrentChannel(int chan)
{
	currentChannel = chan;
}


void NatPlugin::setProcessorType(PluginProcessorType processorType)
{
	m_processorType = processorType;
}


//<DEPRECATED>
// ==================================================================
const String NatPlugin::getInputChannelName(int channelIndex) const { return NatPlugin::m_unusedNameString; }
const String NatPlugin::getOutputChannelName(int channelIndex) const { return NatPlugin::m_unusedNameString; }
// ==================================================================

void NatPlugin::getEventChannelNames(StringArray& Names) { }

const String NatPlugin::getProgramName(int index)   { return ""; }
const String NatPlugin::getName() const              { return m_name; }

int NatPlugin::getCurrentChannel() const { return currentChannel; }

PluginProcessorType NatPlugin::getProcessorType() const { return m_processorType; }

bool NatPlugin::hasEditor() const { return false; }

bool NatPlugin::isInputChannelStereoPair(int index) const { return true; }
bool NatPlugin::isOutputChannelStereoPair(int index) const { return true; }

bool NatPlugin::acceptsMidi() const  { return true; }
bool NatPlugin::producesMidi() const { return true; }

bool NatPlugin::silenceInProducesSilenceOut() const  { return false; }

bool NatPlugin::stillHasSource() const { return true; }

bool NatPlugin::isParameterAutomatable(int parameterIndex) const { return false; }
bool NatPlugin::isMetaParameter(int parameterIndex) const { return false; }

bool NatPlugin::canSendSignalTo(NatPlugin*) const { return true; }

bool NatPlugin::isReady()                { return isEnabled; }
bool NatPlugin::isEnabledState() const   { return isEnabled; }

bool NatPlugin::isGeneratesTimestamps() const { return false; }

bool NatPlugin::isFilter()        const  { return getProcessorType() == PROCESSOR_TYPE_FILTER; }
bool NatPlugin::isSource()        const  { return getProcessorType() == PROCESSOR_TYPE_SOURCE; }
bool NatPlugin::isSink()          const  { return getProcessorType() == PROCESSOR_TYPE_SINK; }
bool NatPlugin::isSplitter()      const  { return getProcessorType() == PROCESSOR_TYPE_SPLITTER; }
bool NatPlugin::isMerger()        const  { return getProcessorType() == PROCESSOR_TYPE_MERGER; }
bool NatPlugin::isUtility()       const  { return getProcessorType() == PROCESSOR_TYPE_UTILITY; }

int NatPlugin::getNumParameters()    { return parameters.size(); }
int NatPlugin::getNumPrograms()      { return 0; }
int NatPlugin::getCurrentProgram()   { return 0; }

int NatPlugin::getNumInputs() const                  { return settings.numInputs; }
int NatPlugin::getNumOutputs() const                 { return settings.numOutputs; }
int NatPlugin::getNumOutputs(int subProcessorIdx) const
{
	uint32 sourceId = getProcessorFullId(nodeId, subProcessorIdx);
	try
	{
		return dataChannelMap.at(sourceId).size();
	}
	catch (...)
	{
		return 0;
	}
}

int NatPlugin::getDefaultNumDataOutputs(DataChannel::DataChannelTypes, int) const        { return 0; }

int NatPlugin::getNodeId() const                     { return nodeId; }
int NatPlugin::getTotalNumberOfChannels() const      { return dataChannelArray.size() + eventChannelArray.size() + spikeChannelArray.size(); }

double NatPlugin::getTailLengthSeconds() const       { return 1.0f; }

float NatPlugin::getParameter(int parameterIndex)   { return 1.0; }
float NatPlugin::getDefaultSampleRate() const        { return 44100.0; }
float NatPlugin::getSampleRate(int) const               { return getDefaultSampleRate(); }
float NatPlugin::getDefaultBitVolts() const          { return 1.0; }
float NatPlugin::getBitVolts(int) const				{ return getDefaultBitVolts(); }
float NatPlugin::getBitVolts(const DataChannel* chan) const   { return 1.0; }

NatPlugin* NatPlugin::getSourceNode() const { return sourceNode; }
NatPlugin* NatPlugin::getDestNode()   const { return destNode; }

int NatPlugin::getNumSubProcessors() const { return 1; }

GenericEditor* NatPlugin::getEditor() const { return editor; }

AudioSampleBuffer* NatPlugin::getContinuousBuffer() const { return 0; }
MidiBuffer* NatPlugin::getEventBuffer() const             { return 0; }

void NatPlugin::switchIO(int)   { }
void NatPlugin::switchIO()       { }

void NatPlugin::setPathToProcessor(NatPlugin* p)   { }
void NatPlugin::setMergerSourceNode(NatPlugin* sn)  { }
void NatPlugin::setSplitterDestNode(NatPlugin* dn)  { }

void NatPlugin::startRecording() { }
void NatPlugin::stopRecording()  { }

void NatPlugin::updateSettings() { }

void NatPlugin::enableCurrentChannel(bool) {}

void NatPlugin::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition) {}

void NatPlugin::handleSpike(const SpikeChannel* spikeInfo, const MidiMessage& event, int samplePosition) {}

void NatPlugin::handleTimestampSyncTexts(const MidiMessage& event) {};

void NatPlugin::setEnabledState(bool t)
{
	isEnabled = t;
}

bool NatPlugin::enableProcessor()
{
	m_lastProcessTime = Time::getHighResolutionTicks();
	return enable();
}

bool NatPlugin::disableProcessor()
{
	return disable();
}

bool NatPlugin::enable()
{
	return isEnabled;
}

bool NatPlugin::disable()
{
	return true;
}

NatPlugin::DefaultEventInfo::DefaultEventInfo(EventChannel::EventChannelTypes t, unsigned int c, unsigned int l, float s)
	:type(t),
	nChannels(c),
	length(l),
	sampleRate(s)
{
}

NatPlugin::DefaultEventInfo::DefaultEventInfo()
	:type(EventChannel::INVALID),
	nChannels(0),
	length(0),
	sampleRate(44100)
{}

uint32 NatPlugin::getProcessorFullId(uint16 sid, uint16 subid)
{
	return (uint32(sid) << 16) + subid;
}

uint16 NatPlugin::getNodeIdFromFullId(uint32 fid)
{
	return (fid & 0xFFFF0000 ) >> 16;
}

uint16 NatPlugin::getSubProcessorFromFullId(uint32 fid)
{
	return (fid & 0x0000FFFF);
}

int64 NatPlugin::getLastProcessedsoftwareTime() const
{
	return m_lastProcessTime;
}

void ChannelCreationIndexes::clearChannelCreationCounts()
{
	dataChannelCount = 0;
	dataChannelTypeCount.clear();
	eventChannelCount = 0;
	eventChannelTypeCount.clear();
	spikeChannelCount = 0;
	spikeChannelTypeCount.clear();
}