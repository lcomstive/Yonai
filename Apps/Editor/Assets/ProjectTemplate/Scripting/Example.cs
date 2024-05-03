using Yonai;
using System;

public class TestComponent : Component
{
	public Colour Value = Colour.White;
	public float ValueChangeSpeed = 1.0f;
}

public class TestSystem : YonaiSystem
{
	protected override void Enabled()
	{
		Log.Debug("Test system enabled");
	}

	protected override void Update()
	{
		
	}
}