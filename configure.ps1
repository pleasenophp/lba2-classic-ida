<#
.SYNOPSIS
    A script to configure the project by generating cfg-defines.h.

.DESCRIPTION
    This script generates cfg-defines.h based on the build type (Debug or Release) and user input.
    In Debug mode, it prompts the user to select the LBA2 Common directory, ensuring it contains LBA2.HQR.

.PARAMETER BuildType
    Specifies the build type. Acceptable values are "Debug" or "Release".

.EXAMPLE
    ./configure.ps1 -BuildType Debug
#>

param (
    [string]$BuildType = "Debug"
)

# Ensure the BuildType parameter is valid
if ($BuildType -notin @("Debug", "Release")) {
    Write-Error "Invalid BuildType. Use 'Debug' or 'Release'."
    exit 1
}

# Function to show folder selection dialog
function Select-Folder {
    Add-Type -AssemblyName System.Windows.Forms
    $dialog = New-Object System.Windows.Forms.FolderBrowserDialog
    $dialog.Description = "Select LBA2 Common directory (where HQR files are):"
    $dialog.ShowNewFolderButton = $true
    if ($dialog.ShowDialog() -eq [System.Windows.Forms.DialogResult]::OK) {
        return $dialog.SelectedPath
    } else {
        Write-Host "No folder selected. Exiting." -ForegroundColor Yellow
        exit 1
    }
}

# Function to validate directory contains LBA2.HQR
function Validate-LBA2Directory($directory) {
    $requiredFile = Join-Path $directory "LBA2.HQR"
    return Test-Path $requiredFile
}

# Function to normalize paths and ensure they end with a single backslash and wrapped in quotes
function Normalize-Path($path) {
    $normalizedPath = $path -replace '\\+', '\\'
    return "`"$normalizedPath`""
}

# Prompt user to select LBA2 Common directory (Debug mode only)
$pathResource = ""
if ($BuildType -eq "Debug") {
    do {
        Write-Host "Opening folder selection dialog for LBA2 Common directory..."
        $pathResource = Select-Folder
        if (-not (Validate-LBA2Directory $pathResource)) {
            Add-Type -AssemblyName System.Windows.Forms
            [System.Windows.Forms.MessageBox]::Show("LBA2.HQR not found in the selected directory. Please select a valid LBA2 Common directory.", "Error", [System.Windows.Forms.MessageBoxButtons]::OK, [System.Windows.Forms.MessageBoxIcon]::Error) | Out-Null
        }
    } while (-not (Validate-LBA2Directory $pathResource))
    Write-Host "Selected valid LBA2 Common directory: $pathResource" -ForegroundColor Green
}

# Initialize variables based on build type
$scriptDirectory = Split-Path -Parent $MyInvocation.MyCommand.Definition
$pathSave = Normalize-Path "SAVE\"
$pathPcxSave = Normalize-Path "SAVE\SHOOT\"
$pathSaveBugs = Normalize-Path "BUGS\"
$displayFps = 0
$fullscreen = 1

if ($BuildType -eq "Debug") {
    # Create debug folders if they don't exist
    $debugFolders = @(
        "GameRun",
        "GameRun\\save",
        "GameRun\\save\\shoot",
        "GameRun\\bugs"
    )

    foreach ($folder in $debugFolders) {
        $fullPath = Join-Path $scriptDirectory $folder
        if (-not (Test-Path $fullPath)) {
            New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        }
    }

    # Copy LBA2.cfg from SOURCES to GameRun folder only if it doesn't already exist
    if (-not (Test-Path (Join-Path "GameRun" "LBA2.cfg"))) {
        Copy-Item -Path (Join-Path "SOURCES" "LBA2.cfg") -Destination (Join-Path "GameRun" "LBA2.cfg")
    }

    $pathResource = Normalize-Path $pathResource
    $pathSave = Normalize-Path (Join-Path $scriptDirectory "GameRun\save\")
    $pathPcxSave = Normalize-Path (Join-Path $scriptDirectory "GameRun\save\shoot\")
    $pathSaveBugs = Normalize-Path (Join-Path $scriptDirectory "GameRun\bugs\")
    $displayFps = 1
    $fullscreen = 0
} else {
    $pathResource ="`"`""
}

# Path to template and output files
$templateFile = "configure.defines.h"
$outputFile = "cfg-defines.h"

# Ensure the template file exists
if (-not (Test-Path $templateFile)) {
    Write-Error "Template file '$templateFile' not found. Ensure it exists in the project directory."
    exit 1
}

# Generate the cfg-defines.h file
Write-Host "Generating $outputFile with the following values:"
Write-Host "  PATH_RESOURCE: $pathResource"
Write-Host "  PATH_SAVE: $pathSave"
Write-Host "  PATH_PCX_SAVE: $pathPcxSave"
Write-Host "  PATH_SAVE_BUGS: $pathSaveBugs"
Write-Host "  DISPLAY_FPS: $displayFps"
Write-Host "  FULLSCREEN: $fullscreen"

$templateContent = Get-Content $templateFile -Raw
$templateContent = $templateContent -replace "\$\{PATH_RESSOURCE\}", $pathResource
$templateContent = $templateContent -replace "\$\{PATH_SAVE\}", $pathSave
$templateContent = $templateContent -replace "\$\{PATH_PCX_SAVE\}", $pathPcxSave
$templateContent = $templateContent -replace "\$\{PATH_SAVE_BUGS\}", $pathSaveBugs
$templateContent = $templateContent -replace "\$\{DISPLAY_FPS\}", $displayFps
$templateContent = $templateContent -replace "\$\{FULLSCREEN\}", $fullscreen

$outputFileSources = Join-Path "SOURCES" $outputFile
$outputFileLib386 = Join-Path "LIB386" $outputFile
Set-Content -Path $outputFileSources -Value $templateContent
Set-Content -Path $outputFileLib386 -Value $templateContent

Write-Host "$outputFile has been successfully generated. You can now build the game" -ForegroundColor Green
