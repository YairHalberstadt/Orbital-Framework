﻿using System;
using System.Runtime.InteropServices;

namespace Orbital.Video.D3D12
{
	public sealed class RenderPass : RenderPassBase
	{
		internal IntPtr handle;

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern IntPtr Orbital_Video_D3D12_RenderPass_Create_WithSwapChain(IntPtr device, IntPtr swapChain);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static unsafe extern int Orbital_Video_D3D12_RenderPass_Init(IntPtr handle, RenderPassDesc_NativeInterop* desc);

		[DllImport(Instance.lib, CallingConvention = Instance.callingConvention)]
		private static extern void Orbital_Video_D3D12_RenderPass_Dispose(IntPtr handle);

		public RenderPass(SwapChain swapChain)
		{
			handle = Orbital_Video_D3D12_RenderPass_Create_WithSwapChain(swapChain.deviceD3D12.handle, swapChain.handle);
		}

		public unsafe bool Init(RenderPassDesc desc)
		{
			var descNative = new RenderPassDesc_NativeInterop(ref desc);
			return Orbital_Video_D3D12_RenderPass_Init(handle, &descNative) != 0;
		}

		public override void Dispose()
		{
			if (handle != IntPtr.Zero)
			{
				Orbital_Video_D3D12_RenderPass_Dispose(handle);
				handle = IntPtr.Zero;
			}
		}
	}
}
