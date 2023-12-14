using Yonai.Systems;
using Yonai.Graphics.Backends.Vulkan;
using System;

using PoolSizeRatio = Yonai.Graphics.Backends.Vulkan.VulkanDescriptorAllocator.PoolSizeRatio;

namespace Yonai.Graphics.RenderPaths
{
	public class ForwardRenderPath : IRenderPath
	{
		public VulkanImage ColourOutput { get; private set; }

		private VulkanDescriptorAllocator m_DescriptorAllocator;
		private VulkanDescriptorSet m_DrawDescriptors;
		private VulkanDescriptorSetLayout m_DrawDescriptorLayout;

		private VulkanDevice m_Device;
		private VulkanComputePipeline m_Pipeline;

		public struct PushConstants
		{
			public Colour Data1;
			public Colour Data2;
		}
		public PushConstants m_Constants = new PushConstants
		{
			Data1 = Colour.Green,
			Data2 = Colour.Blue
		};

		public ForwardRenderPath()
		{
			m_Device = RenderSystem.Backend.Device as VulkanDevice;
			m_DescriptorAllocator = new VulkanDescriptorAllocator(m_Device);

			InitDescriptors();
			CreatePipeline();

			OnResized(Window.Resolution);
		}

		public void Draw(VulkanCommandBuffer cmd)
		{
			World[] worlds = SceneManager.GetActiveScenes();
			foreach(World world in worlds)
			{
				(Transform[] transforms, Camera[] cameras) = world.GetComponents<Transform, Camera>();
				for (int i = 0; i < transforms.Length; i++)
					Draw(cmd, transforms[i], cameras[i]);
			}
		}

		private void Draw(VulkanCommandBuffer cmd, Transform camTransform, Camera camera)
		{
			VulkanImage target = null;
			if (camera.RenderTarget != null)
				target = (VulkanImage)camera.RenderTarget;
			else if (camera.IsMain)
				target = ColourOutput;

			if (target == null) return; // No valid target found

			cmd.TransitionImageLayout(ColourOutput, VkImageLayout.Undefined, VkImageLayout.GENERAL);

			cmd.BindPipeline(m_Pipeline, VkPipelineBindPoint.Compute);
			cmd.BindDescriptorSets(m_Pipeline, VkPipelineBindPoint.Compute, m_DrawDescriptors);

			cmd.PushConstants(m_Pipeline, VkShaderStage.Compute, 0, sizeof(float) * 8, m_Constants);

			cmd.Dispatch(
				(uint)Math.Ceiling(ColourOutput.Resolution.x / 16.0f),
				(uint)Math.Ceiling(ColourOutput.Resolution.y / 16.0f),
				1);

			DrawGeometry(cmd, camera.World);

			cmd.TransitionImageLayout(ColourOutput, VkImageLayout.Undefined, VkImageLayout.TRANSFER_SRC_OPTIMAL);

			if(camera.RenderTarget != null)
			{
				cmd.TransitionImageLayout(target, VkImageLayout.Undefined, VkImageLayout.TRANSFER_DST_OPTIMAL);

				cmd.CopyImageTo(ColourOutput, VkImageLayout.TRANSFER_SRC_OPTIMAL, target, VkImageLayout.TRANSFER_DST_OPTIMAL);

				cmd.TransitionImageLayout(target, VkImageLayout.Undefined, VkImageLayout.SHADER_READ_ONLY_OPTIMAL);
			}
		}

		private void DrawGeometry(VulkanCommandBuffer cmd, World world)
		{
			IVector2 resolution = ColourOutput.Resolution;
            
			VkRenderingAttachmentInfo colorAttachment = new VkRenderingAttachmentInfo
			{
				Image = ColourOutput,
				ImageLayout = VkImageLayout.GENERAL
			};
			VkRenderingInfo renderInfo = new VkRenderingInfo
			{
				ColorAttachments = new VkRenderingAttachmentInfo[] { colorAttachment },
				RenderArea = new VkRect2D(IVector2.Zero, resolution),
				LayerCount = 1
			};
			cmd.BeginRendering(renderInfo);

			cmd.SetViewport(new VkViewport(resolution));
			cmd.SetScissor(new VkRect2D(resolution));

			(Transform[] transforms, MeshRenderer[] meshRenderers) = world.GetComponents<Transform, MeshRenderer>();

			for(int i = 0; i < transforms.Length; i++)
			{
				Mesh mesh = meshRenderers[i].Mesh;
				Material material = meshRenderers[i].Material;

				if (!material || !mesh || material.Pipeline == null) continue;

				cmd.BindPipeline(material.Pipeline, VkPipelineBindPoint.Graphics);

				if (mesh.VertexBuffer == null) Log.Warning("Vertex buffer is null?");

				cmd.BindVertexBuffer((VulkanBuffer)mesh.VertexBuffer);
				cmd.BindIndexBuffer((VulkanBuffer)mesh.IndexBuffer);

				cmd.DrawIndexed((uint)mesh.Indices.Length);
			}

			cmd.EndRendering();
		}

		public void OnResized(IVector2 resolution)
		{
			GenerateColourOutput(resolution);
			UpdateDescriptors();
		}

		public void Dispose()
		{
			ColourOutput?.Dispose();
			m_Pipeline?.Dispose();
		}

		private void GenerateColourOutput(IVector2 resolution)
		{
			ColourOutput?.Dispose();

			VkImageCreateInfo createInfo = VkImageCreateInfo.Default;
			createInfo.Format = VkFormat.R16G16B16A16_SFLOAT;
			createInfo.Extent = new Extents3D(resolution);
			createInfo.Usage = VkImageUsage.TransferSrc |
								VkImageUsage.TransferDst |
								VkImageUsage.Storage |
								VkImageUsage.ColorAttachment;

			VkImageViewCreateInfo imageViewInfo = VkImageViewCreateInfo.Default;
			imageViewInfo.Format = createInfo.Format;

			ColourOutput = new VulkanImage(m_Device, createInfo, imageViewInfo, VkSamplerCreateInfo.Default);
			ColourOutput.TransitionImageLayout(VkImageLayout.Undefined, VkImageLayout.TRANSFER_SRC_OPTIMAL);
		}

		private void InitDescriptors()
		{
			PoolSizeRatio[] ratios = new PoolSizeRatio[]
			{
				new PoolSizeRatio
				{
					Type = VkDescriptorType.STORAGE_IMAGE,
					Ratio = 1.0f
				}
			};

			m_DescriptorAllocator.CreatePool(10, ratios);

			m_DrawDescriptorLayout = new VulkanDescriptorSetLayoutBuilder()
				.Add(0, VkDescriptorType.STORAGE_IMAGE, VkShaderStage.Compute)
				.Build(m_Device);

			m_DrawDescriptors = m_DescriptorAllocator.Allocate(m_DrawDescriptorLayout);
		}

		private void UpdateDescriptors()
		{
			VkWriteDescriptorSet drawImgWrite = new VkWriteDescriptorSet
			{
				DestinationBinding = 0,
				DescriptorCount = 1,
				DescriptorType = VkDescriptorType.STORAGE_IMAGE,
				ImageInfos = new VkDescriptorImageInfo[]
				{
					new VkDescriptorImageInfo
					{
						ImageLayout = VkImageLayout.GENERAL,
						ImageView = ColourOutput.ImageView
					}
				}
			};

			m_DrawDescriptors.Update(new VkWriteDescriptorSet[] { drawImgWrite });
		}

		private void CreatePipeline()
		{
			m_Pipeline?.Dispose();

			VulkanShaderModule shaderModule = new VulkanShaderModule(m_Device, "assets://Shaders/Vulkan/compute.spv");

			VkComputePipelineCreateInfo info = new VkComputePipelineCreateInfo
			{
				DescriptorSetLayouts = new VulkanDescriptorSetLayout[] { m_DrawDescriptorLayout },
				Shader = shaderModule,
				PushConstantRanges = new VkPushConstantRange[]
				{
					new VkPushConstantRange
					{
						Offset = 0,
						Stage = VkShaderStage.Compute,
						Size = sizeof(float) * 8 // Vector4 * 2
					}
				}
			};
			m_Pipeline = new VulkanComputePipeline(m_Device, info);
		}
	}
}