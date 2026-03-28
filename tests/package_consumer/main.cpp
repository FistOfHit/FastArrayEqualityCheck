#include <fae/fae.h>

#include <cstdint>
#include <span>
#include <vector>

int main() {
    const std::vector<std::int32_t> values {1, 2, 3, 4};
    const fae::Fingerprint result = fae::fingerprint(std::span(values));
    return result.length == values.size() ? 0 : 1;
}
