using UnrealBuildTool;

public class APOCALYPSE : ModuleRules
{
    public APOCALYPSE(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore",
            "AIModule", "NavigationSystem", "HTTP", "Json", "JsonUtilities"
        });
    }
}
