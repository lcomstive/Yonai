using System;
using System.Runtime.CompilerServices;

namespace ScriptingTest
{
	public class HelloWorld
	{
		public float TestFloat = 5.0f;

		public void PrintTestFloat() => Console.WriteLine($"Test float: {TestFloat}");

		public void PrintTestMsg() => Console.WriteLine(Sample.GetMessage());
	}
}

class Sample
{
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern string GetMessage();
}