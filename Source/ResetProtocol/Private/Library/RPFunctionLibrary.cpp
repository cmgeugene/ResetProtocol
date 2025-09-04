// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/RPFunctionLibrary.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Frameworks/RPPlayerState.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "ObjectTools.h"
#include "ThumbnailRendering/ThumbnailManager.h"

FString URPFunctionLibrary::GetConnectionStringFromSessionResult(const FBlueprintSessionResult& SessionResult)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface)
        {
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(SessionResult.OnlineResult, NAME_GamePort, ConnectString))
            {
                return ConnectString;
            }
        }
    }
    // 실패한 경우 빈 문자열을 반환
    return FString();
}

void URPFunctionLibrary::BlueprintClientTravel(APlayerController* PC, const FString& URL)
{
    if (PC)
    {
        PC->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[RPFunctionLibrary] BlueprintClientTravel : 플레이어 컨트롤러 없음"));
    }
}

FPlayerInfo URPFunctionLibrary::GetMyPlayerInfo(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		return GetPlayerInfoFromController(PC);
	}
	return FPlayerInfo();
}

FPlayerInfo URPFunctionLibrary::GetPlayerInfoFromController(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		ARPPlayerState* RPPlayerState = PlayerController->GetPlayerState<ARPPlayerState>();
		if (RPPlayerState)
		{
			UPlayerInfoComponent* InfoComponent = RPPlayerState->GetPlayerInfoComponent();
			if (InfoComponent)
			{
				return InfoComponent->GetPlayerInfo();
			}
		}
	}
	return FPlayerInfo();
}



bool URPFunctionLibrary::ExportAssetThumbnail(UObject* InAsset, const FString& InDirectoryPath, const FString& InFileName, const FIntPoint& InResolution)
{
    // 1. 에셋 유효성 검사
    if (!InAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("ExportAssetThumbnail: Invalid asset"));
        return false;
    }

    // 2. 썸네일 생성 및 가져오기
    // 썸네일이 존재하지 않거나 최신이 아닌 경우 강제로 생성합니다.
    
    FObjectThumbnail* Thumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(InAsset);
    if (!Thumbnail)
    {
        UE_LOG(LogTemp, Warning, TEXT("ExportAssetThumbnail: thumbnail exists or it's not an asset type with a visual representation."), *InAsset->GetName());
        return false;
    }
    if (Thumbnail->GetUncompressedImageData().Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ExportAssetThumbnail: Thumbnail for asset %s has no uncompressed image data. This can happen if the thumbnail is not yet generated or is corrupted."), *InAsset->GetName());
        return false;
    }

    // 3. 이미지 데이터 리사이즈 및 인코딩
    TArray<uint8> CompressedData;
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    // 썸네일의 원본 해상도
    int32 OriginalWidth = Thumbnail->GetImageWidth();
    int32 OriginalHeight = Thumbnail->GetImageHeight();

    // 썸네일 데이터를 RGBA 형식으로 변환 (필요시)
    const TArray<uint8>& RawBGRAData = Thumbnail->GetUncompressedImageData();
    TArray<uint8> RawRGBAData;
    RawRGBAData.SetNumUninitialized(OriginalWidth * OriginalHeight * 4);

    // BGRA to RGBA conversion
    for (int32 i = 0; i < OriginalWidth * OriginalHeight; ++i)
    {
        RawRGBAData[i * 4 + 0] = RawBGRAData[i * 4 + 2]; // R
        RawRGBAData[i * 4 + 1] = RawBGRAData[i * 4 + 1]; // G
        RawRGBAData[i * 4 + 2] = RawBGRAData[i * 4 + 0]; // B
        RawRGBAData[i * 4 + 3] = RawBGRAData[i * 4 + 3]; // A
    }

    // Resizing logic (Nearest Neighbor for simplicity)
    TArray<uint8> ResizedRGBAData;
    int32 TargetWidth = InResolution.X;
    int32 TargetHeight = InResolution.Y;

    if (TargetWidth > 0 && TargetHeight > 0 && (TargetWidth != OriginalWidth || TargetHeight != OriginalHeight))
    {
        ResizedRGBAData.SetNumUninitialized(TargetWidth * TargetHeight * 4);
        float ScaleX = (float)OriginalWidth / TargetWidth;
        float ScaleY = (float)OriginalHeight / TargetHeight;

        for (int32 Y = 0; Y < TargetHeight; ++Y)
        {
            for (int32 X = 0; X < TargetWidth; ++X)
            {
                int32 SrcX = FMath::Min((int32)(X * ScaleX), OriginalWidth - 1);
                int32 SrcY = FMath::Min((int32)(Y * ScaleY), OriginalHeight - 1);

                int32 SrcIndex = (SrcY * OriginalWidth + SrcX) * 4;
                int32 DstIndex = (Y * TargetWidth + X) * 4;

                ResizedRGBAData[DstIndex + 0] = RawRGBAData[SrcIndex + 0];
                ResizedRGBAData[DstIndex + 1] = RawRGBAData[SrcIndex + 1];
                ResizedRGBAData[DstIndex + 2] = RawRGBAData[SrcIndex + 2];
                ResizedRGBAData[DstIndex + 3] = RawRGBAData[SrcIndex + 3];
            }
        }
    }
    else
    {
        ResizedRGBAData = RawRGBAData; // No resizing needed
        TargetWidth = OriginalWidth;
        TargetHeight = OriginalHeight;
    }

    if (ImageWrapper->SetRaw(ResizedRGBAData.GetData(), ResizedRGBAData.Num(), TargetWidth, TargetHeight, ERGBFormat::RGBA, 8))
    {
        CompressedData = ImageWrapper->GetCompressed(5); // 5는 압축 품질 (0-9)
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ExportAssetThumbnail: Failed to set raw image data for asset %s"), *InAsset->GetName());
        return false;
    }

    // 4. 파일로 저장
    FString FullExportPath = FPaths::Combine(*InDirectoryPath, InFileName + TEXT(".png"));

    // 디렉토리가 없으면 생성
    IFileManager::Get().MakeDirectory(*InDirectoryPath, true);

    if (CompressedData.Num() > 0)
    {
        if (FFileHelper::SaveArrayToFile(CompressedData, *FullExportPath))
        {
            UE_LOG(LogTemp, Log, TEXT("ExportAssetThumbnail: Successfully exported thumbnail for %s to %s"), *InAsset->GetName(), *FullExportPath);
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ExportAssetThumbnail: Failed to save thumbnail to file %s"), *FullExportPath);
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ExportAssetThumbnail: Compressed image data is empty for asset %s"), *InAsset->GetName());
        return false;
    }
}


