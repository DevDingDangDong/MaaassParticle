// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassStateTreeTypes.h"
#include "MPDebugLogTask.generated.h"

/**
 * Instance data for the debug log task.
 * Contains the messages, display duration, and color used for screen logging when entering or exiting a state.
 */
USTRUCT()
struct FMPDebugLogTaskInstanceData
{
    GENERATED_BODY()

    /** Message to display when the state is entered */
    UPROPERTY(EditAnywhere, Category = "Log")
    FString EnterString = TEXT("Enter State");

    /** Message to display when the state is exited */
    UPROPERTY(EditAnywhere, Category = "Log")
    FString ExitString = TEXT("Exit State");

    /** Duration (in seconds) to display the message on screen */
    UPROPERTY(EditAnywhere, Category = "Log", meta = (UIMin = 0.0, ClampMin = 0.0))
    float Duration = 5.0f;

    /** Color used for the log message text */
    UPROPERTY(EditAnywhere, Category = "Log")
    FColor TextColor = FColor::Yellow;
};

/**
 * A simple Mass StateTree task that prints debug messages to the screen when entering or exiting a state.
 */

USTRUCT(meta = (DisplayName = "MP Debug Log Task"))
struct FMPDebugLogTask : public FMassStateTreeTaskBase
{
    GENERATED_BODY()

    /** Declares the struct that provides instance data to this task */
    using FInstanceDataType = FMPDebugLogTaskInstanceData;

protected:
    /** Informs the StateTree editor of the instance data struct used by this task */
    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

    /** Links task bindings and runtime access to data */
    virtual bool Link(FStateTreeLinker& Linker) override;
    
    /** Declares any fragment or shared data dependencies */
    virtual void GetDependencies(UE::MassBehavior::FStateTreeDependencyBuilder& Builder) const override;

    /** Called when the state is entered */
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

    /** Called when the state is exited */
    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};