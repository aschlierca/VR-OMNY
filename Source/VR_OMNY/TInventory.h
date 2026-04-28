#pragma once
#include <vector>
#include <stdexcept>

// Generic container — satisfies template concept requirement
template<typename T>
class TInventory
{
public:
    void Add(const T& Item)       { Items.push_back(Item); }
    int32 Count()           const { return static_cast<int32>(Items.size()); }
    bool  IsEmpty()         const { return Items.empty(); }
    void  Clear()                 { Items.clear(); }

    const T& Get(int32 Index) const
    {
        if (Index < 0 || Index >= Count())
            throw std::out_of_range("TInventory: index out of range");
        return Items[static_cast<size_t>(Index)];
    }

    void Remove(int32 Index)
    {
        if (Index < 0 || Index >= Count())
            throw std::out_of_range("TInventory: index out of range");
        Items.erase(Items.begin() + Index);
    }

private:
    std::vector<T> Items;
};
