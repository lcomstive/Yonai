using Yonai;
using TestGame.Components;

namespace TestGame.Systems
{
	public class MoveAlongPathSystem : YonaiSystem
	{
		protected override void Update()
		{
			(MoveAlongPath[] paths, Transform[] transforms) = World.GetComponents<MoveAlongPath, Transform>();
			for(int i = 0; i < paths.Length; i++)
			{
				Vector3 from = paths[i].m_Backwards ? paths[i].PointA : paths[i].PointB;
				Vector3 to   = paths[i].m_Backwards ? paths[i].PointB : paths[i].PointA;

				float movement = paths[i].Speed / Vector3.Distance(from, to);
				paths[i].m_Position += movement * Time.DeltaTime;

				transforms[i].LocalPosition = Vector3.Lerp(from, to, paths[i].m_Position);

				if (paths[i].m_Position >= 1.0f)
				{
					paths[i].m_Position = 0.0f;
					paths[i].m_Backwards = !paths[i].m_Backwards;
				}
			}
		}
	}
}
