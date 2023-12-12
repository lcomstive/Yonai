using Yonai;
using Yonai.Graphics;
using Yonai.Systems;
using YonaiEditor.Systems;

namespace YonaiEditor.Views
{
	public class GameView : View
	{
		[MenuItem("Window/Game")]
		private static void MenuCallback() => EditorUIService.Open<GameView>();

		protected override void Opened()
		{
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
				DrawScene();
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

			// TODO: Move this to graphics backend or render path?
			if (camera.RenderTarget == null || camera.RenderTarget.Resolution != viewportSize)
			{
				camera.RenderTarget?.Dispose();
				camera.RenderTarget = RenderSystem.Backend.Device.CreateTexture(Yonai.Graphics.Backends.Vulkan.VkFormat.R8G8B8A8_SRGB, viewportSize, 1, false);
			}

			ImGUI.Image(Camera.Main.RenderTarget, viewportSize);
		}
	}
}