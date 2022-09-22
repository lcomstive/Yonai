# Retrieve version from file
$ProjectVersion = Get-Content -Path ../../Version.txt

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
