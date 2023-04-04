using Newtonsoft.Json.Linq;

namespace AquaEngine.IO
{
	public interface ISerializable
	{
		/// <summary>
		/// Called when serializing this object
		/// </summary>
		JObject OnSerialize();

		/// <summary>
		/// Called when deserializing this object
		/// </summary>
		void OnDeserialize(JObject json);
	}
}