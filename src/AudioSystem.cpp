//
// Created by Lauri Kortevaara on 26/12/15.
//

#include "AudioSystem.h"
#include <dshow.h>
#include <cstdio>
#include <atlcomcli.h>

// For IID_IGraphBuilder, IID_IMediaControl, IID_IMediaEvent
#pragma comment(lib, "strmiids.lib") 

const wchar_t* filePath = L"C:/Users/Kortevaara/Downloads/Forest and Nature Sounds 10 Hours.mp3";

using namespace omen;
using namespace ecs;

AudioSystem::AudioSystem() :
	System(),
	pGraph(nullptr),
	pControl(nullptr),
	pEvent(nullptr),
	basicAudio(nullptr)
{

}

long AudioSystem::setVolume(long pVolume)
{
	CComQIPtr<IBasicAudio> pAudio(pGraph);
	return pAudio ? pAudio->put_Volume(pVolume) : E_NOINTERFACE;
}

long AudioSystem::getVolume()
{
	long volume = 0;
	CComQIPtr<IBasicAudio> pAudio(pGraph);
	HRESULT res = pAudio ? pAudio->get_Volume(&volume) : E_NOINTERFACE;
	return volume;
}

void AudioSystem::playAudio() {
	return;
	m_audioThread = new std::thread([&] {

	// Initialize the COM library.
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
	{
		::printf("ERROR - Could not initialize COM library");
		return;
	}

	// Create the filter graph manager and query for interfaces.
	hr = ::CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder, (void **)&pGraph);
	if (FAILED(hr))
	{
		::printf("ERROR - Could not create the Filter Graph Manager.");
		return;
	}

	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	hr = pGraph->QueryInterface(IID_IBasicAudio, (void **)&basicAudio);

	// Build the graph.
	hr = pGraph->RenderFile(filePath, NULL);
	if (SUCCEEDED(hr))
	{
		// Run the graph.
		hr = pControl->Run();
		if (SUCCEEDED(hr))
		{
			// Wait for completion.
			long volume = getVolume();
			setVolume(10000);

			long evCode;
			pEvent->WaitForCompletion(INFINITE, &evCode);

			// Note: Do not use INFINITE in a real application, because it
			// can block indefinitely.
		}
	}
	// Clean up in reverse order.
	pEvent->Release();
	pControl->Release();
	pGraph->Release();
	::CoUninitialize();
	});
}
