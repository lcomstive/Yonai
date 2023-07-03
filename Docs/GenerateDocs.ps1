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

if(Test-Path -Path "./Dist")
{ Remove-Item -Path ./Dist -Force -Recurse }

Write-Output "Generating documents for $ProjectVersion"

### Generate documentation
# Pipe output with project version to doxygen command
New-Item "./Dist/" -Type Directory

# Public facing documentation
cmd.exe /c "(type Doxyfile & echo. & echo PROJECT_NUMBER=$ProjectVersion) | doxygen -"

# Internal (C++ Engine) documentation
cmd.exe /c "(type DoxyfileInternal & echo. & echo PROJECT_NUMBER=$ProjectVersion) | doxygen -"

# Move all items in Dist/API/html to Dist/API
Move-Item -Path ./Dist/API/html/* -Destination ./Dist/API/ -Force
Remove-Item -Path ./Dist/API/html -Force -Recurse

# Move all items in Dist/InternalAPI/html to Dist/InternalAPI
Move-Item -Path ./Dist/InternalAPI/html/* -Destination ./Dist/InternalAPI -Force
Remove-Item -Path ./Dist/InternalAPI/html -Force -Recurse

# Copy Home/ folder to Dist/
Copy-Item -Path ./Home/* -Destination ./Dist -Force
Copy-Item -Path ./Home/img/* -Destination ./Dist/img -Force