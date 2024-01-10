using Yonai;
using System;
using Steamworks;

namespace TestGame.Systems
{
	public class SteamSystem : YonaiSystem
	{
		protected override void Start()
		{			
			if (!Packsize.Test())
				Log.Error("Packsize test is false, the wrong version of Steamworks.NET is being run on this platform");
			if (!DllCheck.Test())
				Log.Error("DllCheck returned false, the steamworks binary is missing or incorrect");

			try
			{
				// If Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
				// Steam client and also launches this game again if the User owns it. This can act as a rudimentary form of DRM.

				// Once you get a Steam AppID assigned by Valve, you need to replace AppId_t.Invalid with it and
				// remove steam_appid.txt from the game depot. eg: "(AppId_t)480" or "new AppId_t(480)".
				// See the Valve documentation for more information: https://partner.steamgames.com/doc/sdk/api#initialization_and_shutdown
				if (SteamAPI.RestartAppIfNecessary(AppId_t.Invalid))
				{
					Log.Debug("Restarting via Steam");
					Application.Exit();
					return;
				}
			}
			catch (DllNotFoundException e)
			{
				Log.Exception(e, "Could not load steam_api.dll/so/dylib");
				return;
			}
		}

		protected override void Enabled()
		{
			// Initialise Steamworks API
			if(!SteamAPI.Init())
			{
				Log.Error("Failed to initialise Steam API");
				Enable(false);
				return;
			}

			Log.Debug("Steam API initialised");
		}

		protected override void Disabled() => SteamAPI.Shutdown();

		protected override void Update() => SteamAPI.RunCallbacks();
	}
}
