using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace Yonai.Graphics
{
	public struct FramebufferSpec
	{
		/// <summary>
		/// Drawable resolution of framebuffer
		/// </summary>
		public IVector2 Resolution;

		/// <summary>
		/// When true, outputs directly to screen
		/// </summary>
		public bool SwapchainTarget;

		/// <summary>
		/// Multisampling. Disabled when set to 1
		/// </summary>
		public uint Samples;

		/// <summary>
		/// Output attachments to create for the framebuffer
		/// </summary>
		public TextureFormat[] Attachments;

		public FramebufferSpec(IVector2 resolution, bool swapchainTarget = false)
		{
			Resolution = resolution;
			SwapchainTarget = swapchainTarget;
			Samples = 1;
			Attachments = null;
		}
	}

	public class Framebuffer : IDisposable
	{
		public uint Samples
		{
			get => Specs.Samples;
			set => _SetSamples(Handle, Specs.Samples = value);
		}

		public IVector2 Resolution
		{
			get => Specs.Resolution;
			set
			{
				Specs.Resolution = value;
				_SetResolution(Handle, ref value);
			}
		}

		public RenderTexture[] Attachments { get; private set; }
		public RenderTexture DepthAttachment { get; private set; }
		public RenderTexture[] ColourAttachments { get; private set; }

		public uint AttachmentCount => _GetAttachmentCount(Handle);
		public uint ColourAttachmentCount => _GetColourAttachmentCount(Handle);

		public bool HasDepthAttachment => m_DepthAttachmentIndex >= 0;

		internal IntPtr Handle { get; private set; }

		private FramebufferSpec Specs;
		private int m_DepthAttachmentIndex = -1; // < 0 represents no depth attachment

		public Framebuffer(FramebufferSpec specs)
		{
			Specs = specs;

			// Convert attachments from TextureFormat[] to byte[]
			byte[] attachments = new byte[specs.Attachments.Length];
			for (int i = 0; i < attachments.Length; i++)
				attachments[i] = (byte)specs.Attachments[i];

			// Create framebuffer and attachments
			Handle = _Load(
				ref specs.Resolution,
				specs.SwapchainTarget,
				specs.Samples,
				attachments
			);

			LoadAttachments();
		}

		internal Framebuffer(IntPtr handle)
		{
			Handle = handle;
			LoadAttachments();
		}

		private void LoadAttachments()
		{
			// Get handles to all attachments
			IntPtr[] allAttachments = _GetAttachments(Handle, out m_DepthAttachmentIndex);

			// Prepare arrays
			Attachments = new RenderTexture[allAttachments.Length];
			ColourAttachments = new RenderTexture[allAttachments.Length - (HasDepthAttachment ? 1 : 0)];

			// Create managed render textures from unmanaged handles
			int colourAttachmentIndex = 0;
			for (int i = 0; i < allAttachments.Length; i++)
			{
				Attachments[i] = new RenderTexture(allAttachments[i]);

				// Assign either colour or depth attachment
				if (i == m_DepthAttachmentIndex)
					DepthAttachment = Attachments[i];
				else
					ColourAttachments[colourAttachmentIndex++] = Attachments[i];
			}

			// Assign depth attachment, if present
			DepthAttachment = HasDepthAttachment ? Attachments[m_DepthAttachmentIndex] : null;
		}

		/// <summary>
		/// Creates a new framebuffer using the same specs as <paramref name="original"/>
		/// </summary>
		public Framebuffer(Framebuffer original) : this(original.Specs) { }

		public void Dispose() => _Destroy(Handle);

		public void Bind() => _Bind(Handle);
		public void Unbind() => _Unbind(Handle);

		public void CopyAttachmentTo(RenderTexture destination, uint attachmentIndex = 0)
		{
			if (destination != null)
				_CopyAttachmentTo(Handle, destination.Handle, attachmentIndex);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="destination">Set to null for screen output</param>
		/// <param name="flags"></param>
		public void BlitTo(Framebuffer destination, BufferFlags flags = BufferFlags.Colour | BufferFlags.Depth)
			=> _BlitTo(Handle, destination?.Handle ?? IntPtr.Zero, (uint)flags);

		public RenderTexture GetColourAttachment(uint attachmentIndex = 0)
		{
			attachmentIndex = MathUtils.Clamp(attachmentIndex, 0, ColourAttachmentCount);
			return ColourAttachments[attachmentIndex];
		}

		#region Internal Calls
		[MethodImpl(MethodImplOptions.InternalCall)]
		private extern static IntPtr _Load(ref IVector2 resolution, bool swapchainTarget, uint samples, byte[] attachmentFormats);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _Destroy(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _BlitTo(IntPtr handle, IntPtr destination, uint flags);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _CopyAttachmentTo(IntPtr handle, IntPtr destination, uint attachmentIndex);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _Bind(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _Unbind(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _SetSamples(IntPtr handle, uint samples);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static void _SetResolution(IntPtr handle, ref IVector2 resolution);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static uint _GetAttachmentCount(IntPtr handle);
		[MethodImpl(MethodImplOptions.InternalCall)] private extern static uint _GetColourAttachmentCount(IntPtr handle);

		[MethodImpl(MethodImplOptions.InternalCall)] private extern static IntPtr[] _GetAttachments(IntPtr handle, out int depthAttachmentIndex);
		#endregion
	}
}
