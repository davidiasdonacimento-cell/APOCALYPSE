using UnrealBuildTool;

public class APOCALYPSETarget : TargetRules
{
    public APOCALYPSETarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("APOCALYPSE");
    }
}
