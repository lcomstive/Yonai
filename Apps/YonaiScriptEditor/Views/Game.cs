using Yonai;
using Yonai.Graphics;
using YonaiEditor.Systems;

namespace YonaiEditor.Views
{
	public class GameView : View
	{
		[MenuItem("Window/Game")]
		private static void MenuCallback() => EditorUIService.Open<GameView>();

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

			if (!camera.RenderTarget)
				camera.RenderTarget = new RenderTexture(viewportSize);
			else
				camera.RenderTarget.Resolution = viewportSize;

			// Draw to camera render target
			IRenderPipeline pipeline = Renderer.Pipeline;
			pipeline.Resolution = viewportSize;

			pipeline.Draw(camera);

			ImGUI.Image(camera.RenderTarget, viewportSize);
		}
	}
}