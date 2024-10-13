#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <type_traits>

class bitstream {
  private:
    static constexpr uint32_t MIN_REALLOC = 10;

  public:
    bitstream()
        : m_pos(0), m_size(0), m_alloc_size(0), m_data(nullptr),
          m_read_only(false) {}

    /**
     * @brief bitstream
     * @param data
     * @param length
     *
     * NOTE: Use assign() if you want to prevent memcpy
     */
    bitstream(const uint8_t *data, const uint32_t length)
        : m_pos(0), m_size(0), m_alloc_size(MIN_REALLOC), m_data(nullptr),
          m_read_only(false) {
        m_data = reinterpret_cast<uint8_t *>(malloc(m_alloc_size));

        if (length > 0) {
            write_raw_data(data, length, false);
        }
    }

    bitstream(bitstream &&other) noexcept
        : m_pos(other.m_pos), m_size(other.m_size),
          m_alloc_size(other.m_alloc_size), m_data(other.m_data),
          m_read_only(other.m_read_only) {
        other.m_data = nullptr;
    }

    bitstream(const bitstream &other) = delete;

    ~bitstream() { clear(); }

    [[nodiscard]]
    bool is_read_only() const {
        return m_read_only;
    }

    /// Create read-only bitstream that accesses this bitstream's data
    [[nodiscard]]
    bitstream make_view() const {
        bitstream view;
        view.assign(data(), size(), true);
        return view;
    }

    /**
     * Allocate initial buffer space so we reduce the number of mallocs
     */
    void pre_alloc(uint32_t size) {
        if (size < m_alloc_size) {
            throw std::runtime_error("Can only increase alloc size");
        }

        m_alloc_size = size;
        m_data = reinterpret_cast<uint8_t *>(realloc(m_data, m_alloc_size));

        if (m_data == nullptr) {
            throw std::runtime_error("Failed to allocated data");
        }
    }

    /**
     * Create an identical copy of this bitstream
     *
     * \param force_copy [optional]
     *      Ensure the copy has it's own dedicated copy of the data
     */
    [[nodiscard]]
    bitstream duplicate(bool force_copy = false) const {
        bitstream result;

        if (m_read_only && !force_copy) {
            result.assign(m_data, m_size, true);
        } else {
            result = bitstream(m_data, m_size);
        }

        result.move_to(pos());
        return result;
    }

    void clear() {
        // NOLINTNEXTLINE
        if (!m_read_only && m_data != nullptr) {
            // NOLINTNEXTLINE
            free(m_data);
        }

        m_read_only = false;
        m_data = nullptr;
        m_size = m_alloc_size = 0;
    }

    bitstream &operator=(bitstream &&other) noexcept {
        clear();

        m_data = other.m_data;
        m_read_only = other.m_read_only;
        m_pos = other.m_pos;
        m_size = other.m_size;
        m_alloc_size = other.m_alloc_size;

        other.m_data = nullptr;
        other.m_pos = other.m_size = other.m_alloc_size = 0;

        return *this;
    }

    void resize(uint32_t new_size) {
        if (m_read_only) {
            throw std::runtime_error("Cannot resize: is read-only!");
        }

        if (m_data == nullptr) {
            m_size = m_alloc_size = new_size;
            if (new_size > 0) {
                m_data = reinterpret_cast<uint8_t *>(malloc(new_size));
            }
            return;
        }

        if (new_size < m_size) {
            m_size = new_size;
            // no reason to change alloc size?
            return;
        }

        if (new_size <= m_alloc_size) {
            m_size = new_size;
            return;
        }

        const auto step = std::max(MIN_REALLOC, new_size - m_size);
        pre_alloc(m_alloc_size + step);

        m_size = new_size;
    }

    /**
     * Write a raw buffer to the bitstream
     *
     * @note This will raise an exception if data is a nullpointer or length is
     * of size 0
     */
    bitstream &write_raw_data(const uint8_t *data, const uint32_t length,
                              bool advance = true) {
        if (data == nullptr || length == 0) {
            throw std::runtime_error("Invalid parameters");
        }

        if (m_pos + length > size()) {
            resize(m_pos + length);
        }

        memcpy(&m_data[m_pos], data, length);

        if (advance) {
            m_pos += length;
        }

        return *this;
    }

    void make_space(uint32_t increase) {
        if (m_read_only) {
            throw std::runtime_error("Cannot make space in read only mode");
        }

        auto remain = remaining_size();
        resize(size() + increase);
        memmove(current() + increase, current(), remain);
    }

    void remove_space(uint32_t decrease) {
        if (decrease > remaining_size()) {
            throw std::runtime_error("Not enoug space left");
        }

        memmove(current(), current() + decrease, remaining_size() - decrease);
        resize(size() - decrease);
    }

    void read_raw_data(uint8_t **data, uint32_t length) {
        if (m_pos + length > size()) {
            throw std::runtime_error(
                "Cannot read from bitstream. Already at the end.");
        }

        *data = &m_data[m_pos];
        m_pos += length;
    }

#ifndef IS_ENCLAVE
    bitstream &operator<<(std::ifstream &file);
#endif

    /**
     * Write virtually any kind of data to the stream
     *
     * @note data is expected not to be a pointer. you might have to dereference
     * it
     */
    template <typename T> bitstream &operator<<(const T &data) {
        static_assert(
            std::is_trivially_copyable<T>(),
            "Need a specialized serialized function for non-POD types");

        if (m_pos + sizeof(T) > size()) {
            const uint32_t newSize = m_pos + sizeof(T);
            resize(newSize);
        }

        memcpy(&m_data[m_pos], &data, sizeof(T));
        m_pos += sizeof(T);

        return *this;
    }

    /**
     * Read virtually any kind of data from the stream
     *
     * @note data is expected not to be a pointer. you might have to dereference
     * it
     */
    template <typename T> bitstream &operator>>(T &data) {
        if (m_pos + sizeof(T) > size()) {
            throw std::runtime_error(
                "Cannot read from bitstream: Already at the end.");
        }

        memcpy(&data, &m_data[m_pos], sizeof(T));
        m_pos += sizeof(T);

        return *this;
    }

    /**
     * @brief Access raw content of the stream
     * @note This should only be used internally to write to the socket
     */
    [[nodiscard]]
    const uint8_t *data() const {
        return m_data;
    }

    uint8_t *data() { return m_data; }

    void detach(uint8_t *&out, uint32_t &len) {
        if (m_size == 0) {
            out = nullptr;
            // there might be pre-allocated buffer
            free(m_data);
        } else if (m_size < m_alloc_size) {
            out = reinterpret_cast<uint8_t *>(realloc(m_data, m_size));
        } else if (m_size == m_alloc_size) {
            out = m_data;
        } else {
            throw std::runtime_error("invalid state: m_size > m_alloc_size");
        }

        len = m_size;

        m_data = nullptr;
        m_size = 0;
        m_alloc_size = 0;
    }

    [[nodiscard]]
    int64_t hash() const {
        int64_t result = 0;
        auto *ptr = reinterpret_cast<uint8_t *>(&result);

        for (uint32_t i = 0; i < this->size(); ++i) {
            ptr[i % 8] = ptr[i % 8] ^ m_data[i];
        }

        return result;
    }

    void assign(const uint8_t *buffer, uint32_t len, bool read_only) {
        if (read_only == false) {
            throw std::runtime_error(
                "Cannot use const reference and writable buffer");
        }

        assign(const_cast<uint8_t *>(buffer), len, true);
    }

    void assign(uint8_t *buffer, uint32_t len, bool read_only = false) {
        clear();

        m_data = buffer;
        m_size = m_alloc_size = len;
        m_pos = 0;
        m_read_only = read_only;
    }

    /**
     * @brief Return the current size of the stream
     */
    [[nodiscard]]
    size_t size() const {
        return m_size;
    }

    [[nodiscard]]
    bool empty() const {
        return m_size == 0;
    }

    [[nodiscard]]
    size_t remaining_size() const {
        return size() - pos();
    }

    [[nodiscard]]
    size_t allocated_size() const {
        return m_alloc_size;
    }

    /**
     * @brief Did we read until the end of the stream
     * @note This only makes sense when reading. Writing dynamically expands the
     * stream
     */
    [[nodiscard]]
    bool at_end() const {
        return m_pos == m_size;
    }

    /**
     * @brief Get the currenty position
     * @return Returns the position (in Bytes)
     */
    [[nodiscard]]
    uint32_t pos() const {
        return m_pos;
    }

    bool move_to(uint32_t pos, bool allocate = false) {
        if (pos > m_size) {
            if (allocate) {
                resize(pos + 1);
            } else {
                return false;
            }
        }

        m_pos = pos;
        return true;
    }

    bool move_by(int32_t offset, bool allocate = false) {
        // make sure we don't overflow
        if (offset < 0 && static_cast<uint32_t>((-1) * offset) > m_pos) {
            throw std::runtime_error("Can't move. Would overflow buffer!");
        }

        return move_to(m_pos + offset, allocate);
    }

    /**
     * @brief Get raw pointer to the current position in the stream
     * @return
     */
    uint8_t *current() { return &m_data[m_pos]; }

    [[nodiscard]]
    const uint8_t *current() const {
        return &m_data[m_pos];
    }

  private:
    uint32_t m_pos;
    uint32_t m_size, m_alloc_size;

    uint8_t *m_data;

    // a read only bitstream cannot modify the underlying data
    // useful to avoid copying and delete
    bool m_read_only;
};

#ifndef IS_ENCLAVE
inline bitstream &bitstream::operator<<(std::ifstream &file) {
    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0);

    if (file_size <= 0) {
        throw std::runtime_error("invalid file");
    }

    resize(size() + file_size);
    file.read(reinterpret_cast<char *>(current()), file_size);

    return *this;
}
#endif

template <>
inline bitstream &bitstream::operator>> <bitstream>(bitstream &bstream) {
    if (bstream.size() > 0) {
        throw std::runtime_error("Target bstream already contains data");
    }

    uint32_t length = 0;
    *this >> length;

    if (m_pos + length > size()) {
        throw std::runtime_error("length is longer than bitstream");
    }

    if (length > 0) {
        bstream.write_raw_data(current(), length);
        move_by(static_cast<int32_t>(length));
    }

    bstream.move_to(0);
    return *this;
}

template <>
inline bitstream &bitstream::operator<< <bitstream>(const bitstream &other) {
    const uint32_t length = other.size();
    *this << length;

    if (length > 0) {
        write_raw_data(other.data(), length);
    }

    return *this;
}

template <> inline bitstream &bitstream::operator>> <char>(char &data) {
    if (at_end()) {
        throw std::runtime_error("Cannot read more: Already at end");
    }

    data = static_cast<char>(m_data[m_pos]);
    m_pos++;

    return *this;
}

inline bool operator==(const bitstream &first, const bitstream &second) {
    if (first.size() != second.size()) {
        return false;
    } else {
        return memcmp(first.data(), second.data(), first.size()) == 0;
    }
}

inline bool operator!=(const bitstream &first, const bitstream &second) {
    return !(first == second);
}
