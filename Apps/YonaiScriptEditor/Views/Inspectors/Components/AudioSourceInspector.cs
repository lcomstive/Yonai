using System;
using Yonai;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(AudioSource))]
	public class AudioSourceInspector : CustomInspector
	{
		private AudioSource m_Target;

		public override void OnTargetChanged()
		{
			base.OnTargetChanged();
			m_Target = (AudioSource)Target;
		}

		public override void DrawInspector()
		{
			if (!m_Target)
				return;

			SetupTable();

			m_Target.SoundID = DrawResource<AudioData>("Sound", m_Target.SoundID);
			m_Target.MixerID = DrawResource<AudioMixer>("Mixer", m_Target.MixerID);
			m_Target.Is3D = Draw("3D Audio", m_Target.Is3D);
			m_Target.Volume = Draw("Volume", m_Target.Volume, new RangeAttribute(1.0f));
			m_Target.Panning = Draw("Panning", m_Target.Panning, new RangeAttribute(-1, 1));
			m_Target.IsLooping = Draw("Loop", m_Target.IsLooping);
			m_Target.PlayOnStart = Draw("Play on Start", m_Target.PlayOnStart);

			ImGUI.Space();

			if(ImGUI.Foldout("Attenuation"))
			{
				AudioSource.AttenuationSettings attenuation = m_Target.Attenuation;
				attenuation.Distance = Draw("Distance", attenuation.Distance);
				attenuation.Gain = Draw("Gain", attenuation.Gain);
				attenuation.Factor = Draw("Rolloff Factor", attenuation.Factor);
				attenuation.Model = Draw("Gain", attenuation.Model);
				
				m_Target.Attenuation = attenuation;
			}

			ImGUI.Space();

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