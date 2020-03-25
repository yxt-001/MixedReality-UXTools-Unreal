// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Input/UxtPointerTypes.h"

/** Result of closest point search functions. */
struct FUxtPointerFocusSearchResult
{
	bool IsValid() const;

	/** Closest object that implements the requires pointer target interface. */
	UObject* Target;

	/** Primitive that contains the closes point. */
	UPrimitiveComponent* Primitive;

	/** Closest point on the primitive to the pointer position. */
	FVector ClosestPointOnTarget;

	/** Distance of the closest point to the pointer position. */
	float MinDistance;
};

/** Utility class that is used by components to manage different pointers and their focus targets. */
struct FUxtPointerFocus
{
public:

	virtual ~FUxtPointerFocus() {}

	/** Get the closest point on the surface of the focused target */
	const FVector& GetClosestTargetPoint() const;

	/** Get the currently focused target object. */
	UObject* GetFocusedTarget() const;

	/** Get the currently focused target object.
	 *  Returns null if the target does not implement the expected interface.
	 */
	UObject* GetFocusedTargetChecked() const;

	// TODO get hand joints from WMR => no need to pass PointerTransform

	/** Select and set the focused target among the list of overlaps. */
	void SelectClosestTarget(int32 PointerId, const FTransform& PointerTransform, const TArray<FOverlapResult>& Overlaps);

	/** Select the closest primitive from the owner of the given target component.
	 *  The target component will be the new focus, unless no usable primitive can be found.
	 */
	void SelectClosestPointOnTarget(int32 PointerId, const FTransform& PointerTransform, UActorComponent* NewTarget);

	/** Clear the focused target. */
	void ClearFocus(int32 PointerId);

	/** Notify the focused target of a pointer update. */
	void UpdateFocus(int32 PointerId, const FTransform& PointerTransform) const;

	/**
	 * Get the default target object.
	 * This object receives events when no other target is focused.
	 */
	UObject* GetDefaultTarget() const;

	/**
	 * Set the default target object.
	 * This object receives events when no other target is focused.
	 */
	void SetDefaultTarget(UObject* NewDefaultTarget);

protected:

	/** Set the focus to the given target object, primitive, and point on the target. */
	void SetFocus(
		int32 PointerId,
		const FTransform& PointerTransform,
		UObject* NewTarget,
		UPrimitiveComponent* NewPrimitive,
		const FVector& NewClosestPointOnTarget);

	/** Find a component of the actor that implements the required interface. */
	UActorComponent* FindInterfaceComponent(AActor* Owner) const;

	/** Find the closest target object, primitive, and point among the overlaps. */
	FUxtPointerFocusSearchResult FindClosestTarget(const TArray<FOverlapResult>& Overlaps, const FVector& Point) const;

	/** Find the closest primitive and point on the owner of the given component. */
	FUxtPointerFocusSearchResult FindClosestPointOnComponent(UActorComponent* Target, const FVector& Point) const;

	/** Get the interface class that targets for the pointer must implement. */
	virtual UClass* GetInterfaceClass() const = 0;

	/** Returns true if the given object implements the required target interface. */
	virtual bool ImplementsTargetInterface(UObject* Target) const = 0;

	/** Find the closest point on the given primitive using the distance function of the target interface. */
	virtual bool GetClosestPointOnTarget(const UActorComponent* Target, const UPrimitiveComponent* Primitive, const FVector& Point, FVector& OutClosestPoint) const = 0;

	/** Notify the target object that it has entered focus. */
	virtual void RaiseEnterFocusEvent(UObject* Target, int32 PointerId, const FUxtPointerInteractionData& Data) const = 0;
	/** Notify the focused target object that the pointer has been updated. */
	virtual void RaiseUpdateFocusEvent(UObject* Target, int32 PointerId, const FUxtPointerInteractionData& Data) const = 0;
	/** Notify the target object that it has exited focus. */
	virtual void RaiseExitFocusEvent(UObject* Target, int32 PointerId) const = 0;

private:

	/** Weak reference to the currently focused target. */
	TWeakObjectPtr<UObject> FocusedTargetWeak;

	/**
	 * Optional weak reference to a default target object
	 * that receives events if no other target is focused.
	 */
	TWeakObjectPtr<UObject> DefaultTargetWeak;

	/** Weak reference to the focused grab target primitive. */
	TWeakObjectPtr<UPrimitiveComponent> FocusedPrimitiveWeak;

	/** Closest point on the surface of the focused target. */
	FVector ClosestTargetPoint = FVector::ZeroVector;
};


/** Focus implementation for the grab pointers. */
struct FUxtGrabPointerFocus : public FUxtPointerFocus
{
public:

	/** Notify the target object that grab has started. */
	void BeginGrab(int32 PointerId, const FTransform& PointerTransform);
	/** Notify the grabbed target object that the pointer has been updated. */
	void UpdateGrab(int32 PointerId, const FTransform& PointerTransform);
	/** Notify the target object that grab has ended. */
	void EndGrab(int32 PointerId);

	bool IsGrabbing() const;

protected:

	virtual UClass* GetInterfaceClass() const override;

	virtual bool ImplementsTargetInterface(UObject* Target) const override;

	virtual bool GetClosestPointOnTarget(const UActorComponent* Target, const UPrimitiveComponent* Primitive, const FVector& Point, FVector& OutClosestPoint) const override;

	virtual void RaiseEnterFocusEvent(UObject* Target, int32 PointerId, const FUxtPointerInteractionData& Data) const override;
	virtual void RaiseUpdateFocusEvent(UObject* Target, int32 PointerId, const FUxtPointerInteractionData& Data) const override;
	virtual void RaiseExitFocusEvent(UObject* Target, int32 PointerId) const override;

private:

	bool bIsGrabbing = false;

};


/** Focus implementation for the touch pointers. */
struct FUxtTouchPointerFocus : public FUxtPointerFocus
{
protected:

	virtual UClass* GetInterfaceClass() const override;

	virtual bool ImplementsTargetInterface(UObject* Target) const override;

	virtual bool GetClosestPointOnTarget(const UActorComponent* Target, const UPrimitiveComponent* Primitive, const FVector& Point, FVector& OutClosestPoint) const override;

	virtual void RaiseEnterFocusEvent(UObject* Target, int32 PointerId, const FUxtPointerInteractionData& Data) const override;
	virtual void RaiseUpdateFocusEvent(UObject* Target, int32 PointerId, const FUxtPointerInteractionData& Data) const override;
	virtual void RaiseExitFocusEvent(UObject* Target, int32 PointerId) const override;
};
