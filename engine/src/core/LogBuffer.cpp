#include <engine/core/LogBuffer.h>

namespace eng {

void LogBuffer::SetCapacity(std::size_t capacity) {
}

std::size_t LogBuffer::Capacity() {
    return 0;
}

void LogBuffer::Append(const LogRecord& record) {
}

void LogBuffer::Snapshot(std::vector<LogRecord>& out) {
}

void LogBuffer::Channels(std::vector<std::string>& out) {
}

std::size_t LogBuffer::Count() {
    return 0;
}

unsigned long long LogBuffer::TotalWritten() {
    return 0;
}

void LogBuffer::Clear() {
}

} // namespace eng
