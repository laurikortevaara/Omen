//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#include "AudioSystem.h"
#include <dshow.h>
#include <cstdio>
// For IID_IGraphBuilder, IID_IMediaControl, IID_IMediaEvent
#pragma comment(lib, "strmiids.lib") 

const wchar_t* filePath = L"C:/Users/Kortevaara/Downloads/Black Sails OST - Theme from Black Sails.mp3";

using namespace omen;
using namespace ecs;

void AudioSystem::add(Component* component)
{
	m_components.push_back(component);
}

void AudioSystem::playAudio() {
	m_audioThread = new std::thread([] {
	IGraphBuilder *pGraph = NULL;
	IMediaControl *pControl = NULL;
	IMediaEvent   *pEvent = NULL;

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

	// Build the graph.
	hr = pGraph->RenderFile(filePath, NULL);
	if (SUCCEEDED(hr))
	{
		// Run the graph.
		hr = pControl->Run();
		if (SUCCEEDED(hr))
		{
			// Wait for completion.
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
