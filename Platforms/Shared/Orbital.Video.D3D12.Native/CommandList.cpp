#include "CommandList.h"
#include "SwapChain.h"

extern "C"
{
	ORBITAL_EXPORT CommandList* Orbital_Video_D3D12_CommandList_Create(Device* device)
	{
		CommandList* handle = (CommandList*)calloc(1, sizeof(CommandList));
		handle->device = device;
		return handle;
	}

	ORBITAL_EXPORT int Orbital_Video_D3D12_CommandList_Init(CommandList* handle)
	{
		if (FAILED(handle->device->physicalDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, handle->device->commandAllocator, nullptr, IID_PPV_ARGS(&handle->commandList)))) return 0;
		if (FAILED(handle->commandList->Close())) return 0;// make sure this is closed as it defaults to open for writing

		return 1;
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_CommandList_Dispose(CommandList* handle)
	{
		if (handle->pipelineState != NULL)
		{
			handle->pipelineState->Release();
			handle->pipelineState = NULL;
		}

		if (handle->commandList != NULL)
		{
			handle->commandList->Release();
			handle->commandList = NULL;
		}

		free(handle);
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_CommandList_Start(CommandList* handle, Device* device)
	{
		handle->commandList->Reset(device->commandAllocator, handle->pipelineState);
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_CommandList_Finish(CommandList* handle)
	{
		handle->commandList->Close();
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_CommandList_EnableSwapChainRenderTarget(CommandList* handle, SwapChain* swapChain)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = swapChain->renderTargetViews[swapChain->currentRenderTargetIndex];
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		handle->commandList->ResourceBarrier(1, &barrier);
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_CommandList_EnableSwapChainPresent(CommandList* handle, SwapChain* swapChain)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = swapChain->renderTargetViews[swapChain->currentRenderTargetIndex];
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		handle->commandList->ResourceBarrier(1, &barrier);
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_CommandList_ClearSwapChainRenderTarget(CommandList* handle, SwapChain* swapChain, float r, float g, float b, float a)
	{
		float rgba[4] = {r, g, b, a};
		handle->commandList->ClearRenderTargetView(swapChain->renderTargetDescHandles[swapChain->currentRenderTargetIndex], rgba, 0, NULL);
	}
}