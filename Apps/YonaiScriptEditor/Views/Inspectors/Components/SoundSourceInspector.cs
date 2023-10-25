using System;
using Yonai;

namespace YonaiEditor.Inspectors
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
			m_Target.Spatialise = Draw("3D Audio", m_Target.Spatialise);
			m_Target.Volume = Draw("Volume", m_Target.Volume, new RangeAttribute(1.0f));
			m_Target.Panning = Draw("Panning", m_Target.Panning, new RangeAttribute(-1, 1));
			m_Target.IsLooping = Draw("Loop", m_Target.IsLooping);
			m_Target.PlayOnStart = Draw("Play on Start", m_Target.PlayOnStart);

			if (EditorService.State != EditorState.Edit)
			{
				ImGUI.Space();

				float playTime = (float)m_Target.PlayTime;
				if (ImGUI.Slider($"{TimeToString(playTime)}/{TimeToString(m_Target.Length)}", ref playTime, 0, (float)m_Target.Length, ""))
					m_Target.Seek(playTime);

				ImGUI.Space();

				// Draws enum field
				m_Target.State = Draw("State", m_Target.State);
			}

			ImGUI.EndTable();
		}

		private static string TimeToString(double seconds) =>
			Math.Floor(seconds / 60.0).ToString("00") + ":" +
				(seconds % 60.0).ToString("00");
	}
}