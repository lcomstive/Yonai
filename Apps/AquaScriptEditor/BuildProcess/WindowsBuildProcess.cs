using System.IO;
using AquaEngine.IO;

namespace AquaEditor.BuildProcess
{
	public class WindowsBuildProcess : IBuildProcess
	{
		public bool OutputIsFolder => true;

		public void Execute(string outputFolder, ProjectFile project)
		{
			VFS.Mount("build://", outputFolder);

			// Launching executable
			VFS.Copy("app://BaseGame.exe", $"build://{project.Name}.exe");

			// Mono assemblies
			VFS.CreateDirectory("build://Assets/Mono");
			VFS.Copy("mono://", "build://Assets/Mono/");

			// Resources file
			VFS.Copy("project://Resources.json", "build://Assets/Resources.json");

			// Project assemblies
			VFS.CreateDirectory("build://Assets/Assemblies");
			foreach (string assemblyPath in project.Assemblies)
				VFS.Copy(assemblyPath, "build://Assets/Assemblies/" + Path.GetFileName(assemblyPath));

			// Project files
			VFS.CreateDirectory("build://Assets/ProjectFiles");
			VFS.Copy("project://Assets", "build://Assets/ProjectFiles");

			// If base game was compiled as shared library, will need to copy dependency .dll files
			VFSFile[] localFiles = VFS.GetFiles("app://");
			foreach (var file in localFiles)
				if(file.Extension == ".dll" && file.FileName != "AquaScriptCore.dll" && file.FileName != "AquaScriptEditor.dll")
					VFS.Copy(file, "build://");

			// Unmount virtual mount
			VFS.Unmount("build://");
		}
	}
}