# Get Git information
. .\GetGitInfo.ps1

# Set directory to /Docs/
Set-Location -Path ../../Docs

Write-Output "Generating documents for $ProjectVersion"

### Generate documentation
# Pipe output with project version to doxygen command
cmd.exe /c "(type Doxyfile & echo. & echo PROJECT_NUMBER=$ProjectVersion) | doxygen -"

# Restore location
Set-Location -Path ../Platforms/Scripts