// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

/**
 * A hierarchical spatial hash grid for fast 3D volume queries.
 * Used for efficient broad-phase overlap checks in multiple resolution levels.
 */
template<typename ItemType>
class MAAASSPARTICLE_API TMPHierarchicalBoundsHashGrid3D
{
public:
    /** A single cell in the grid storing indices to items */
    struct FCell
    {
        TArray<int32> ItemIndices;
    };

    /** An individual item stored in the grid */
    struct FItem
    {
        ItemType ID; // The actual item data or ID

        FItem() {}
        FItem(ItemType InID) : ID(InID) {}
    };

    /** Identifies a cell's location and level in the hierarchy */
    struct FCellLocationVolume
    {
        FIntVector CellCoords;
        int32 Level;

        FCellLocationVolume() : CellCoords(FIntVector::ZeroValue), Level(INDEX_NONE) {}
        FCellLocationVolume(FIntVector InCellCoords, int32 InLevel) : CellCoords(InCellCoords), Level(InLevel) {}

        bool IsValid() const { return Level != INDEX_NONE; }
        void Invalidate() { Level = INDEX_NONE; }

        bool operator==(const FCellLocationVolume& Other) const { return CellCoords == Other.CellCoords && Level == Other.Level; }
        bool operator!=(const FCellLocationVolume& Other) const { return !(*this == Other); }
    };

    /** Represents a 3D bounding box in cell space */
    struct FCellBoxVolume
    {
        FIntVector Min;
        FIntVector Max;

        FCellBoxVolume() : Min(FIntVector::ZeroValue), Max(FIntVector::ZeroValue) {}
        FCellBoxVolume(FIntVector InMin, FIntVector InMax) : Min(InMin), Max(InMax) {}
    };
    TMPHierarchicalBoundsHashGrid3D();
    ~TMPHierarchicalBoundsHashGrid3D();

    /**
     * Initialize the grid hierarchy
     * @param InNumLevels - Number of resolution levels
     * @param InCellSizes - Size of a single cell per level
     */
    void Init(int32 InNumLevels, const TArray<FVector::FReal>& InCellSizes);

    /**
     * Query for overlapping items at a given point
     * @param Point - World-space point to query
     * @param Level - Grid level to search
     * @param OutOverlappingItems - Output array of overlapping item IDs
     */
    void FindOverlapping(const FVector& Point, int32 Level, TArray<ItemType>& OutOverlappingItems) const;

    /**
     * Add a new item to the grid
     * @param Item - The item to insert
     * @param Bounds - The AABB for the item
     * @param Level - Grid level to insert into
     * @return Index of the inserted item (used for later removal/move)
     */
    int32 Add(const ItemType& Item, const FBox& Bounds, int32 Level);

    /**
     * Move an existing item in the grid
     * @param ItemIndex - Index returned from Add()
     * @param OldBounds - Previous bounding box
     * @param NewBounds - New bounding box
     * @param Level - Grid level
     */
    void Move(int32 ItemIndex, const FBox& OldBounds, const FBox& NewBounds, int32 Level = 0);

   /**
	 * Remove an item from the grid
	 * @param ItemIndex - Index of the item to remove
	 * @param Bounds - Bounding box used to locate the item
	 * @param Level - Grid level
	 */
    void Remove(int32 ItemIndex, const FBox& Bounds, int32 Level = 0);

    /** Returns cell size at a specific level */
    FVector::FReal GetCellSize(int32 Level) const { check(Level >= 0 && Level < NumLevels); return CellSizes[Level]; }

    /** Accessor for debug/inspection */
    const TSparseArray<FItem>& GetItems() const { return Items; }

    /** Returns the cell for the given coordinates and level */
    const FCell* FindCell(int32 X, int32 Y, int32 Z, int32 Level = 0) const;

    /** Computes the min/max grid cell range covered by a box */
    FCellBoxVolume CalcQueryBounds(const FBox& QueryBox, int32 Level) const;

    /** Computes the grid cell index of a world location */
    FIntVector GetCellCoords(const FVector& Location, int32 Level) const;

    int32 NumLevels;
    TArray<int32> FreeItemIndices;
    
private:
    TArray<TMap<FIntVector, FCell>> CellsByLevel;
    TSparseArray<FItem> Items;
    TArray<FVector::FReal> CellSizes;

    
    FCell* FindMutableCell(int32 X, int32 Y, int32 Z, int32 Level);
    void AddItemToCell(int32 ItemIndex, FIntVector CellCoords, int32 Level);
    void RemoveItemFromCell(int32 ItemIndex, FIntVector CellCoords, int32 Level);
};

// Implementation details (directly in .h for templates)

template<typename ItemType>
TMPHierarchicalBoundsHashGrid3D<ItemType>::TMPHierarchicalBoundsHashGrid3D()
    : NumLevels(0)
{
}

template<typename ItemType>
TMPHierarchicalBoundsHashGrid3D<ItemType>::~TMPHierarchicalBoundsHashGrid3D()
{
    CellsByLevel.Empty();
    Items.Empty();
    FreeItemIndices.Empty();
    CellSizes.Empty();
}

template<typename ItemType>
void TMPHierarchicalBoundsHashGrid3D<ItemType>::Init(int32 InNumLevels, const TArray<FVector::FReal>& InCellSizes)
{
    NumLevels = InNumLevels;
    CellSizes = InCellSizes;
    CellsByLevel.SetNum(NumLevels); // Resize the array of maps
}

template<typename ItemType>
inline void TMPHierarchicalBoundsHashGrid3D<ItemType>::FindOverlapping(const FVector& Point, int32 Level, TArray<ItemType>& OutOverlappingItems) const
{
    OutOverlappingItems.Reset();

    if (Level < 0 || Level >= NumLevels)
    {
        return;
    }

    const FIntVector CellCoords = GetCellCoords(Point, Level);

    if (const FCell* Cell = FindCell(CellCoords.X, CellCoords.Y, CellCoords.Z, Level))
    {
        for (const int32 ItemIndex : Cell->ItemIndices)
        {
            if (Items.IsAllocated(ItemIndex))
            {
                // The grid does not know the item's bounding box.
                // The caller (typically a subsystem) must perform a precise AABB check.
                OutOverlappingItems.Add(Items[ItemIndex].ID);
            }
        }
    }
}

template<typename ItemType>
typename TMPHierarchicalBoundsHashGrid3D<ItemType>::FCellBoxVolume TMPHierarchicalBoundsHashGrid3D<ItemType>::CalcQueryBounds(const FBox& QueryBox, int32 Level) const
{
    check(Level >= 0 && Level < NumLevels);
    const FVector::FReal CellSize = CellSizes[Level];

    FIntVector MinCoords = GetCellCoords(QueryBox.Min, Level);
    FIntVector MaxCoords = GetCellCoords(QueryBox.Max, Level);

    // Adjust MaxCoords to ensure it covers the entire box, even if QueryBox.Max falls exactly on a cell boundary
    // This is crucial for inclusive iteration (<= MaxCoords)
    if (FMath::Fmod(QueryBox.Max.X, CellSize) == 0 && QueryBox.Max.X != QueryBox.Min.X) MaxCoords.X--;
    if (FMath::Fmod(QueryBox.Max.Y, CellSize) == 0 && QueryBox.Max.Y != QueryBox.Min.Y) MaxCoords.Y--;
    if (FMath::Fmod(QueryBox.Max.Z, CellSize) == 0 && QueryBox.Max.Z != QueryBox.Min.Z) MaxCoords.Z--;

    return FCellBoxVolume(MinCoords, MaxCoords);
}

template<typename ItemType>
const typename TMPHierarchicalBoundsHashGrid3D<ItemType>::FCell* TMPHierarchicalBoundsHashGrid3D<ItemType>::FindCell(int32 X, int32 Y, int32 Z, int32 Level) const
{
    check(Level >= 0 && Level < NumLevels);
    const TMap<FIntVector, FCell>& LevelCells = CellsByLevel[Level];
    return LevelCells.Find(FIntVector(X, Y, Z));
}

template<typename ItemType>
typename TMPHierarchicalBoundsHashGrid3D<ItemType>::FCell* TMPHierarchicalBoundsHashGrid3D<ItemType>::FindMutableCell(int32 X, int32 Y, int32 Z, int32 Level)
{
    check(Level >= 0 && Level < NumLevels);
    TMap<FIntVector, FCell>& LevelCells = CellsByLevel[Level];
    return LevelCells.Find(FIntVector(X, Y, Z));
}

template<typename ItemType>
void TMPHierarchicalBoundsHashGrid3D<ItemType>::AddItemToCell(int32 ItemIndex, FIntVector CellCoords, int32 Level)
{
    FCell& Cell = CellsByLevel[Level].FindOrAdd(CellCoords);
    Cell.ItemIndices.Add(ItemIndex);
}

template<typename ItemType>
void TMPHierarchicalBoundsHashGrid3D<ItemType>::RemoveItemFromCell(int32 ItemIndex, FIntVector CellCoords, int32 Level)
{
    if (FCell* Cell = CellsByLevel[Level].Find(CellCoords))
    {
        Cell->ItemIndices.RemoveSingleSwap(ItemIndex, false); // Use fast unordered removal since order doesn't matter.
        if (Cell->ItemIndices.IsEmpty())
        {
            CellsByLevel[Level].Remove(CellCoords);
        }
    }
}

template<typename ItemType>
int32 TMPHierarchicalBoundsHashGrid3D<ItemType>::Add(const ItemType& Item, const FBox& Bounds, int32 Level)
{
    // Try to reuse a freed index or allocate a new one
    int32 ItemIndex;
    if (FreeItemIndices.Num() > 0)
    {
        ItemIndex = FreeItemIndices.Pop(false);
        Items[ItemIndex] = FItem(Item);
    }
    else
    {
        ItemIndex = Items.Emplace(Item);
    }

    // Compute all cells overlapped by the bounding box
    const FCellBoxVolume CellBox = CalcQueryBounds(Bounds, Level);
    
    for (int32 Z = CellBox.Min.Z; Z <= CellBox.Max.Z; ++Z)
    {
        for (int32 Y = CellBox.Min.Y; Y <= CellBox.Max.Y; ++Y)
        {
            for (int32 X = CellBox.Min.X; X <= CellBox.Max.X; ++X)
            {
                AddItemToCell(ItemIndex, FIntVector(X, Y, Z), Level);
            }
        }
    }

    return ItemIndex;
}

template<typename ItemType>
void TMPHierarchicalBoundsHashGrid3D<ItemType>::Move(int32 ItemIndex, const FBox& OldBounds, const FBox& NewBounds, int32 Level)
{
    if (!Items.IsAllocated(ItemIndex)) return;
    
    // Simple implementation: remove from old bounds and re-add to new
    // TODO : Optimize by skipping unchanged cells
    Remove(ItemIndex, OldBounds, Level);
    Add(Items[ItemIndex].ID, NewBounds, Level);
}

template<typename ItemType>
void TMPHierarchicalBoundsHashGrid3D<ItemType>::Remove(int32 ItemIndex, const FBox& Bounds, int32 Level)
{
    if (!Items.IsAllocated(ItemIndex))
    {
        return;       // Invalid index
    }

    // Compute the cells the item was stored in
    const FCellBoxVolume CellBox = CalcQueryBounds(Bounds, Level);

    // Remove item index from all cells it overlapped
    for (int32 Z = CellBox.Min.Z; Z <= CellBox.Max.Z; ++Z)
    {
        for (int32 Y = CellBox.Min.Y; Y <= CellBox.Max.Y; ++Y)
        {
            for (int32 X = CellBox.Min.X; X <= CellBox.Max.X; ++X)
            {
                RemoveItemFromCell(ItemIndex, FIntVector(X, Y, Z), Level);
            }
        }
    }

    Items.RemoveAt(ItemIndex);
    FreeItemIndices.Push(ItemIndex);
}

template<typename ItemType>
FIntVector TMPHierarchicalBoundsHashGrid3D<ItemType>::GetCellCoords(const FVector& Location, int32 Level) const
{
    check(Level >= 0 && Level < NumLevels);
    const FVector::FReal CellSize = CellSizes[Level];
    return FIntVector(
        FMath::FloorToInt(Location.X / CellSize),
        FMath::FloorToInt(Location.Y / CellSize),
        FMath::FloorToInt(Location.Z / CellSize)
    );
}