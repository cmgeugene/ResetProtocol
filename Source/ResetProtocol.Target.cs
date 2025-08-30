// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ResetProtocolTarget : TargetRules
{
	public ResetProtocolTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("ResetProtocol");

        bOverrideBuildEnvironment = true;
        AdditionalCompilerArguments = "/Zm500"; // PCH heap limit ´Ã¸®±â (±âº» 100)
    }
}
