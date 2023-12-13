using Yonai;
using Yonai.Systems;
using YonaiEditor.Systems;
using Yonai.Graphics.Backends.Vulkan;
using Yonai.Graphics;
using Yonai.Graphics.RenderPaths;

namespace YonaiEditor.Views
{
	public class GameView : View
	{
		private IGraphicsDevice m_Device;

		[MenuItem("Window/Game")]
		private static void MenuCallback() => EditorUIService.Open<GameView>();

		protected override void Opened()
		{
			m_Device = RenderSystem.Backend.Device;
			EditorService.StateChanged += OnEditorStateChanged;
		}

		private void OnEditorStateChanged(EditorState oldState, EditorState newState)
		{
			// Set focus to game window
			if (oldState != EditorState.Play && newState == EditorState.Play)
				ImGUI.SetWindowFocus("Game");
		}

		protected override void Draw()
		{
			bool isOpen = true;
			ImGUI.PushStyleVar(ImGUI.StyleVar.WindowPadding, Vector2.Zero);
			ImGUI.PushStyleColour(ImGUI.StyleColour.WindowBg, Colour.Black);
			if (ImGUI.Begin("Game", ref isOpen))
			{
				Vector2 cursorPos = ImGUI.GetCursorPos();

				DrawScene();

				ImGUI.SetCursorPos(cursorPos);
				ForwardRenderPath renderPath = (ForwardRenderPath)ImGUISystem.m_PreviousRenderPath;
				ImGUI.ColourEdit3("Colour 1", ref renderPath.m_Constants.Data1);
				ImGUI.ColourEdit3("Colour 2", ref renderPath.m_Constants.Data2);
			}
			ImGUI.End();
			ImGUI.PopStyleVar();
			ImGUI.PopStyleColour();

			// Check if window requested to be closed
			if (!isOpen)
				EditorUIService.Close<GameView>();
		}

		private void DrawScene()
		{
			Camera camera = Camera.Main;
			if(!camera)
			{
				foreach(World world in SceneManager.GetActiveScenes())
				{
					Camera[] cameras = world.GetComponents<Camera>();
					if (cameras.Length == 0)
						continue;

					camera = Camera.Main = cameras[0];
					break;
				}
			}
			if(!camera)
			{
				ImGUI.Text("No camera found");
				return;
			}

			// Disable input system to game content unless this view is focused
			Input.Enabled = ImGUI.WindowFocused;

			IVector2 viewportSize = (IVector2)ImGUI.ContentRegionAvailable;

			/*
			if (!camera.RenderTarget)
				camera.RenderTarget = new RenderTexture(viewportSize);
			else
				camera.RenderTarget.Resolution = viewportSize;

			// Draw to camera render target
			IRenderPipeline pipeline = Renderer.Pipeline;
			pipeline.Resolution = viewportSize;

			pipeline.Draw(camera);

			ImGUI.Image(camera.RenderTarget, viewportSize);
			*/

			if (camera.RenderTarget == null || camera.RenderTarget.Resolution != viewportSize)
			{
				((VulkanDevice)m_Device).WaitIdle();
				camera.RenderTarget?.Dispose();
				camera.RenderTarget = m_Device.CreateTexture(VkFormat.R8G8B8A8_SRGB, viewportSize, 1, false);
				((VulkanImage)camera.RenderTarget).TransitionImageLayout(VkImageLayout.Undefined, VkImageLayout.SHADER_READ_ONLY_OPTIMAL);
			}

			ImGUI.Image(camera.RenderTarget, viewportSize);
		}
	}
}