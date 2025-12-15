param (
    [Parameter(Mandatory = $true)]
    [ValidateSet("release", "debug", "clean")]
    [string]$Action
)

switch ($Action) {
    "release" {
        Write-Host "🔧 Configuring and building in Release mode..." -ForegroundColor Cyan
        cmake --preset=release
        cmake --build build/release
    }

    "debug" {
        Write-Host "🔧 Configuring and building in Debug mode..." -ForegroundColor Cyan
        cmake --preset=default
        cmake --build build/debug
    }

    "clean" {
        $buildDir = "build"
        if (Test-Path $buildDir) {
            Write-Host "🧹 Cleaning build directory..." -ForegroundColor Yellow
            Remove-Item -Recurse -Force $buildDir
            Write-Host "✅ Build directory removed." -ForegroundColor Green
        } else {
            Write-Host "ℹ️ No build directory found to clean." -ForegroundColor Gray
        }
    }
}
