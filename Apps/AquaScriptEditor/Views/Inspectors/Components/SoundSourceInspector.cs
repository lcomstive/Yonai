using System;
using AquaEngine;

namespace AquaEditor.Inspectors
{
	[CustomInspector(typeof(SoundSource))]
	public class SoundSourceInspector : CustomInspector
	{
		private SoundSource m_Target;

		public override void OnTargetChanged()
		{
			base.OnTargetChanged();
			m_Target = (SoundSource)Target;
		}

		public override void DrawInspector()
		{
			if (!m_Target)
				return;

			SetupTable();

			m_Target.SoundID = DrawResource<Sound>("Sound", m_Target.SoundID);
			m_Target.MixerID = DrawResource<SoundMixer>("Mixer", m_Target.MixerID);
			m_Target.Spatialise = Draw("Spatialize", m_Target.Spatialise);
			m_Target.Volume = Draw("Volume", m_Target.Volume, new RangeAttribute(1.0f));
			m_Target.Panning = Draw("Panning", m_Target.Panning, new RangeAttribute(-1, 1));
			m_Target.Pitch = Draw("Pitch", m_Target.Pitch);
			m_Target.IsLooping = Draw("Loop", m_Target.IsLooping);

			ImGUI.Space();

			float playTime = m_Target.PlayTime;
			float minutes = (float)Math.Floor(playTime / 60.0f);
			float seconds = playTime % 60.0f;
			if(ImGUI.Slider(minutes.ToString("00") + ":" + seconds.ToString("00"), ref playTime, 0, m_Target.Length, ""))
				m_Target.Seek(playTime);

			ImGUI.Space();

			// Draws enum field
			m_Target.State = Draw("State", m_Target.State);

			ImGUI.EndTable();
		}
	}
}