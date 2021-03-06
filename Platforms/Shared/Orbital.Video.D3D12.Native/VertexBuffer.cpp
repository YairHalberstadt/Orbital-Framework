#include "VertexBuffer.h"

extern "C"
{
	ORBITAL_EXPORT VertexBuffer* Orbital_Video_D3D12_VertexBuffer_Create(Device* device, VertexBufferMode mode)
	{
		VertexBuffer* handle = (VertexBuffer*)calloc(1, sizeof(VertexBuffer));
		handle->device = device;
		handle->mode = mode;
		return handle;
	}

	ORBITAL_EXPORT int Orbital_Video_D3D12_VertexBuffer_Init(VertexBuffer* handle, void* vertices, uint64_t vertexCount, uint32_t vertexSize, VertexBufferLayout* layout)
	{
		uint64_t bufferSize = vertexSize * vertexCount;

		// create buffer
		D3D12_HEAP_PROPERTIES heapProperties = {};
		if (handle->mode == VertexBufferMode_GPUOptimized) heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		else return 0;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProperties.CreationNodeMask = 1;// TODO: multi-gpu setup
        heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = bufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1, 
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		handle->resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if (vertices != NULL && handle->mode == VertexBufferMode_GPUOptimized) handle->resourceState = D3D12_RESOURCE_STATE_COPY_DEST;// init for gpu copy
		if (FAILED(handle->device->device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, handle->resourceState, nullptr, IID_PPV_ARGS(&handle->vertexBuffer)))) return 0;

		// upload cpu buffer to gpu
		if (vertices != NULL)
		{
			// allocate gpu upload buffer if needed
			bool useUploadBuffer = false;
			ID3D12Resource* uploadResource = handle->vertexBuffer;
			if (heapProperties.Type != D3D12_HEAP_TYPE_UPLOAD)
			{
				useUploadBuffer = true;
				uploadResource = NULL;
				heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
				if (FAILED(handle->device->device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadResource)))) return 0;
			}

			// copy CPU memory to GPU
			UINT8* gpuDataPtr;
			D3D12_RANGE readRange = {};
			if (FAILED(uploadResource->Map(0, &readRange, reinterpret_cast<void**>(&gpuDataPtr))))
			{
				if (useUploadBuffer) uploadResource->Release();
				return 0;
			}
			memcpy(gpuDataPtr, vertices, bufferSize);
			uploadResource->Unmap(0, nullptr);

			// copy upload buffer to default buffer
			if (useUploadBuffer)
			{
				handle->device->internalMutex->lock();
				// reset command list and copy resource
				handle->device->internalCommandList->Reset(handle->device->commandAllocator, NULL);
				handle->device->internalCommandList->CopyResource(handle->vertexBuffer, uploadResource);

				// close command list
				handle->device->internalCommandList->Close();

				// execute operations
				ID3D12CommandList* commandLists[1] = { handle->device->internalCommandList };
				handle->device->commandQueue->ExecuteCommandLists(1, commandLists);
				WaitForFence(handle->device, handle->device->internalFence, handle->device->internalFenceEvent, handle->device->internalFenceValue);

				// release temp resource
				uploadResource->Release();
				handle->device->internalMutex->unlock();
			}
		}

		// create view
		handle->vertexBufferView.BufferLocation = handle->vertexBuffer->GetGPUVirtualAddress();
        handle->vertexBufferView.StrideInBytes = vertexSize;
        handle->vertexBufferView.SizeInBytes = bufferSize;

		// vertex buffer layout
		handle->elementCount = layout->elementCount;
		handle->elements = (D3D12_INPUT_ELEMENT_DESC*)calloc(layout->elementCount, sizeof(D3D12_INPUT_ELEMENT_DESC));
		for (int i = 0; i != layout->elementCount; ++i)
		{
			VertexBufferLayoutElement element = layout->elements[i];
			D3D12_INPUT_ELEMENT_DESC elementDesc = {};
			elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			elementDesc.InputSlot = element.streamIndex;
			elementDesc.AlignedByteOffset = element.byteOffset;
			elementDesc.SemanticIndex = element.usageIndex;

			switch (element.type)
			{
				case VertexBufferLayoutElementType::VertexBufferLayoutElementType_Float: elementDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
				case VertexBufferLayoutElementType::VertexBufferLayoutElementType_Float2: elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
				case VertexBufferLayoutElementType::VertexBufferLayoutElementType_Float3: elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				case VertexBufferLayoutElementType::VertexBufferLayoutElementType_Float4: elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				case VertexBufferLayoutElementType::VertexBufferLayoutElementType_RGBAx8: elementDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
				default: return 0;
			}

			switch (element.usage)
			{
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Position: elementDesc.SemanticName = "POSITION"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Color: elementDesc.SemanticName = "COLOR"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_UV: elementDesc.SemanticName = "TEXCOORD"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Normal: elementDesc.SemanticName = "NORMAL"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Tangent: elementDesc.SemanticName = "TANGENT"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Binormal: elementDesc.SemanticName = "BINORMAL"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Index: elementDesc.SemanticName = "BLENDINDICES"; break;
				case VertexBufferLayoutElementUsage::VertexBufferLayoutElementUsage_Weight: elementDesc.SemanticName = "BLENDWEIGHT"; break;
				default: return 0;
			}

			memcpy(&handle->elements[i], &elementDesc, sizeof(D3D12_INPUT_ELEMENT_DESC));
		}

		return 1;
	}

	ORBITAL_EXPORT void Orbital_Video_D3D12_VertexBuffer_Dispose(VertexBuffer* handle)
	{
		if (handle->elements != NULL)
		{
			free(handle->elements);
			handle->elements = NULL;
		}

		if (handle->vertexBuffer != NULL)
		{
			handle->vertexBuffer->Release();
			handle->vertexBuffer = NULL;
		}

		free(handle);
	}
}

void Orbital_Video_D3D12_VertexBuffer_ChangeState(VertexBuffer* handle, D3D12_RESOURCE_STATES state, ID3D12GraphicsCommandList5* commandList)
{
	if (handle->resourceState == state) return;
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = handle->vertexBuffer;
	barrier.Transition.StateBefore = handle->resourceState;
	barrier.Transition.StateAfter = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);
	handle->resourceState = state;
}