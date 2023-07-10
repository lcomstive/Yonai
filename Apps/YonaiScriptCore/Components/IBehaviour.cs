namespace Yonai
{
	public interface IBehaviour
	{
		bool Enabled { get; set; }

		void Start();
		void Destroyed();

		void Update();
	}
}