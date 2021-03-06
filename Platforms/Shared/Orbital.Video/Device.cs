﻿using System;
using System.IO;
using Orbital.Host;

namespace Orbital.Video
{
	/// <summary>
	/// How a device will be used
	/// </summary>
	public enum DeviceType
	{
		/// <summary>
		/// Device will be used for presenting rendered buffers on a physical screen
		/// </summary>
		Presentation,

		/// <summary>
		/// Device will only be used for background processing (such as Compute-Shaders, UI-Embedding, etc)
		/// </summary>
		Background
	}

	public abstract class DeviceBase : IDisposable
	{
		public readonly InstanceBase instance;
		public readonly DeviceType type;

		public DeviceBase(InstanceBase instance, DeviceType type)
		{
			this.instance = instance;
			this.type = type;
		}

		public abstract void Dispose();

		/// <summary>
		/// Do any prep work needed before new presentation frame
		/// </summary>
		public abstract void BeginFrame();

		/// <summary>
		/// Finish and present frame to physical screen
		/// </summary>
		public abstract void EndFrame();

		#region Create Methods
		public abstract SwapChainBase CreateSwapChain(WindowBase window, int bufferCount, bool fullscreen, bool ensureSwapChainMatchesWindowSize);
		public abstract CommandListBase CreateCommandList();
		public abstract RenderPassBase CreateRenderPass(RenderPassDesc desc);
		public abstract RenderStateBase CreateRenderState(RenderStateDesc desc, int gpuIndex);
		public abstract ShaderEffectBase CreateShaderEffect(Stream stream, ShaderEffectSamplerAnisotropy anisotropyOverride);
		public abstract ShaderEffectBase CreateShaderEffect(ShaderBase vs, ShaderBase ps, ShaderBase hs, ShaderBase ds, ShaderBase gs, ShaderEffectDesc desc, bool disposeShaders);
		#if CS_7_3
		public abstract VertexBufferBase CreateVertexBuffer<T>(T[] vertices, VertexBufferLayout layout, VertexBufferMode mode) where T : unmanaged;
		public abstract ConstantBufferBase CreateConstantBuffer<T>(T initialData, ConstantBufferMode mode) where T : unmanaged;
		#else
		public abstract VertexBufferBase CreateVertexBuffer<T>(T[] vertices, VertexBufferLayout layout, VertexBufferMode mode) where T : struct;
		public abstract ConstantBufferBase CreateConstantBuffer<T>(T initialData, ConstantBufferMode mode) where T : struct;
		#endif
		public abstract VertexBufferBase CreateVertexBuffer(int size, VertexBufferLayout layout, VertexBufferMode mode);
		public abstract ConstantBufferBase CreateConstantBuffer<T>(ConstantBufferMode mode) where T : struct;
		public abstract ConstantBufferBase CreateConstantBuffer(int size, ConstantBufferMode mode);
		public abstract Texture2DBase CreateTexture2D(TextureFormat format, int width, int height, byte[] data, TextureMode mode);
		#endregion
	}
}
