using System;
using System.Collections.Generic;

namespace Yonai
{
	public class FunctionQueue : Queue<Action>
	{
		public void Flush()
		{
			Action action = Dequeue();
			while(action != null)
			{
				action.Invoke();
				action = Dequeue();
			}
		}
	}
}