﻿using System;

namespace Orbital.Video
{
	public enum VertexBufferMode
	{
		/// <summary>
		/// Memory will be optimized for GPU only use
		/// </summary>
		GPUOptimized,

		/// <summary>
		/// Memory will be frequently written to by CPU
		/// </summary>
		Write,

		/// <summary>
		/// Memory will be frequently read from the CPU
		/// </summary>
		Read
	}

	public enum VertexBufferTopology
	{
		Point,
		Line,
		Triangle
	}

	public enum VertexBufferLayoutElementType
	{
		Float,
		Float2,
		Float3,
		Float4,
		RGBAx8
	}

	public enum VertexBufferLayoutElementUsage
	{
		Position,
		Color,
		UV,
		Normal,
		Tangent,
		Binormal,
		Index,
		Weight
	}

	public struct VertexBufferLayoutElement
	{
		public VertexBufferLayoutElementType type;
		public VertexBufferLayoutElementUsage usage;
		public int streamIndex, usageIndex, byteOffset;
	}

	public struct VertexBufferLayout
	{
		public VertexBufferLayoutElement[] elements;
	}

	public abstract class VertexBufferBase : IDisposable
	{
		public int vertexCount { get; protected set; }
		public int vertexSize { get; protected set; }

		public abstract void Dispose();
	}
}
