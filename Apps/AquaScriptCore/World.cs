using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AquaEngine
{
	public class World
	{
		public uint ID { get; internal set; }

		public World(uint id) => ID = id;
	}
}
