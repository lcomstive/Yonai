
using AquaEngine;
using System;

namespace AquaEditor.Views
{
	public class StatsView : View
	{
		/// <summary>
		/// How many FPS values to hold
		/// </summary>
		private const int TotalValues = 100;

		/// <summary>
		/// How often to add an FPS value to <see cref="m_FPSValues"/>, in seconds
		/// </summary>
		private const float UpdateRate = 0.1f;

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

		protected override void Draw()
		{
			ImGUI.Begin("Stats");

			ImGUI.Text($"FPS: {Time.FPS}");
			ImGUI.Text($"Delta Time: {(int)Math.Round(Time.DeltaTime * 1000)}ms");

			IVector2 resolution = Window.Resolution;
			ImGUI.Text($"Resolution: {resolution.x}x{resolution.y}");

			VideoMode videomode = Screen.VideoMode;
			ImGUI.Text($"Monitor: {videomode.Resolution.x}x{videomode.Resolution.y}@{videomode.RefreshRate.ToString("F0")}Hz");

			ImGUI.PlotLines("", m_FPSValues, $"FPS: {Time.FPS}");

			ImGUI.End();
		}
	}
}