using Yonai;

namespace YonaiEditor.BuildProcess
{
	public interface IBuildProcess
	{
		/// <summary>
		/// When true <see cref="Execute(string)"/>'s output parameter is a folder, rather than a singular file
		/// </summary>
		bool OutputIsFolder { get; }

		/// <summary>
		/// Begins building the game and outputs to <paramref name="outputFile"/>
		/// </summary>
		/// <param name="outputFile">The expected build output as an absolute file path</param>
		void Execute(string outputFile, ProjectFile project);
	}
}