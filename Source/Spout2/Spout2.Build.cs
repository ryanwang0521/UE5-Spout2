// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class Spout2 : ModuleRules
{
	public Spout2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		string thirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/"));
		
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(thirdPartyPath, "Spout/include"),
				// ... add public include paths required here ...
			}
			);
		
		string spoutLibPath = Path.Combine(thirdPartyPath, "Spout", "lib", "amd64");
		
		LoadLib(spoutLibPath, new string[]
		{
			"SpoutDX"
		});

		LoadDll(spoutLibPath, new string[]
		{
			"SpoutDX"
		});
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RenderCore",
				"RHI",
				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		void LoadLib(string path, string[] names)
		{
			foreach (string item in names)
			{
				PublicAdditionalLibraries.Add(Path.Combine(path, item + ".lib"));
			}
		}

		void LoadDll(string path, string[] names)
		{
			string binariesPath = Path.Combine(PluginDirectory, "Binaries/Win64/");
			Directory.CreateDirectory(binariesPath);
			foreach (string item in names)
			{
				string fileName = item + ".dll";
				string srcPath = Path.Combine(path, fileName);
				string destPath = Path.Combine(binariesPath, fileName);
				if (!File.Exists(destPath))
				{
					File.Copy(srcPath, destPath, true);
				}

				RuntimeDependencies.Add($"$(TargetOutputDir)/{item}.dll",destPath);
				Console.WriteLine($"Spout2: {srcPath} copied to {destPath}");
			}
		}
	}
}
