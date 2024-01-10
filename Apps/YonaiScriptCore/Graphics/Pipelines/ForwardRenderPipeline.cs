
namespace Yonai.Graphics.Pipelines
{
	public class ForwardRenderPipeline : IRenderPipeline
	{
		public IVector2 Resolution
		{
			get => Output.Resolution;
			set => Output.Resolution = value;
		}

		public Framebuffer Output { get; private set; }

		public ForwardRenderPipeline()
		{
			Output = new Framebuffer(new FramebufferSpec()
			{
				Attachments = new TextureFormat[] { TextureFormat.RGBA8, TextureFormat.Depth },
				Resolution = Window.Resolution,
				SwapchainTarget = false,
				Samples = 1
			});
		}

		public void Dispose() => Output.Dispose();

		public void Draw(Camera camera)
		{
			Renderer.BeginDebugGroup("Forward render pipeline");

			Output.Bind();

			IVector2 resolution = camera.RenderTarget?.Resolution ?? Output.Resolution;
			if (resolution.x <= 0 || resolution.y <= 0)
				return; // Nothing to draw

			Renderer.ClearColour(Colour.Black);
			Renderer.Clear(BufferFlags.Colour | BufferFlags.Depth);
			Renderer.SetViewport(IVector2.Zero, resolution);

			Renderer.Enable(RenderFeature.DepthTest);
			Renderer.Enable(RenderFeature.CullFace);
			Renderer.CullFace(CullFace.Back);
			Renderer.SetFill(FillMode.Fill);

			World[] worlds = SceneManager.GetActiveScenes();
			foreach(World world in worlds)
			{
				Renderer.BeginDebugGroup($"World [{world.ID}]");

				// Mesh Renderers //
				(MeshRenderer[] renderers, Transform[] meshTransforms) = world.GetComponents<MeshRenderer, Transform>();

				Renderer.BeginDebugGroup("Mesh renderers");
				for(int i = 0; i < renderers.Length; i++)
				{
					MeshRenderer renderer = renderers[i];

					if(renderer.MaterialID == UUID.Invalid ||
						renderer.MeshID == UUID.Invalid)
						continue;

					Mesh mesh = Resource.Get<Mesh>(renderer.MeshID);
					Material material = Resource.Get<Material>(renderer.MaterialID);

					if(!mesh || !material || !material.Shader)
						continue; // Invalid resources

					// TODO: Upload material properties to shader

					material.Shader.Bind();

					material.Shader.Set("time", Time.TimeSinceLaunch);
					material.Shader.Set("resolution", resolution);

					Renderer.UploadTransform(material.Shader, meshTransforms[i]);
					Renderer.UploadCamera(material.Shader, camera, resolution);
					Renderer.DrawMesh(mesh);

					material.Shader.Unbind();
				}
				Renderer.EndDebugGroup();

				Renderer.Disable(RenderFeature.CullFace);

				Renderer.BeginDebugGroup("Sprite renderers");
				// Sprite Renderers //
				(SpriteRenderer[] spriteRenderers, Transform[] spriteTransforms) = world.GetComponents<SpriteRenderer, Transform>();
				for(int i = 0; i < spriteRenderers.Length; i++)
				{
					SpriteRenderer renderer = spriteRenderers[i];
					if (!renderer.Shader || !renderer.Sprite) continue;

					renderer.Shader.Bind();
					renderer.Sprite.Bind();
					renderer.Shader.Set("inputTexture", 0);
					renderer.Shader.Set("colour", renderer.Colour);

					Renderer.UploadTransform(renderer.Shader, spriteTransforms[i]);
					Renderer.UploadCamera(renderer.Shader, camera, resolution);
					Renderer.DrawMesh(Mesh.Quad);

					renderer.Shader.Unbind();
				}
				Renderer.EndDebugGroup();
				Renderer.EndDebugGroup();
			}

			Output.Unbind();

			// Copy output to camera's render target
			if (camera.RenderTarget)
			{
				Renderer.BeginDebugGroup("Copy output to render target");
				Output.CopyAttachmentTo(camera.RenderTarget);
				Renderer.EndDebugGroup();
			}

			Renderer.EndDebugGroup();
		}
	}
}