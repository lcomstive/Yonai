using Newtonsoft.Json.Linq;

namespace Yonai
{
	public abstract class Behaviour : Component, IBehaviour
	{
		private bool m_Enabled = true;

		public bool Enabled
		{
			get => m_Enabled;
			set
			{
				if(m_Enabled != value)
					SetEnabled(value);
			}
		}

		public virtual void Start() { }
		public virtual void Destroyed() { }

		public virtual void Update() { }

		public virtual void OnEnable() { }
		public virtual void OnDisable() { }

		public override void OnDeserialize(JObject json)
		{
			base.OnDeserialize(json);
			SetEnabled(m_Enabled);
		}

		private void SetEnabled(bool enabled)
		{
			m_Enabled = enabled;
			if (m_Enabled)
				OnEnable();
			else
				OnDisable();

			
		}
	}
}