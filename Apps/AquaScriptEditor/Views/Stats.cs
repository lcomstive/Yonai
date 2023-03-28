
using AquaEngine;
using System;

namespace AquaEditor.Views
{
	public class StatsView : View
	{
		/// <summary>
		/// How many FPS values to hold
		/// </summary>
		private const int TotalValues = 50;

		/// <summary>
		/// How often to add an FPS value to <see cref="m_FPSValues"/>, in seconds
		/// </summary>
		private const float UpdateRate = 0.03f;

		/// <summary>
		/// Tracks <see cref="TotalValues"/> most recent FPS values.
		/// Higher index is more recent FPS value.
		/// </summary>
		private float[] m_FPSValues = new float[TotalValues];

		/// <summary>
		/// Increments each frame, when reaches threshold of <see cref="UpdateRate"/>,
		/// shifts all values in <see cref="m_FPSValues"/> down one and sets last value as current FPS
		/// </summary>
		private float m_UpdateTimer = 0.0f;

		protected override void Update()
		{
			m_UpdateTimer += Time.DeltaTime;
			if (m_UpdateTimer < UpdateRate)
				return;

			m_UpdateTimer -= UpdateRate;

			for (int i = 0; i < TotalValues - 1; i++)
				m_FPSValues[i] = m_FPSValues[i + 1];
			m_FPSValues[m_FPSValues.Length - 1] = Time.FPS;
		}

		private bool m_IsOpen = true;
		protected override void Draw()
		{
			if(!m_IsOpen) return;

			if (ImGUI.Begin("Stats", ref m_IsOpen))
			{
				ImGUI.Text($"FPS: {Time.FPS}");
				ImGUI.Text($"Delta Time: {(Time.DeltaTime * 1000).ToString("F1")}ms");

				IVector2 resolution = Window.Resolution;
				ImGUI.Text($"Resolution: {resolution.x}x{resolution.y}");

				VideoMode videomode = Screen.VideoMode;
				ImGUI.Text($"Monitor: {videomode.Resolution.x}x{videomode.Resolution.y}@{videomode.RefreshRate.ToString("F0")}Hz");

				ImGUI.PlotLines("", m_FPSValues, $"FPS: {Time.FPS}");

				ImGUI.Space();

				bool vsync = Window.VSync;
				if (ImGUI.Checkbox("VSync", ref vsync))
					Window.VSync = vsync;
			}
			ImGUI.End();

			// Check if window closed
			if (!m_IsOpen)
			{
				AquaSystem.Get<EditorUIService>().Close<StatsView>();
			}
		}
	}
}