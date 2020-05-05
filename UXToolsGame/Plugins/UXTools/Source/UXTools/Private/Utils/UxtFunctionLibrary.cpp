// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils/UxtFunctionLibrary.h"
#include "AudioDevice.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#endif


FTransform UUxtFunctionLibrary::GetHeadPose(UObject* WorldContextObject)
{
	FRotator rot;
	FVector pos;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(rot, pos);

	FTransform TrackingSpaceTransform(rot, pos);
	FTransform TrackingToWorld = UHeadMountedDisplayFunctionLibrary::GetTrackingToWorldTransform(WorldContextObject);
	FTransform Result;
	FTransform::Multiply(&Result, &TrackingSpaceTransform, &TrackingToWorld);
	return Result;
}

bool UUxtFunctionLibrary::IsInEditor()
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine);
		return !EdEngine->bUseVRPreviewForPlayWorld;
	}
#endif
	return false;
}

bool UUxtFunctionLibrary::HexCodePointToFString(const FString& Input, FString& Output)
{
	TCHAR Char = (TCHAR)FCString::Strtoi(*Input, nullptr, 16);
	if (Char != 0)
	{
		Output.Reset(1);
		Output.AppendChar(Char);
		return true;
	}

	Output = Input;
	return false;
}
