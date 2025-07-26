// Fill out your copyright notice in the Description page of Project Settings.


#include "MPDebugLogTask.h"
#include "MassStateTreeExecutionContext.h"
#include "Engine/Engine.h" 

bool FMPDebugLogTask::Link(FStateTreeLinker& Linker)
{
    return Super::Link(Linker);
}

void FMPDebugLogTask::GetDependencies(UE::MassBehavior::FStateTreeDependencyBuilder& Builder) const
{
    Super::GetDependencies(Builder);
}

EStateTreeRunStatus FMPDebugLogTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    const double ExecutionTime = FPlatformTime::Seconds();
    UE_LOG(LogTemp, Log, TEXT("[StateTree Task] Executing at T=%.4f"), ExecutionTime);
    // 인스턴스 데이터(디테일 패널에서 설정한 값들)를 가져옵니다.
    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    // GEngine이 유효한지 확인하고 화면에 문자열을 출력합니다.
    if (GEngine)
    {
        // FString::Printf를 사용하여 엔티티 ID와 같은 동적인 정보도 포함할 수 있습니다.
        const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(Context);
        const FMassEntityHandle Entity = MassContext.GetEntity();
        const double EnterTime = FPlatformTime::Seconds();
        const FString FullEnterString = FString::Printf(TEXT("[T:%.4f] Entity[%d:%d]: %s"),
            EnterTime, Entity.Index, Entity.SerialNumber, *InstanceData.EnterString);

        GEngine->AddOnScreenDebugMessage(-1, InstanceData.Duration, InstanceData.TextColor, FullEnterString);
    }

    // 이 태스크는 즉시 완료되므로 Succeeded를 반환합니다.
    return EStateTreeRunStatus::Succeeded;
}

void FMPDebugLogTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    const double ExecutionTime = FPlatformTime::Seconds();
    UE_LOG(LogTemp, Log, TEXT("[StateTree Task] Executing at T=%.4f"), ExecutionTime);
    // 인스턴스 데이터를 가져옵니다.
    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    // GEngine이 유효한지 확인하고 화면에 문자열을 출력합니다.
    if (GEngine)
    {
        const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(Context);
        const FMassEntityHandle Entity = MassContext.GetEntity();
        const FString FullExitString = FString::Printf(TEXT("Entity[%d:%d]: %s"), Entity.Index, Entity.SerialNumber, *InstanceData.ExitString);

        GEngine->AddOnScreenDebugMessage(-1, InstanceData.Duration, InstanceData.TextColor, FullExitString);
    }
}