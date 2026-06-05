#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint32_t read_u32_be(const std::vector<uint8_t> &data, size_t off) {
    return (static_cast<uint32_t>(data[off]) << 24) |
           (static_cast<uint32_t>(data[off + 1]) << 16) |
           (static_cast<uint32_t>(data[off + 2]) << 8) |
           static_cast<uint32_t>(data[off + 3]);
}

uint16_t read_u16_be(const std::vector<uint8_t> &data, size_t off) {
    return (static_cast<uint16_t>(data[off]) << 8) |
           static_cast<uint16_t>(data[off + 1]);
}

std::vector<uint8_t> read_file(const char *path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), {});
}

void parse_title(const uint8_t *payload, uint32_t size) {
    if (size == 0) {
        return;
    }

    uint8_t title_len = payload[0];
    char title[32];

    // Bug: title_len is trusted. A long title block overflows title.
    for (uint8_t i = 0; i < title_len; ++i) {
        title[i] = static_cast<char>(payload[1 + i]);
    }
    title[31] = '\0';

    if (title[0] == 'x') {
        std::cout << title << std::endl;
    }
}

void parse_seek_table(const uint8_t *payload, uint32_t size) {
    if (size < 2) {
        return;
    }

    uint16_t count = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
    uint32_t offsets[16] = {};

    // Bug: count is trusted. More than 16 entries corrupts the stack.
    for (uint16_t i = 0; i < count; ++i) {
        size_t off = 2 + static_cast<size_t>(i) * 4;
        if (off + 4 > size) {
            return;
        }
        offsets[i] = (static_cast<uint32_t>(payload[off]) << 24) |
                     (static_cast<uint32_t>(payload[off + 1]) << 16) |
                     (static_cast<uint32_t>(payload[off + 2]) << 8) |
                     payload[off + 3];
    }

    if (offsets[0] == 0x41414141) {
        std::cout << "first seek point selected" << std::endl;
    }
}

bool parse(const std::vector<uint8_t> &data) {
    if (data.size() < 8) {
        return false;
    }
    if (std::string(reinterpret_cast<const char *>(data.data()), 4) != "IMAF") {
        return false;
    }

    size_t off = 4;
    uint8_t blocks = data[off++];
    for (uint8_t i = 0; i < blocks; ++i) {
        if (off + 5 > data.size()) {
            return false;
        }
        uint8_t type = data[off++];
        uint32_t size = read_u32_be(data, off);
        off += 4;
        if (off + size > data.size()) {
            return false;
        }

        const uint8_t *payload = data.data() + off;
        if (type == 1) {
            parse_title(payload, size);
        } else if (type == 2) {
            parse_seek_table(payload, size);
        }
        off += size;
    }

    if (off + 2 <= data.size()) {
        (void)read_u16_be(data, off);
    }
    return true;
}

}  // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        return 2;
    }
    auto data = read_file(argv[1]);
    return parse(data) ? 0 : 1;
}
