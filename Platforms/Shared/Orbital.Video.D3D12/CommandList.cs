﻿using System;
using System.Runtime.InteropServices;

namespace Orbital.Video.D3D12
{
	public sealed class CommandList : CommandListBase
	{
		public readonly Device deviceD3D12;
		internal IntPtr handle;

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern IntPtr Orbital_Video_D3D12_CommandList_Create(IntPtr device);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern int Orbital_Video_D3D12_CommandList_Init(IntPtr handle);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_CommandList_Dispose(IntPtr handle);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_CommandList_Start(IntPtr handle, IntPtr device);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_CommandList_Finish(IntPtr handle);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_CommandList_EnableSwapChainRenderTarget(IntPtr handle, IntPtr swapChain);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_CommandList_EnableSwapChainPresent(IntPtr handle, IntPtr swapChain);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_CommandList_ClearSwapChainRenderTarget(IntPtr handle, IntPtr swapChain, float r, float g, float b, float a);

		internal CommandList(Device device)
		: base(device)
		{
			deviceD3D12 = device;
			handle = Orbital_Video_D3D12_CommandList_Create(device.handle);
		}

		internal bool Init()
		{
			return Orbital_Video_D3D12_CommandList_Init(handle) != 0;
		}

		public override void Dispose()
		{
			if (handle != IntPtr.Zero)
			{
				Orbital_Video_D3D12_CommandList_Dispose(handle);
				handle = IntPtr.Zero;
			}
		}

		public override void Start()
		{
			Orbital_Video_D3D12_CommandList_Start(handle, deviceD3D12.handle);
		}

		public override void Finish()
		{
			Orbital_Video_D3D12_CommandList_Finish(handle);
		}

		public override void EnabledRenderTarget()
		{
			Orbital_Video_D3D12_CommandList_EnableSwapChainRenderTarget(handle, deviceD3D12.swapChain.handle);
		}

		public override void EnabledRenderTarget(DepthStencilBase depthStencil)
		{
			throw new System.NotImplementedException();
		}

		public override void EnabledRenderTarget(SwapChainBase swapChain)
		{
			var swapChainD3D12 = (SwapChain)swapChain;
			Orbital_Video_D3D12_CommandList_EnableSwapChainRenderTarget(handle, swapChainD3D12.handle);
		}

		public override void EnabledRenderTarget(SwapChainBase swapChain, DepthStencilBase depthStencil)
		{
			throw new System.NotImplementedException();
		}

		public override void EnabledRenderTarget(RenderTargetBase renderTarget)
		{
			throw new System.NotImplementedException();
		}

		public override void EnabledRenderTarget(RenderTargetBase renderTarget, DepthStencilBase depthStencil)
		{
			throw new System.NotImplementedException();
		}

		public override void EnabledPresent()
		{
			Orbital_Video_D3D12_CommandList_EnableSwapChainPresent(handle, deviceD3D12.swapChain.handle);
		}

		public override void EnabledPresent(SwapChainBase swapChain)
		{
			var swapChainD3D12 = (SwapChain)swapChain;
			Orbital_Video_D3D12_CommandList_EnableSwapChainPresent(handle, swapChainD3D12.handle);
		}

		public override void ClearRenderTarget(float r, float g, float b, float a)
		{
			Orbital_Video_D3D12_CommandList_ClearSwapChainRenderTarget(handle, deviceD3D12.swapChain.handle, r, b, g, a);
		}

		public override void ClearRenderTarget(SwapChainBase swapChain, float r, float g, float b, float a)
		{
			var swapChainD3D12 = (SwapChain)swapChain;
			Orbital_Video_D3D12_CommandList_ClearSwapChainRenderTarget(handle, swapChainD3D12.handle, r, b, g, a);
		}

		public override void ClearRenderTarget(RenderTargetBase renderTarget, float r, float g, float b, float a)
		{
			throw new NotImplementedException();
		}
	}
}