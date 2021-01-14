# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk

Compress-Archive -Path "./libs/arm64-v8a/libmod-list.so", "./extern/libbeatsaber-hook_1_0_9.so", "./extern/libcodegen_0_5_3.so", "./extern/libcustom-types.so", "./extern/libquestui.so", "cover.jpg", "./bmbfmod.json" -DestinationPath "./mod-list_v0.1.1.zip" -Update
