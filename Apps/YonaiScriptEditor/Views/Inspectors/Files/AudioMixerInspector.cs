using Yonai;
using System;
using Yonai.IO;

namespace YonaiEditor.Inspectors
{
	[CustomInspector(typeof(AudioMixer), ".mixer")]
	public class AudioMixerInspector : CustomInspector
	{
		private AudioMixer m_Target;
		private string m_ParentMixerPath = string.Empty;
		private SoundMixerImportSettings m_Settings;

		public override void OnTargetChanged()
		{
			// Check if filepath is selected (resource view)
			if (Target is VFSFile)
			{
				VFSFile file = (VFSFile)Target;
				m_Target = Resource.Load<AudioMixer>(file.FullPath);
			}
			else // Is SoundMixer object
				m_Target = Target as AudioMixer;

			RefreshSettings();
		}

		public override void DrawInspector()
		{
			if (!m_Target)
				return;

			ImGUI.SetNextItemWidth(ImGUI.ContentRegionAvailable.x);
			ImGUI.Text(m_Target.ResourcePath, Colour.Grey);
			ImGUI.Separator();

			SetupTable();

			m_Settings.Name = Draw("Name", m_Settings.Name);
			m_Settings.Volume = Draw("Volume", m_Settings.Volume, new RangeAttribute(1.0f));

			if (DrawFilepath("Parent Mixer", m_ParentMixerPath, out VFSFile newParentMixerPath, m_Target.ResourceID /* Cannot select self */, ".mixer"))
			{
				m_ParentMixerPath = newParentMixerPath;
				m_Settings.ParentMixer = Resource.Get<AudioMixer>(m_ParentMixerPath)?.ResourceID ?? UUID.Invalid;
			}

			ImGUI.EndTable();

			if (!PendingChanges())
				return;

			Vector2 region = ImGUI.ContentRegionAvailable;
			if (ImGUI.Button("Revert", new Vector2(region.x * 0.5f, 0)))
				RefreshSettings(); // Re-import using previous settings
			ImGUI.SameLine();
			if (ImGUI.Button("Apply", new Vector2(region.x * 0.5f, 0)))
			{
				// Get latest settings. These are set in the object directly in DrawInspector(..) above
				m_Target.Import(m_Settings);

				// Save changes to disk
				Resource.SaveToDisk(m_Target);
			}
		}

		private void RefreshSettings()
		{
			m_Settings = new SoundMixerImportSettings()
			{
				Name = m_Target.Name,
				Volume = m_Target.Volume,
				ParentMixer = m_Target.ParentMixerID
			};

			m_ParentMixerPath = m_Settings.ParentMixer != UUID.Invalid ? Resource.GetPath(m_Settings.ParentMixer) : string.Empty;
		}

		private bool PendingChanges() => m_Settings.Name.CompareTo(m_Target.Name) != 0 ||
											m_Settings.Volume != m_Target.Volume ||
											m_Settings.ParentMixer != m_Target.ParentMixerID;
	}
}