# Retrieve version from file
$ProjectVersion = Get-Content -Path ../Version.txt

# Get current branch
$GitBranch = (git rev-parse --abbrev-ref HEAD)

# Check if Git branch is not main
if($GitBranch -ne 'main')
{
	# Retrieve Git repository info
	$GitHash = (git log --pretty=format:'%h' -n 1)

	# Update project version to include Git info
	$ProjectVersion = "$ProjectVersion-$GitHash [$GitBranch]"
}

Write-Output "Generating documents for $ProjectVersion"

### Generate documentation
# Pipe output with project version to doxygen command
cmd.exe /c "(type Doxyfile & echo. & echo PROJECT_NUMBER=$ProjectVersion) | doxygen -"
