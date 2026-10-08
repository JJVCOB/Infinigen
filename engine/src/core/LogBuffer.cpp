#include <engine/core/LogBuffer.h>
#include <algorithm>
#include <set>

namespace eng {
namespace {

std::vector<LogRecord> g_ring;
std::size_t g_capacity = LogBuffer::kDefaultCapacity;
std::size_t g_head, g_size, g_total = 0;
std::set<std::string> g_channels;

} // namespace

void LogBuffer::SetCapacity(std::size_t capacity) {
    if (capacity == 0) { capacity = 1; }
    g_capacity = capacity;
    g_ring.clear();
    g_ring.shrink_to_fit();
    g_head = 0;
    g_size = 0;
}

std::size_t LogBuffer::Capacity() { return g_capacity; }

void LogBuffer::Append(const LogRecord& record) {
    if (g_ring.size() < g_capacity) { g_ring.resize(g_capacity); }
    LogRecord stored = record;
    stored.sequence = ++g_total;
    g_ring[g_head] = std::move(stored);
    g_head = (g_head + 1) % g_capacity; // After the last slot, index 0 again
    g_size = std::min(g_size + 1, g_capacity); // Grows until ring is full, then stays there forever
    g_channels.insert(record.channel);
}

void LogBuffer::Snapshot(std::vector<LogRecord>& out) {
    out.clear();
    out.reserve(g_size);
    const std::size_t first = (g_size == g_capacity) ? g_head : 0;

    for (std::size_t i = 0; i < g_size; ++i) {
        out.push_back(g_ring[(first + i) % g_capacity]);
    }
}

void LogBuffer::Channels(std::vector<std::string>& out) { out.assign(g_channels.begin(), g_channels.end()); }
std::size_t LogBuffer::Count() { return g_size; }
std::size_t LogBuffer::TotalWritten() { return g_total; }
void LogBuffer::Clear() { g_head, g_size = 0; }

} // namespace eng
