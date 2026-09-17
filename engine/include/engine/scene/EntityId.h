#pragma once

namespace eng {

struct EntityId {
    int index      = -1;
    int generation = 0;
    bool IsNull() const { return index < 0; }
    friend bool operator==(const EntityId& a, const EntityId& b) { return a.index == b.index && a.generation == b.generation; }
    friend bool operator!=(const EntityId& a, const EntityId& b) { return !(a == b); }
    friend bool operator<(const EntityId& a, const EntityId& b) {
        if (a.index != b.index) {
            return a.index < b.index;
        }
        return a.generation < b.generation;
    }
};

} // namespace eng
