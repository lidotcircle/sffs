#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <ios>
#include <memory>
#include <map>
#include <set>
#include <variant>
#include <vector>
#include <limits>
#include <type_traits>
#include <array>
#include <list>
#include <queue>
#include <functional>
#include <optional>
#include "./ldc_utils.h"
#include "./rbtree.h"


namespace ldc::SFFS {
using addr_t  = size_t;

class BaseException: public std::exception {};
class RuntimeError:  public BaseException {};
class OutOfRange:    public BaseException {};
class OutOfSpace:    public BaseException {};
class BadFormat:     public BaseException {};
class FileCorrupt:   public BaseException {};
class SectorTooHuge: public BaseException {};
class AlreadyExists: public BaseException {};

namespace Impl {
template <typename T>
struct device_traits {
    LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(T, const&, read,     size_t(addr_t,void*,size_t));
    LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(T, ,       write,    size_t(addr_t,const void*,size_t));
    LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(T, ,       flush,    void  ());
    LDC_CLASS_MEMBER_TEST_VALUE_AUTONAME(T, const&, maxsize, size_t());
};

template <typename T, bool complain = false>
struct is_block_device {
    using traits = device_traits<T>;

    static_assert(traits::has_read    || !complain, "read");
    static_assert(traits::has_write   || !complain, "write");
    static_assert(traits::has_maxsize || !complain, "maxsize");

    static constexpr bool value = traits::has_read && traits::has_write && traits::has_maxsize;
};
}

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class BlockView {
public:
    inline size_t read(addr_t addr, void* buf, size_t n) const {
        if (addr < m_offset || addr + n > m_offset + m_size) {
            throw OutOfRange();
        }

        return this->m_block.read(addr + m_offset, buf, n);
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        if (addr < m_offset || addr + n > m_offset + m_size) {
            throw OutOfRange();
        }

        return this->m_block.write(addr + m_offset, buf, n);
    }
    inline size_t maxsize() const {
        return this->m_size;
    }
    inline void flush() {
        if constexpr (Impl::device_traits<T>::has_flush) {
            this->m_block.flush();
        }
    }

    inline BlockView(T& block, addr_t offset, size_t size): m_block(block), m_size(size), m_offset(offset)
    {
        if (m_offset + size > m_block.maxsize()) {
            throw OutOfRange();
        }
    }

private:
    T& m_block;
    const size_t m_size;
    const addr_t m_offset;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class BlockDeviceExt {
private:
    T m_block;

    class ConstByteAccessor {
    protected:
        const BlockDeviceExt& m_wrapper;
        addr_t m_index;

    public:
        inline ConstByteAccessor(const BlockDeviceExt& wrapper, addr_t addr):
            m_wrapper(wrapper), m_index(addr) { }

        inline explicit operator char() const {
            return m_wrapper.get<char>(m_index);
        }
    };

    class ByteAccessor: public ConstByteAccessor {
    public:
        inline ByteAccessor(BlockDeviceExt& wrapper, addr_t addr):
            ConstByteAccessor(wrapper, addr) { }

        inline ByteAccessor& operator=(char byte) & {
            const_cast<BlockDeviceExt&>(this->m_wrapper).set<char>(this->m_index, byte);
            return *this;
        }

        inline ByteAccessor& operator=(char byte) && {
            const_cast<BlockDeviceExt&>(this->m_wrapper).set<char>(this->m_index, byte);
            return *this;
        }
    };


public:
    explicit inline BlockDeviceExt(T&& block): m_block(std::move(block)) {
    }

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        return this->m_block.read(addr, buf, n);
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        return this->m_block.write(addr, buf, n);
    }
    inline size_t maxsize() const {
        return this->m_block.maxsize();
    }
    inline void flush() {
        if constexpr (Impl::device_traits<T>::has_flush) {
            this->m_block.flush();
        }
    }

    // assume everything is little-endian FIXME
    template<typename Int, std::enable_if_t<std::is_integral<Int>::value,bool> = true>
    inline Int get(addr_t addr) const {
        Int ans;
        this->read(addr, &ans, sizeof(Int));
        return ans;
    }

    template<typename Int, std::enable_if_t<std::is_integral<Int>::value,bool> = true>
    inline void set(addr_t addr, Int val) {
        this->write(addr, &val, sizeof(Int));
    }

    inline ByteAccessor operator[](size_t idx) {
        return ByteAccessor(*this, idx);
    }

    inline ConstByteAccessor operator[](size_t idx) const {
        return ConstByteAccessor(*this, idx);
    }
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class BlockDeviceRefWrapper {
public:
    explicit inline BlockDeviceRefWrapper(T& block): m_block(&block) {}

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        return this->m_block->read(addr, buf, n);
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        return this->m_block->write(addr, buf, n);
    }
    inline size_t maxsize() const {
        return this->m_block->maxsize();
    }
    inline void flush() {
        if constexpr (Impl::device_traits<T>::has_flush) {
            this->m_block->flush();
        }
    }

private:
    T* m_block;
};

// TODO
class CacheBlock {
public:
    explicit CacheBlock(size_t size):
        m_cache(new char[size]), m_size(size)
    {
    }

    ~CacheBlock() {
        delete[] this->m_cache;
        this->m_cache = nullptr;
    }

    size_t read (addr_t addr, void* buf, size_t n) const;
    size_t write(addr_t addr, const void* buf, size_t n);
    size_t size() const;

private:
    char*  m_cache;
    size_t m_size;
};

// TODO
template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class CachedBlockDevice {
public:
    CachedBlockDevice(T block, size_t sec_size, size_t max_sec, std::vector<addr_t> pinned);

    inline size_t read (addr_t addr, void* buf, size_t n) const {
        return 0;
    }
    inline size_t write(addr_t addr, const void* buf, size_t n) {
        return 0;
    }
    inline void   flush() {
    }
    inline size_t maxsize() const {
        return m_internal.maxsize();
    }

private:
    T m_internal;
    std::set<addr_t>            m_dirty;
    std::list<addr_t>           m_lru;
    std::set<addr_t>            m_cached;
    std::set<addr_t>            m_pinned;
    std::map<addr_t,CacheBlock> m_caches;
};

enum class AllocTableEntry {
    NOT_USED      = -1,
    END_OF_CHAIN  = -2,
    SAT_USED      = -3,
    MSAT_USED     = -4,
    NotApplicable = -5,
    MaxRegSector  = -6,
};
inline bool is_reg_entry(uint32_t id) {
    return id != static_cast<uint32_t>(AllocTableEntry::NOT_USED) &&
           id != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) &&
           id != static_cast<uint32_t>(AllocTableEntry::SAT_USED) &&
           id != static_cast<uint32_t>(AllocTableEntry::MSAT_USED) &&
           id != static_cast<uint32_t>(AllocTableEntry::NotApplicable) &&
           id != static_cast<uint32_t>(AllocTableEntry::MaxRegSector);
}

#define CompoundFileSignature { 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1 }

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class CompoundFileHeaderAccessor {
public:
    inline explicit CompoundFileHeaderAccessor(T block_ref): m_block(std::move(block_ref))
    {
        if (m_block.maxsize() < 512) {
            throw BadFormat();
        }

        unsigned char signature_buf[8] = { 0 };
        const unsigned char cf_signature[8] = CompoundFileSignature;
        if (m_block.read(0, signature_buf, sizeof(signature_buf)) != sizeof(signature_buf)) {
            throw RuntimeError();
        }
        for (size_t i=0;i<sizeof(signature_buf);i++) {
            if (signature_buf[i] != cf_signature[i]) {
                throw BadFormat();
            }
        }

        if (this->version() != 3 && this->version() != 4) {
            throw BadFormat();
        }

        if (this->version() != 3 && this->version() != 4) {
            throw BadFormat();
        }

        if (!this->isLittleEndian()) {
            throw BadFormat();
        }

        const auto s  = m_block.template get<uint16_t>(30);
        if (s > 25) {
            throw SectorTooHuge();
        }
        m_sizeOfSector = 1 << s;

        const auto sm = m_block.template get<uint16_t>(30);
        if (sm > 25) {
            throw SectorTooHuge();
        }
        if (sm >= s) {
            throw BadFormat();
        }
        m_sizeOfShortSector = 1 << sm;
    }

    inline uint16_t version() const {
        return m_block.template get<uint16_t>(26);
    }

    inline uint16_t revision() const {
        return m_block.template get<uint16_t>(24);
    }

    inline bool isLittleEndian() const {
        return m_block.template get<uint8_t>(28) == 0xFE && m_block.template get<uint8_t>(29) == 0xFF;
    }

    inline size_t sizeOfSector() const {
        return m_sizeOfSector;
    }

    inline size_t sizeOfShortSector() const {
        return m_sizeOfShortSector;
    }

    inline uint32_t getNumsSectorforSAT() const {
        return m_block.template get<uint32_t>(44);
    }

    inline void setNumsSectorforSAT(uint32_t val) {
        m_block.template set<uint32_t>(44, val);
    }

    inline uint32_t getDirStreamSectorId() const {
        return m_block.template get<uint32_t>(48);
    }

    inline void setDirStreamSectorId(uint32_t val) {
        m_block.template set<uint32_t>(48, val);
    }

    inline uint32_t getMinSizeOfStandardStream() const {
        return m_block.template get<uint32_t>(56);
    }

    inline void setMinSizeOfStandardStream(uint32_t val) {
        m_block.template set<uint32_t>(56, val);
    }

    inline uint32_t getFirstSectorIdOfSSAT() const {
        return m_block.template get<uint32_t>(60);
    }

    inline void setFirstSectorIdOfSSAT(uint32_t val) {
        m_block.template set<uint32_t>(60, val);
    }

    inline uint32_t getNumsSectorforSSAT() const {
        return m_block.template get<uint32_t>(64);
    }

    inline void setNumsSectorforSSAT(uint32_t val) {
        m_block.template set<uint32_t>(64, val);
    }

    inline uint32_t getFirstSectorIdOfMSAT() const {
        return m_block.template get<uint32_t>(68);
    }

    inline void setFirstSectorIdOfMSAT(uint32_t val) {
        m_block.template set<uint32_t>(68, val);
    }

    inline uint32_t getNumsSectorForMSAT() const {
        return m_block.template get<uint32_t>(72);
    }

    inline void setNumsSectorForMSAT(uint32_t val) {
        m_block.template set<uint32_t>(72, val);
    }

    inline uint32_t getHeaderMSAT(uint32_t index) const {
        assert(index < 109);
        return m_block.template get<uint32_t>(76 + index * 4);
    }

    inline void setHeaderMSAT(uint32_t index, uint32_t val) {
        assert(index < 109);
        m_block.template set<uint32_t>(76 + index * 4, val);
    }

    inline constexpr size_t headerSize() const {
        return 512;
    }

    inline static CompoundFileHeaderAccessor format(T block, uint16_t majorVersion, uint16_t sectorShift, uint16_t shortSectorShift) {
        return CompoundFileHeaderAccessor(block, majorVersion, sectorShift, shortSectorShift);
    }

private:
    inline explicit CompoundFileHeaderAccessor(T block, uint16_t majorVersion, uint16_t sectorShift, uint16_t shortSectorShift): m_block(std::move(block))
    {
        if (m_block.maxsize() < 512) {
            throw BadFormat();
        }

        const char buf[512] = { 0 };
        if (m_block.write(0, buf, sizeof(buf)) != sizeof(buf)) {
            throw RuntimeError();
        }

        const unsigned char signature_buf[8] = CompoundFileSignature;
        if (m_block.write(0, signature_buf, sizeof(signature_buf)) != sizeof(signature_buf)) {
            throw RuntimeError();
        }

        if (sectorShift < shortSectorShift) {
            throw BadFormat();
        }

        if (sectorShift > 25 || shortSectorShift > 25) {
            throw BadFormat();
        }

        m_block.template set<uint16_t>(24, 0x3E);
        m_block.template set<uint16_t>(26, majorVersion);
        m_block.template set<uint8_t>(28, 0xFE);
        m_block.template set<uint8_t>(29, 0xFF);
        m_block.template set<uint16_t>(30, sectorShift);
        m_block.template set<uint16_t>(32, shortSectorShift);
        m_sizeOfSector = 1 << sectorShift;
        m_sizeOfShortSector = 1 << shortSectorShift;

        this->setDirStreamSectorId  (static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        this->setFirstSectorIdOfSSAT(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        this->setFirstSectorIdOfMSAT(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        this->setMinSizeOfStandardStream(4096);

        for (size_t i=0;i<109;i++) {
            this->setHeaderMSAT(i, static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        }
    }

    BlockDeviceExt<T> m_block;
    size_t m_sizeOfSector, m_sizeOfShortSector;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class MasterSectorAllocationTable {
public:
    inline MasterSectorAllocationTable(CompoundFileHeaderAccessor<T>& header, T block_ref):
        m_header(header), m_block(std::move(block_ref)),
        m_caches(header.getNumsSectorForMSAT() * this->entriesPerBlock() + 109, static_cast<uint32_t>(AllocTableEntry::NOT_USED)),
        m_usedsize(0)
    {
        for (size_t i=0;i<109;i++) {
            this->m_caches[i] = header.getHeaderMSAT(i);
        }

        const size_t numsOfMSATSec = header.getNumsSectorForMSAT();
        if (numsOfMSATSec > 0) {
            auto secId = header.getFirstSectorIdOfMSAT();

            size_t count = 0;
            for (;secId != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);count++) {
                if (!is_reg_entry(secId)) {
                    throw FileCorrupt();
                }
                m_lastSecId = secId;

                for (size_t i=0;i<this->entriesPerBlock();i++) {
                    auto val = m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + i * sizeof(uint32_t));
                    this->m_caches.push_back(val);
                }

                secId = m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + this->entriesPerBlock() * sizeof(uint32_t));
            }

            if (count != numsOfMSATSec) {
                throw FileCorrupt();
            }
        }

        for (size_t i=m_caches.size();i>0;i--) {
            if (m_caches[i-1] != static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                m_usedsize = i;
            }
        }
    }

    /**
     * mark MSAT entry specified by index as sector Id
     *
     * @param index for specifying MSAT entry, it should be a NOT_USED entry
     * @param secId SAT sector id
     */
    inline std::optional<size_t> MarkSATBlock(uint32_t secId) {
        std::optional<size_t> index_opt;
        if (m_usedsize >= m_caches.size()) return std::nullopt;
        
        auto index = m_usedsize;
        this->m_caches[index] = secId;
        m_usedsize++;
        if (index < 109) {
            this->m_header.setHeaderMSAT(index, secId);
        } else {
            index -= 109;
            auto secId = this->m_header.getFirstSectorIdOfMSAT();
            if (!is_reg_entry(secId)) {
                throw FileCorrupt();
            }

            while (index >= this->entriesPerBlock()) {
                secId = m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + this->entriesPerBlock() * sizeof(uint32_t));
                index -= this->entriesPerBlock();

                if (!is_reg_entry(secId)) {
                    throw FileCorrupt();
                }
            }

            m_block.template set<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * secId + index * sizeof(uint32_t), secId);
        }

        return index;
    }

    inline bool hasFreeEntry() const { return m_caches.size() > m_usedsize; }

    /**
     * expanding MSAT by add sector secId to the chain
     *
     * @param secId sector of last MSAT sector
     */
    inline void expand(uint32_t secId) {
        if (m_lastSecId.has_value()) {
            m_block.template set<uint32_t>(
                    m_header.headerSize() + m_header.sizeOfSector() * m_lastSecId.value() + this->entriesPerBlock() * sizeof(uint32_t),
                    secId);
        } else {
            m_header.setFirstSectorIdOfMSAT(secId);
        }

        for (size_t i=0;i<this->entriesPerBlock();i++) {
            m_block.template set<uint32_t>(
                    m_header.headerSize() + m_header.sizeOfSector() * secId + i * sizeof(uint32_t),
                    static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        }
        m_block.template set<uint32_t>(
                m_header.headerSize() + m_header.sizeOfSector() * secId + this->entriesPerBlock() * sizeof(uint32_t),
                static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));

        m_lastSecId = secId;
        for (size_t i=0;i<this->entriesPerBlock();i++) m_caches.push_back(static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        m_header.setNumsSectorForMSAT(m_header.getNumsSectorForMSAT() + 1);
    }

    inline uint32_t get(size_t idx) const {
        assert(idx < this->size());
        return m_caches[idx];
    }

    inline size_t size() const { return m_usedsize; }

private:
    inline uint32_t entriesPerBlock() const {
        return m_header.sizeOfSector() / sizeof(uint32_t) - 1;
    }

    CompoundFileHeaderAccessor<T>& m_header;
    BlockDeviceExt<T>              m_block;
    uint32_t                       m_usedsize;
    std::vector<uint32_t>          m_caches;
    std::optional<uint32_t>        m_lastSecId;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class SectorAllocationTable {
public:
    inline SectorAllocationTable(CompoundFileHeaderAccessor<T>& header, T block_ref):
        m_block(std::move(block_ref)),
        m_header(header),
        m_msat(header, block_ref),
        m_lru_sectorIdBase(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)),
        m_sat_free_count(m_msat.size(), 0)
    {
        size_t kspa = 0;
        for (size_t i=0;i<m_sat_free_count.size();i++) {
            std::vector<uint32_t> cache;
            size_t nfree = 0;
            this->readNthSATSecIntoVec(i, cache, nfree);
            m_sat_free_count[i] = nfree;
            if (nfree > kspa) {
                m_lru_sectorCaches = std::move(cache);
                m_lru_sectorIdBase = i * this->entriesPerBlock();
                kspa = nfree;
            }
        }
    }

    inline uint32_t allocateNextSector(uint32_t preSecId) {
        if (preSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)) {
            return this->allocateSector();
        }
        const auto secId = this->allocateSector();
        this->setEntry(preSecId, secId);
        return secId;
    }

    inline void freeInterSector(uint32_t preSecId, uint32_t secId) {
        if (preSecId == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
            const auto next = this->getEntry(secId);
            assert(next == static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        } else {
            assert(is_reg_entry(secId));
            assert(this->getEntry(preSecId) == secId);
            const auto next = this->getEntry(secId);
            assert(is_reg_entry(next) || next == static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            this->setEntry(preSecId, next);
        }
        this->setEntry(secId, static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        const auto idx = secId / this->entriesPerBlock();
        assert(idx < m_sat_free_count.size());
        m_sat_free_count[idx]++;
    }

    inline std::optional<uint32_t> getNextSector(uint32_t secId) const {
        auto ans = this->getEntry(secId);

        if (!is_reg_entry(ans)) {
            assert(ans == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
            return std::nullopt;
        }

        return ans;
    }

    inline size_t size() const {
        return m_header.getNumsSectorforSAT() * this->entriesPerBlock();
    }

private:
    inline uint32_t entriesPerBlock() const {
        return m_header.sizeOfSector() / sizeof(uint32_t);
    }

    inline uint32_t getEntry(uint32_t secId) const {
        assert(secId < m_msat.size() * this->entriesPerBlock());
        const auto tbl_idx = secId / this->entriesPerBlock();
        const auto tbl_sec = m_msat.get(tbl_idx);
        const auto sec_idx = secId % this->entriesPerBlock();

        if (!m_lru_sectorCaches.empty() && m_lru_sectorIdBase == tbl_idx * this->entriesPerBlock()) {
            assert(m_lru_sectorCaches.size() > sec_idx);
            return m_lru_sectorCaches[sec_idx];
        }

        return m_block.template get<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * tbl_sec + sec_idx * sizeof(uint32_t));
    }

    inline void setEntry(uint32_t secId, uint32_t val) {
        assert(secId < m_msat.size() * this->entriesPerBlock());
        const auto tbl_idx = secId / this->entriesPerBlock();
        const auto tbl_sec = m_msat.get(tbl_idx);
        const auto sec_idx = secId % this->entriesPerBlock();

        if (!m_lru_sectorCaches.empty() && m_lru_sectorIdBase == tbl_idx * this->entriesPerBlock()) {
            assert(m_lru_sectorCaches.size() > sec_idx);
            m_lru_sectorCaches[sec_idx] = val;
        }

        m_block.template set<uint32_t>(m_header.headerSize() + m_header.sizeOfSector() * tbl_sec + sec_idx * sizeof(uint32_t), val);
    }

    /*
     * used for caching SAT subtable associated with nth MSAT entry,
     * which also report the number of free SAT entry
     *
     * @param nth index of MSAT, this entry should be allocated,
     *            otherwise OutOfRange() will be throwed
     * @param cache for filling SAT page
     * @param nfree number of NOT_USED entry in cached SAT page
     * @return index of first NOT_USED entry in SAT page if it existed otherwise std::nullopt
     */
    inline std::optional<size_t> readNthSATSecIntoVec(size_t nth, std::vector<uint32_t>& cache, size_t& nfree)
    {
        assert(cache.empty());
        assert(nfree == 0);

        const auto sec = m_msat.get(nth);
        if (static_cast<AllocTableEntry>(sec) == AllocTableEntry::NOT_USED)
            return std::nullopt;

        uint32_t buf[4096] = { 0 };
        const size_t n = this->entriesPerBlock() * sizeof(uint32_t);
        std::optional<size_t> u = std::nullopt;
        size_t offset = 0;
        while (n > offset) {
            const auto k = std::min(n - offset, sizeof(buf));
            m_block.read(m_header.headerSize() + m_header.sizeOfSector() * sec + offset, buf, k);
            offset += k;

            for (size_t j=0;j<(k/sizeof(uint32_t));j++) {
                if (buf[j] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                    nfree++;
                    u = cache.size();
                }
                cache.push_back(buf[j]);
            }
        }
        return u;
    }

    inline uint32_t allocateSector() {
        for (size_t i=0;i<m_lru_sectorCaches.size();i++) {
            if (m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                const auto ans = m_lru_sectorIdBase + i;
                this->setEntry(ans, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                assert(m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));

                this->check_limit(ans);
                const auto idx = ans / this->entriesPerBlock();
                assert(m_sat_free_count.size() > idx);
                assert(m_sat_free_count[idx] > 0);
                m_sat_free_count[idx]--;
                if (m_sat_free_count[idx] == 0) {
                    m_lru_sectorCaches.clear();
                }
                return ans;
            }
        }

        for (size_t _i=m_sat_free_count.size();_i>0;_i--) {
            const auto i = _i - 1;
            if (m_sat_free_count[i] > 0) {
                this->m_lru_sectorCaches.clear();

                size_t nfree = 0;
                const auto u_opt = this->readNthSATSecIntoVec(i, m_lru_sectorCaches, nfree);
                assert (u_opt.has_value());
                const auto u = u_opt.value();

                m_lru_sectorIdBase = i * this->entriesPerBlock();;
                const auto ans = m_lru_sectorIdBase + u;
                this->setEntry(ans, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                assert(m_lru_sectorCaches[u] == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));

                m_sat_free_count[i]--;
                if (m_sat_free_count[i] == 0) {
                    m_lru_sectorCaches.clear();
                }

                this->check_limit(ans);
                return ans;
            }
        }

        const auto initSATSector = [&](uint32_t secId) {
            for (uint32_t i=0;i<this->entriesPerBlock();i++) {
                this->setEntry(secId+i, static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            }
        };

        // allocate new SAT sector
        if (m_msat.hasFreeEntry()) {
            const auto satSecId = m_sat_free_count.size() * this->entriesPerBlock();
            this->check_limit(satSecId);
            const auto idx = m_msat.MarkSATBlock(satSecId);
            assert(idx.has_value());
            m_lru_sectorCaches = std::vector<uint32_t>(this->entriesPerBlock(), static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            m_lru_sectorIdBase = satSecId;
            initSATSector(satSecId);
            this->setEntry(satSecId, static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            assert(m_lru_sectorCaches[0] == static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            this->m_sat_free_count.push_back(this->entriesPerBlock() - 1);
        } else {
            const auto msatSecId = m_sat_free_count.size() * this->entriesPerBlock();
            this->check_limit(msatSecId + 1);
            m_msat.expand(msatSecId);
            const auto satSecId = msatSecId + 1;

            const auto idx = m_msat.MarkSATBlock(satSecId);
            assert(idx.has_value());
            m_lru_sectorCaches = std::vector<uint32_t>(this->entriesPerBlock(), static_cast<uint32_t>(AllocTableEntry::NOT_USED));
            m_lru_sectorIdBase = satSecId;
            initSATSector(satSecId);
            this->setEntry(msatSecId,     static_cast<uint32_t>(AllocTableEntry::MSAT_USED));
            this->setEntry(msatSecId + 1, static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            assert(m_lru_sectorCaches[0] == static_cast<uint32_t>(AllocTableEntry::MSAT_USED));
            assert(m_lru_sectorCaches[1] == static_cast<uint32_t>(AllocTableEntry::SAT_USED));
            this->m_sat_free_count.push_back(this->entriesPerBlock() - 2);
        }
        m_header.setNumsSectorforSAT(m_sat_free_count.size());

        for (size_t i=0;i<m_lru_sectorCaches.size();i++) {
            if (m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                const auto ans = m_lru_sectorIdBase + i;
                this->setEntry(ans, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                assert(m_lru_sectorCaches[i] == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
               
                this->check_limit(ans);

                const auto idx = ans / this->entriesPerBlock();
                assert(m_sat_free_count.size() > idx);
                assert(m_sat_free_count[idx] > 1);
                return ans;
            }
        }

        throw RuntimeError();
    }

    inline void check_limit(uint32_t secId) {
        if (m_header.sizeOfSector() * (secId + 1) + m_header.sizeOfSector() > m_block.maxsize()) {
            throw OutOfSpace();
        }
    }

    BlockDeviceExt<T>               m_block;
    CompoundFileHeaderAccessor<T>&  m_header;
    MasterSectorAllocationTable<T>  m_msat;

    // keep least-recently-used allocated/free sector
    uint32_t                        m_lru_sectorIdBase;
    std::vector<uint32_t>           m_lru_sectorCaches;
    std::vector<size_t>             m_sat_free_count;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class SectorChainStream {
public:
    inline SectorChainStream(CompoundFileHeaderAccessor<T>& header, SectorAllocationTable<T>& sat, T block_ref, uint32_t headSecId):
        m_header(header),
        m_sat(sat),
        m_block(std::move(block_ref)),
        m_headSecId(headSecId)
    {
        assert(is_reg_entry(m_headSecId) || m_headSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
    }

    inline uint32_t getHeadSectorID() const { return m_headSecId; }

    inline void deleteStream() {
        if (m_headSecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)) {
            return;
        }

        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        m_secIdChainCache.pop_back();
        for (auto secId: m_secIdChainCache) {
            assert(is_reg_entry(secId));
            m_sat.freeInterSector(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN), secId);
        }
        m_secIdChainCache.clear();
        this->m_headSecId = static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
    }

    inline void deleteLastSector() {
        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        assert(this->m_secIdChainCache.size() > 1);

        const auto prev = m_secIdChainCache.size() > 2 
                             ? m_secIdChainCache[m_secIdChainCache.size() - 3] 
                             : static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
        m_sat.freeInterSector(prev, m_secIdChainCache[m_secIdChainCache.size() - 2]);
        m_secIdChainCache.erase(m_secIdChainCache.end() - 2);
        if (m_secIdChainCache.size() == 1) {
            this->m_headSecId = static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
        }
    }

    inline addr_t appendSector() {
        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        if (m_secIdChainCache.size() > 1) {
            m_secIdChainCache.pop_back();
            const auto id = this->m_sat.allocateNextSector(m_secIdChainCache.back());
            m_secIdChainCache.push_back(id);
            m_secIdChainCache.push_back(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        } else {
            const auto id = this->m_sat.allocateNextSector(m_secIdChainCache.back());
            m_secIdChainCache.insert(m_secIdChainCache.begin(), id);
            m_headSecId = id;
        }

        return static_cast<addr_t>(this->size() - this->sectorSize());
    }

    inline size_t size() const {
        return this->prepareCacheUntil(std::numeric_limits<size_t>::max());
    }

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        const auto tlt = prepareCacheUntil(static_cast<size_t>(n + addr));
        if (tlt < static_cast<size_t>(n + addr))
            throw OutOfRange();

        const auto ss = this->sectorSize();
        int nread = 0;
        while (nread < n) {
            const auto secIdx = (addr + nread) / ss;
            const auto offset = (addr + nread) % ss;
            const auto k = std::min(n - nread, ss - offset);
            assert(m_secIdChainCache.size() > secIdx);
            const auto secId = m_secIdChainCache[secIdx];
            this->m_block.read(m_header.headerSize() + secId * ss + offset, (static_cast<char*>(buf) + nread), k);
            nread += k;
        }
        return nread;
    }

    inline size_t write(addr_t addr, const void* buf, size_t n) {
        const auto tlt = prepareCacheUntil(static_cast<size_t>(n + addr));
        if (tlt < static_cast<size_t>(n + addr)) {
            while (this->size() < static_cast<size_t>(n + addr)) {
                this->appendSector();
            }
        }

        const auto ss = this->sectorSize();
        int nwrited = 0;
        while (nwrited < n) {
            const auto secIdx = (addr + nwrited) / ss;
            const auto offset = (addr + nwrited) % ss;
            const auto k = std::min(n - nwrited, ss - offset);
            assert(m_secIdChainCache.size() > secIdx);
            const auto secId = m_secIdChainCache[secIdx];
            this->m_block.write(m_header.headerSize() + secId * ss + offset, (static_cast<const char*>(buf) + nwrited), k);
            nwrited += k;
        }
        return nwrited;
    }

    inline void fillzeros(addr_t begin, addr_t end) {
        const char buf[4096] = {0};
        while (begin < end) {
            const size_t s = std::min(sizeof(buf), end - begin);
            this->write(begin, buf, s);
            begin += s;
        }
    }
 
    inline size_t maxsize() const {
        return m_block.maxsize();
    }

    SectorChainStream(const SectorChainStream&) = delete;
    SectorChainStream& operator=(const SectorChainStream&) = delete;

    SectorChainStream(SectorChainStream&& oth):
        m_header(oth.m_header), m_sat(oth.m_sat), m_block(std::move(oth.m_block)),
        m_headSecId(oth.m_headSecId), m_secIdChainCache(std::move(oth.m_secIdChainCache))
    {
    }

    SectorChainStream& operator=(SectorChainStream&& oth) {
        assert(&this->m_header == &oth.m_header);
        assert(&this->m_sat    == &oth.m_sat);
        this->m_block = std::move(oth.m_block);
        this->m_headSecId = std::move(oth.m_headSecId);
        this->m_secIdChainCache = std::move(oth.m_secIdChainCache);
        return *this;
    }

private:
    inline size_t sectorSize() const { return m_header.sizeOfSector(); }

    inline size_t prepareCacheUntil(size_t limit) const {
        if (m_secIdChainCache.empty()) {
            m_secIdChainCache.push_back(m_headSecId);
        }

        while (m_secIdChainCache.back() != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) &&
               m_secIdChainCache.size() * this->sectorSize() < limit)
        {
            auto next = m_sat.getNextSector(m_secIdChainCache.back());
            if (next.has_value()) {
                m_secIdChainCache.push_back(next.value());
            } else {
                m_secIdChainCache.push_back(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
            }
        }

        const auto n = m_secIdChainCache.back() == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) ? 
                           m_secIdChainCache.size() - 1: m_secIdChainCache.size();
        return n * this->sectorSize();
    }

    CompoundFileHeaderAccessor<T>& m_header;
    SectorAllocationTable<T>& m_sat;
    BlockDeviceExt<T> m_block;
    uint32_t m_headSecId;
    mutable std::vector<uint32_t> m_secIdChainCache;
};

enum class EntryType {
    Empty = 0,
    UserStorage = 1,
    UserStream = 2,
    LockBytes = 3,
    Property = 4,
    RootStorage = 5,
};

#define COMPOUND_FILE_ENTRY_SIZE 128
template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class DirectoryTable {
public:
    inline DirectoryTable(CompoundFileHeaderAccessor<T>& header, SectorAllocationTable<T>& sat, T block_ref):
        m_header(header),
        m_stream(header, sat, std::move(block_ref), header.getDirStreamSectorId()),
        m_algo(rbtreeops(*this))
    {
        if (m_stream.size() == 0) {
            m_stream.appendSector();
            m_header.setDirStreamSectorId(m_stream.getHeadSectorID());
            m_free_entries = m_stream.size() / COMPOUND_FILE_ENTRY_SIZE - 1;
            const std::array<uint16_t, 32> RootEntry = { 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x45, 0x6E, 0x74, 0x72, 0x79, 0x00 };
            this->fillzeros(0);
            this->setName             (0, RootEntry);
            this->setEntryType        (0, EntryType::RootStorage);
            this->setBlack            (0, true);
            this->setLeftChild        (0, this->getNullNode());
            this->setRightChild       (0, this->getNullNode());
            this->setSubdirectoryEntry(0, this->getNullNode());
            this->setStreamUID        (0, { 0 });
            this->setUserFlags        (0, 0);
            this->setCreatedTimestamp (0, { 0 });
            this->setModifiedTimestamp(0, { 0 });
            this->setSectorID         (0, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
            this->setSize             (0, 0);
        }

        m_free_entries = m_stream.size() / COMPOUND_FILE_ENTRY_SIZE;
        m_usedentries.resize(m_free_entries, false);

        std::list<uint32_t> qnq = { 0 };
        std::set<uint32_t> seen = { 0 };
        const auto append = [&] (uint32_t entry) {
            if (entry == this->getNullNode()) return;

            if (seen.find(entry) != seen.end()) {
                throw FileCorrupt();
            }
            seen.insert(entry);
        };

        while (!qnq.empty()) {
            const auto entry = qnq.front();
            qnq.pop_front();

            assert(m_free_entries > 0);
            assert(m_usedentries.size() > entry);
            m_free_entries--;
            m_usedentries[entry] = true;

            append(this->getLeftChild(entry));
            append(this->getRightChild(entry));

            const auto type = this->getEntryType(entry);
            if (type == EntryType::RootStorage || type == EntryType::UserStorage) {
                append(this->getSubdirectoryEntry(entry));
            }
        }
    }

    inline std::optional<uint32_t> createEntry(uint32_t parentDirEntryId, const std::array<uint16_t,32>& name, EntryType type) {
        if (m_free_entries == 0) {
            const auto addr = m_stream.appendSector();
            m_stream.fillzeros(addr, static_cast<addr_t>(m_stream.size()));
            const auto n = m_header.sizeOfSector() / COMPOUND_FILE_ENTRY_SIZE;
            for (size_t i=0;i<n;i++) m_usedentries.push_back(false);
            m_free_entries += n;
        }

        const auto ptype = this->getEntryType(parentDirEntryId);
        assert(ptype == EntryType::RootStorage || ptype == EntryType::UserStorage);

        size_t id = 0;
        for (;id<m_usedentries.size() && m_usedentries[id];id++);

        this->setName             (id, name);
        this->setEntryType        (id, type);
        this->setBlack            (id, false);
        this->setLeftChild        (id, this->getNullNode());
        this->setRightChild       (id, this->getNullNode());
        this->setSubdirectoryEntry(id, this->getNullNode());
        this->setStreamUID        (id, { 0 });
        this->setUserFlags        (id, 0);
        this->setCreatedTimestamp (id, { 0 });
        this->setModifiedTimestamp(id, { 0 });
        this->setSectorID         (id, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        this->setSize             (id, 0);
        m_usedentries[id] = true;
        m_free_entries--;

        return this->insertEntry(parentDirEntryId, id);
    }

    inline std::optional<uint32_t> insertEntry(uint32_t parentDirEntryId, uint32_t childEntryId) {
        if (!insertIntoDirectory(parentDirEntryId, childEntryId)) {
            m_free_entries++;
            m_usedentries[childEntryId] = false;
            return std::nullopt;
        }

        return childEntryId;
    }

    inline std::optional<uint32_t> deleteEntry(uint32_t parentDirEntryId, uint32_t deleteId)
    {
        assert(deleteId > 0);
        auto name = this->getName(deleteId);
        auto root = this->getSubdirectoryEntry(parentDirEntryId);
        auto node = m_algo.findNode(root, name);
        if (m_algo.exists(node)) {
            const auto ans = m_algo.getNode(node);
            const auto oldroot = root;
            m_algo.deleteNode(root, node);
            m_free_entries++;
            m_usedentries[deleteId] = false;
            if (root != oldroot)
                this->setSubdirectoryEntry(parentDirEntryId, root);
            return ans;
        }
        return std::nullopt;
    }

    inline std::optional<uint32_t> searchChild(uint32_t parentId, const std::array<uint16_t,32>& childname) const {
        const auto etype = this->getEntryType(parentId);
        if (etype != EntryType::RootStorage &&
            etype != EntryType::UserStorage)
        {
            return std::nullopt;
        }

        const auto root = this->getSubdirectoryEntry(parentId);
        auto child = m_algo.findNode(root, childname);
        if (!m_algo.exists(child)) return std::nullopt;

        return m_algo.getNode(child);
    }

    inline uint32_t getShortStreamSecId() const { return this->getSectorID(0); }
    inline void setShortStreamSecId(uint32_t val) { return this->setSectorID(0, val); }

    inline uint32_t getShortStreamSize() const { return this->getSize(0); }
    inline void setShortStreamSize(uint32_t size) { return this->setSize(0, size); }

    inline void fillzeros(uint32_t entryid) {
        const char buf[COMPOUND_FILE_ENTRY_SIZE] = { 0 };
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE, buf, COMPOUND_FILE_ENTRY_SIZE);
    }

    inline std::array<uint16_t,32> getName(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<uint16_t,32> ans;
        const auto n = m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setName(uint32_t entryid, const std::array<uint16_t,32>& name) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE, name.data(), name.size());
        uint16_t len = 0;
        for (;len < 32 && name[len]!=0; len++);
        len = (len + 1) * 2;
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 64, &len, sizeof(len));
    }

    inline uint16_t getNameBufferLength(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint16_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 64, &ans, sizeof(ans));
        return ans;
    }

    inline EntryType getEntryType(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint8_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 66, &ans, sizeof(ans));
        return static_cast<EntryType>(ans);
    }

    inline void setEntryType(uint32_t entryid, EntryType type) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint8_t t = static_cast<uint8_t>(type);
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 66, &t, sizeof(t));
    }

    inline bool isBlack(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        bool ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 67, &ans, sizeof(ans));
        return ans;
    }

    inline void setBlack(uint32_t entryid, bool isBlack) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 67, &isBlack, sizeof(isBlack));
    }

    inline uint32_t getLeftChild(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 68, &ans, sizeof(ans));
        return ans;
    }

    inline void setLeftChild(uint32_t entryid, uint32_t childId) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 68, &childId, sizeof(childId));
    }

    inline uint32_t getRightChild(uint32_t entryid) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 72, &ans, sizeof(ans));
        return ans;
    }

    inline void setRightChild(uint32_t entryid, uint32_t childId) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 72, &childId, sizeof(childId));
    }

    inline uint32_t getSubdirectoryEntry(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 76, &ans, sizeof(ans));
        return ans;
    }

    inline void setSubdirectoryEntry(uint32_t entryid, uint32_t dirid) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 76, &dirid, sizeof(dirid));
    }

    inline std::array<char,16> getStreamUID(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<char,16> ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 80, ans.data(), ans.size());
        return ans;
    }

    inline void setStreamUID(uint32_t entryid, const std::array<char,16>& uid) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 80, uid.data(), uid.size());
    }

    inline uint32_t getUserFlags(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 96, &ans, sizeof(ans));
        return ans;
    }

    inline void setUserFlags(uint32_t entryid, uint32_t flags) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 96, &flags, sizeof(flags));
    }

    inline std::array<char,8> getCreatedTimestamp(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<char,8> ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 100, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setCreatedTimestamp(uint32_t entryid, const std::array<char,8>& tstamp) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 100, tstamp.data(), sizeof(tstamp));
    }

    inline std::array<char,8> getModifiedTimestamp(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        std::array<char,8> ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 108, ans.data(), sizeof(ans));
        return ans;
    }

    inline void setModifiedTimestamp(uint32_t entryid, const std::array<char,8>& tstamp) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 108, tstamp.data(), sizeof(tstamp));
    }

    inline uint32_t getSectorID(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 116, &ans, sizeof(ans));
        return ans;
    }

    inline void setSectorID(uint32_t entryid, uint32_t secId) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 116, &secId, sizeof(secId));
    }

    inline uint32_t getSize(uint32_t entryid) const {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        uint32_t ans;
        m_stream.read(entryid * COMPOUND_FILE_ENTRY_SIZE + 120, &ans, sizeof(ans));
        return ans;
    }

    inline void setSize(uint32_t entryid, uint32_t size) {
        assert(entryid * COMPOUND_FILE_ENTRY_SIZE < m_stream.size());
        m_stream.write(entryid * COMPOUND_FILE_ENTRY_SIZE + 120, &size, sizeof(size));
    }

private:
    struct rbtreeops {
        using _Node = uint32_t;
        using _Key = std::array<uint16_t,32>;

        explicit rbtreeops(DirectoryTable& table): m_table(table) { }

        _Node getLeft (_Node n) const { return m_table.getLeftChild(n); }
        _Node getRight(_Node n) const { return m_table.getRightChild(n); }

        void setLeft (_Node n, _Node l) const { m_table.setLeftChild(n, l); }
        void setRight(_Node n, _Node r) const { m_table.setRightChild(n, r); }

        bool isBlack(_Node n) const { return m_table.isBlack(n); }
        void setBlack(_Node n, bool black) { m_table.setBlack(n, black); }

        inline bool isNullNode(_Node node) const { return m_table.isNullNode(node); }
        inline _Node getNullNode() const { return m_table.getNullNode(); }

        inline _Key getKey(_Node& n) const { return m_table.getName(n); }

        inline bool keyCompareLess(const _Key& lhs, const _Key& rhs) const {
            size_t l1 = 0, l2 = 0;
            for (;l1<lhs.size()&&lhs[l1]!=0;l1++);
            for (;l2<rhs.size()&&rhs[l2]!=0;l2++);

            if (l1 < l2) return true;
            if (l1 > l2) return false;

            for (size_t i=0;i<l1;i++) {
                if (lhs[i] < rhs[i]) {
                    return true;
                } else if (lhs[i] > rhs[i]) {
                    return false;
                }
            }

            return false;
        }

        inline bool nodeCompareEqual(const _Node& n1, const _Node& n2) const { return n1 == n2; }

    private:
        DirectoryTable& m_table;
    };
    using RBTreeAlgorithm = ldc::RBTreeAlgorithmImpl::RBTreeAlgorithm<rbtreeops,uint32_t,std::array<uint16_t,32>,false,false>;

    inline uint32_t getNullNode() const { return static_cast<uint32_t>(-1); }
    inline bool     isNullNode(uint32_t n) const { return n == this->getNullNode(); }

    inline bool insertIntoDirectory(uint32_t parentDir, uint32_t newentry) {
        this->setBlack(newentry, false);
        auto root = this->getSubdirectoryEntry(parentDir);
        const auto oldroot = root;
        auto insertPt = m_algo.insertNode(root, newentry);
        if (m_algo.exists(insertPt)) {
            if (root != oldroot) {
                this->setSubdirectoryEntry(parentDir, root);
            }
            return true;
        } else {
            return false;
        }
    }

    RBTreeAlgorithm                m_algo;
    CompoundFileHeaderAccessor<T>& m_header;
    SectorChainStream<T>           m_stream;
    std::vector<bool>              m_usedentries;
    size_t                         m_free_entries;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class ShortSectorStream {
public:
    using stream_t = SectorChainStream<T>;

    inline ShortSectorStream(CompoundFileHeaderAccessor<T>& header, DirectoryTable<T>& dirtable, SectorAllocationTable<T>& sat, T block_ref):
        m_header(header), m_dirtable(dirtable), m_stream(header, sat, std::move(block_ref),
        m_dirtable.getShortStreamSecId()), m_size(m_dirtable.getShortStreamSize())
    {
    }

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        if (addr + n > m_size)
            throw OutOfRange();

        return m_stream.read(addr, buf, n);
    }

    inline size_t write(addr_t addr, const void* buf, size_t n) {
        if (n == 0) return 0;

        const auto s1 = m_stream.size();
        const auto ans = m_stream.write(addr, buf, n);
        if (s1 == 0) m_dirtable.setShortStreamSecId(m_stream.getHeadSectorID());

        if (addr + n > m_size) {
            m_size = addr + n;
            m_dirtable.setShortStreamSize(m_size);
        }

        return ans;
    }

    inline size_t maxsize() const { return m_stream.maxsize(); }

private:
    CompoundFileHeaderAccessor<T>& m_header;
    DirectoryTable<T>&             m_dirtable;
    stream_t                       m_stream;
    uint32_t                       m_size;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class ShortSectorAllocationTable {
public:
    using stream_t = SectorChainStream<T>;

    inline ShortSectorAllocationTable(CompoundFileHeaderAccessor<T>& header, SectorAllocationTable<T>& sat, T block_ref):
        m_header(header),
        m_stream(header, sat, std::move(block_ref), header.getFirstSectorIdOfSSAT())
    {
    }

    inline uint32_t get(uint32_t idx) const {
        uint32_t _val;
        m_stream.read(idx * sizeof(uint32_t), &_val, sizeof(uint32_t));
        return _val;
    }

    inline void set(uint32_t idx, AllocTableEntry val) { return this->set(idx, static_cast<uint32_t>(val)); }

    inline void set(uint32_t idx, uint32_t val) {
        const auto s1 = m_stream.size();

        m_stream.write(idx * sizeof(uint32_t), &val, sizeof(uint32_t));

        if (s1 == 0)
            m_header.setFirstSectorIdOfSSAT(m_stream.getHeadSectorID());

        const auto s2 = m_stream.size();
        if (s1 < s2)
            m_header.setNumsSectorforSSAT(s2 / m_header.sizeOfShortSector());
    }

    // FIXME brute-force search
    inline uint32_t alloc() {
        const auto end = m_stream.size()/sizeof(uint32_t);
        for (uint32_t i=0;i<end;i++) {
            if (this->get(i) == static_cast<uint32_t>(AllocTableEntry::NOT_USED)) {
                return i;
            }
        }

        m_stream.appendSector();
        const auto newend = m_stream.size()/sizeof(uint32_t);
        assert(newend > end);
        for (uint32_t i=end;i<newend;i++)
            this->set(i, static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        return end;
    }

private:
    CompoundFileHeaderAccessor<T>& m_header;
    stream_t                       m_stream;
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class ShortSectorChainStream {
public:
    inline ShortSectorChainStream(CompoundFileHeaderAccessor<T>& header, ShortSectorAllocationTable<T>& ssat, ShortSectorStream<T>& sss, uint32_t headSSecId):
        m_header(header), m_ssat(ssat), m_block(BlockDeviceRefWrapper<ShortSectorStream<T>>(sss)), m_headssecId(headSSecId)
    {
       assert(is_reg_entry(m_headssecId) || m_headssecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
    }

    inline uint32_t getHeadShortSectorID() const { return m_headssecId; }

    inline void deleteStream() {
        if (m_headssecId == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN)) {
            return;
        }

        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        m_ssecIdChainCache.pop_back();
        for (auto ssecId: m_ssecIdChainCache) {
            assert(is_reg_entry(ssecId));
            m_ssat.set(ssecId, static_cast<uint32_t>(AllocTableEntry::NOT_USED));
        }
        m_ssecIdChainCache.clear();
        this->m_headssecId = static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
    }

    inline void deleteLastSector() {
        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        assert(this->m_ssecIdChainCache.size() > 1);

        if (m_ssecIdChainCache.size() > 2) {
            m_ssat.set(m_ssecIdChainCache[m_ssecIdChainCache.size() - 3], AllocTableEntry::END_OF_CHAIN);
        }
        m_ssat.set(m_ssecIdChainCache[m_ssecIdChainCache.size() - 2], AllocTableEntry::NOT_USED);
        m_ssecIdChainCache.erase(m_ssecIdChainCache.end() - 2);
        if (m_ssecIdChainCache.size() == 1) {
            this->m_headssecId = static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN);
        }
    }

    inline addr_t appendSector() {
        this->prepareCacheUntil(std::numeric_limits<size_t>::max());
        const auto newsec = m_ssat.alloc();
        m_ssat.set(newsec, static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        if (m_ssecIdChainCache.size() > 1) {
            m_ssecIdChainCache.pop_back();
            m_ssat.set(m_ssecIdChainCache.back(), newsec);
            m_ssecIdChainCache.push_back(newsec);
            m_ssecIdChainCache.push_back(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
        } else {
            m_ssecIdChainCache.insert(m_ssecIdChainCache.begin(), newsec);
            m_headssecId = newsec;
        }

        return static_cast<addr_t>(this->size() - this->shortSectorSize());
    }

    inline size_t size() const {
        return this->prepareCacheUntil(std::numeric_limits<size_t>::max());
    }

    inline size_t read(addr_t addr, void* buf, size_t n) const {
        const auto tlt = prepareCacheUntil(static_cast<size_t>(n + addr));
        if (tlt < static_cast<size_t>(n + addr))
            throw OutOfRange();

        const auto ss = this->shortSectorSize();
        int nread = 0;
        while (nread < n) {
            const auto ssecIdx = (addr + nread) / ss;
            const auto offset = (addr + nread) % ss;
            const auto k = std::min(n - nread, ss - offset);
            assert(m_ssecIdChainCache.size() > ssecIdx);
            const auto ssecId = m_ssecIdChainCache[ssecIdx];
            this->m_block.read(ssecId * ss + offset, (static_cast<char*>(buf) + nread), k);
            nread += k;
        }
        return nread;
    }

    inline size_t write(addr_t addr, const void* buf, size_t n) {
        const auto tlt = prepareCacheUntil(static_cast<size_t>(n + addr));
        if (tlt < static_cast<size_t>(n + addr)) {
            while (this->size() < static_cast<size_t>(n + addr)) {
                this->appendSector();
            }
        }

        const auto ss = this->shortSectorSize();
        int nwrited = 0;
        while (nwrited < n) {
            const auto ssecIdx = (addr + nwrited) / ss;
            const auto offset = (addr + nwrited) % ss;
            const auto k = std::min(n - nwrited, ss - offset);
            assert(m_ssecIdChainCache.size() > ssecIdx);
            const auto ssecId = m_ssecIdChainCache[ssecIdx];
            this->m_block.write(ssecId * ss + offset, (static_cast<const char*>(buf) + nwrited), k);
            nwrited += k;
        }
        return nwrited;
    }

    inline void fillzeros(addr_t begin, addr_t end) {
        const char buf[4096] = {0};
        while (begin < end) {
            const size_t s = std::min(sizeof(buf), end - begin);
            this->write(begin, buf, s);
            begin += s;
        }
    }
 
    inline size_t maxsize() const {
        return m_block.maxsize();
    }

    ShortSectorChainStream(const ShortSectorChainStream&) = delete;
    ShortSectorChainStream& operator=(const ShortSectorChainStream&) = delete;

    ShortSectorChainStream(ShortSectorChainStream&& oth):
        m_header(oth.m_header), m_ssat(oth.m_ssat), m_block(std::move(oth.m_block)),
        m_headssecId(oth.m_headssecId), m_ssecIdChainCache(oth.m_ssecIdChainCache)
    {
    }
    ShortSectorChainStream& operator=(ShortSectorChainStream&& oth) {
        assert(&m_header == &oth.m_header);
        assert(&m_ssat   == &oth.m_ssat);
        m_block = std::move(oth.m_block);
        m_ssecIdChainCache = std::move(oth.m_ssecIdChainCache);
        m_headssecId = oth.m_headssecId;
        return *this;
    }

private:
    inline size_t shortSectorSize() const { return m_header.sizeOfShortSector(); }

    inline size_t prepareCacheUntil(size_t limit) const {
        if (m_ssecIdChainCache.empty()) {
            m_ssecIdChainCache.push_back(m_headssecId);
        }

        while (m_ssecIdChainCache.back() != static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) &&
               m_ssecIdChainCache.size() * this->shortSectorSize() < limit)
        {
            auto next = m_ssat.get(m_ssecIdChainCache.back());
            m_ssecIdChainCache.push_back(next);
        }

        const auto n = m_ssecIdChainCache.back() == static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN) ? 
                           m_ssecIdChainCache.size() - 1: m_ssecIdChainCache.size();
        return n * this->shortSectorSize();
    }


private:
    CompoundFileHeaderAccessor<T>& m_header;
    ShortSectorAllocationTable<T>& m_ssat;
    BlockDeviceExt<BlockDeviceRefWrapper<ShortSectorStream<T>>> m_block;
    uint32_t                       m_headssecId;
    mutable std::vector<uint32_t>  m_ssecIdChainCache;
};

enum fileopenmode : int{
    CREATE = 1 << 0,
    WRITE  = 1 << 1,
    READ   = 1 << 2,
    APPEND = 1 << 3,
};

enum class seekwhence : int {
    SET, CUR, END
};

template<typename T, std::enable_if_t<Impl::is_block_device<T>::value,bool> = true>
class FileSystem {
private:
    FileSystem(T&& block, uint16_t majorVersion, uint16_t sectorShift, uint16_t shortSectorShift):
        m_block(std::move(block)),
        m_header(CompoundFileHeaderAccessor<BlockDeviceRefWrapper<DeviceType>>::format(
                    BlockDeviceRefWrapper<DeviceType>(m_block),
                    majorVersion, sectorShift, shortSectorShift)),
        m_sat(m_header, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_dirtable(m_header, m_sat, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_ssat(m_header, m_sat, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_sstream(m_header, m_dirtable, m_sat, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_handler_count(100)
    {
    }

    using DeviceType = BlockDeviceExt<T>;
    using mixstream_t = std::variant<SectorChainStream<BlockDeviceRefWrapper<DeviceType>>,ShortSectorChainStream<BlockDeviceRefWrapper<DeviceType>>>;
    inline mixstream_t OpenFileStream(uint32_t entryid)
    {
        const auto fssize = m_dirtable.getSize(entryid);
        if (fssize < m_header.getMinSizeOfStandardStream()) {
            return OpenShortStream(m_dirtable.getSectorID(entryid));
        }

        return OpenNormalStream(m_dirtable.getSectorID(entryid));
    }

    inline auto OpenShortStream(uint32_t ssecId) {
        return ShortSectorChainStream(
                m_header, m_ssat,
                m_sstream,
                ssecId);
    }

    inline auto OpenNormalStream(uint32_t secId) {
        return SectorChainStream(
                m_header, m_sat,
                BlockDeviceRefWrapper<DeviceType>(m_block),
                secId);
    }

    class FileStream {
    public:
        inline FileStream(FileSystem<T>& fs, uint32_t entryid):
            m_fs(fs), m_entryid(entryid), m_filesize(0), m_stream(m_fs.OpenFileStream(entryid))
        {
            m_filesize = m_fs.m_dirtable.getSize(m_entryid);
        }

        inline size_t write(addr_t addr, const void* buf, size_t bufsize) {
            if (this->isShort()) {
                if (addr + bufsize >= m_fs.m_header.getMinSizeOfStandardStream()) {
                    auto stream = m_fs.OpenNormalStream(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                    unsigned char cbuf[4096] = { 0 };
                    uint32_t idx = 0;
                    while (idx < m_filesize) {
                        const auto n = std::min(sizeof(cbuf), m_filesize - idx);
                        this->shortStream().read(idx, cbuf, n);
                        const auto v = stream.write(idx, cbuf, n);
                        assert(v == n);
                        idx += n;
                    }
                    this->shortStream().deleteStream();
                    const auto ans = stream.write(addr, buf, bufsize);
                    m_filesize = addr + bufsize;
                    m_fs.m_dirtable.setSize(m_entryid, m_filesize);
                    m_fs.m_dirtable.setSectorID(m_entryid, stream.getHeadSectorID());
                    m_stream = std::move(stream);
                    return ans;
                } else {
                    const auto ans = this->shortStream().write(addr, buf, bufsize);

                    if (m_filesize == 0)
                        m_fs.m_dirtable.setSectorID(m_entryid, this->shortStream().getHeadShortSectorID());

                    if (addr + ans > m_filesize) {
                        m_filesize = addr + ans;
                        m_fs.m_dirtable.setSize(m_entryid, m_filesize);
                    }
                    return ans;
                }
            } else {
                const auto ans = this->normalStream().write(addr, buf, bufsize);

                if (addr + ans > m_filesize) {
                    m_filesize = addr + ans;
                    m_fs.m_dirtable.setSize(m_entryid, m_filesize);
                }
                return ans;
            }
        }

        inline size_t read (addr_t addr, void* buf, size_t bufsize) const {
            if (addr + bufsize > m_filesize)
                throw OutOfRange();

            if (this->isShort()) {
                return this->shortStream().read(addr, buf, bufsize);
            } else {
                return this->normalStream().read(addr, buf, bufsize);
            }
        }

        inline void truncate(size_t newsize) {
            if (newsize == 0) {
                if (this->isShort()) {
                    this->shortStream().deleteStream();
                } else {
                    this->normalStream().deleteStream();
                }
                m_filesize = 0;
                return;
            }

            if (newsize > m_filesize) {
                const unsigned char buf[4096] = { 0 };
                auto fsize  = m_filesize;
                while (fsize < newsize) {
                    auto n = std::min(sizeof(buf), newsize - fsize);
                    this->write(fsize, buf, n);
                    fsize += n;
                }
            } else {
                if (this->isShort()) {
                    auto& shortst = this->shortStream();
                    while (shortst.size() > newsize - 1 + m_fs.m_header.sizeOfShortSector()) {
                        shortst.deleteLastSector();
                    }
                } else {
                    if (newsize < m_fs.m_header.getMinSizeOfStandardStream()) {
                        auto stream = m_fs.OpenShortStream(static_cast<uint32_t>(AllocTableEntry::END_OF_CHAIN));
                        unsigned char buf[4096] = { 0 };
                        uint32_t idx = 0;
                        while (idx < newsize) {
                            const auto n = std::min(sizeof(buf), newsize - idx);
                            this->read(idx, buf, n);
                            stream.write(idx, buf, n);
                            idx += n;
                        }
                        m_fs.m_dirtable.setSectorID(m_entryid, stream.getHeadShortSectorID());
                        m_stream = std::move(stream);
                    } else {
                        auto& normalst = this->normalStream();
                        while (normalst.size() > newsize - 1 + m_fs.m_header.sizeOfSector()) {
                            normalst.deleteLastSector();
                        }
                    }
                }
            }

            m_filesize = newsize;
            m_fs.m_dirtable.setSize(m_entryid, m_filesize);
        }

        inline uint32_t entryid() const { return m_entryid; };

        inline size_t size() const { return m_filesize; }

    private:
        auto& normalStream() { return std::get<0>(m_stream); }
        auto& shortStream()  { return std::get<1>(m_stream); }

        const auto& normalStream() const { return std::get<0>(m_stream); }
        const auto& shortStream()  const { return std::get<1>(m_stream); }


        bool isShort() const {
            return std::holds_alternative<ShortSectorChainStream<BlockDeviceRefWrapper<DeviceType>>>(m_stream);
        }

        FileSystem<T>& m_fs;
        uint32_t       m_entryid;
        size_t         m_filesize;
        mixstream_t    m_stream;
    };

    struct StatInfo {
        EntryType m_type;
        size_t    m_size;
    };

    using FSPath = std::vector<std::string>;

    struct OpenedFile {
        FileStream m_stream;
        FSPath     m_path;
        size_t     m_ref;

        OpenedFile(const FSPath& path, FileSystem<T>& fs, uint32_t entryid):
            m_stream(fs, entryid), m_path(path), m_ref(0)
        {
        }
    };

    struct FileEntry {
        OpenedFile*  m_file;
        size_t       m_offset;
        fileopenmode m_mode;
    };

    struct OpenedDirectory {
        FSPath   m_path;
        size_t   m_ref;
        uint32_t m_entryid;

        OpenedDirectory(const FSPath& path, uint32_t entryid):
            m_path(path), m_ref(0), m_entryid(entryid)
        {
        }
    };

    struct DirectoryEntry {
        OpenedDirectory* m_dir;
    };

    using file_t = int;
    using dir_t  = std::optional<int>;

    enum class ErrorCode {
        noerror = 0,
        invalid_handle,
        file_opened,
        already_exists,
        nonfile_already_exists,
        not_found,
        not_empty,
        invalid_path,
        cannot_move,
        permission_denied,
    };
    ErrorCode mutable m_errcode;

    DeviceType                                                    m_block;
    CompoundFileHeaderAccessor<BlockDeviceRefWrapper<DeviceType>> m_header;
    SectorAllocationTable<BlockDeviceRefWrapper<DeviceType>>      m_sat;
    DirectoryTable<BlockDeviceRefWrapper<DeviceType>>             m_dirtable;
    ShortSectorAllocationTable<BlockDeviceRefWrapper<DeviceType>> m_ssat;
    ShortSectorStream<BlockDeviceRefWrapper<DeviceType>>          m_sstream;

    // File System State
    std::map<FSPath,OpenedFile>      m_openedFiles;
    std::map<FSPath,OpenedDirectory> m_openedDirectories;
    std::map<file_t,FileEntry>       mutable m_fileEntires;
    std::map<dir_t, DirectoryEntry>  m_dirEntires;
    int                              m_handler_count;

    inline void decRef(const FSPath& path) {
        auto p = path;
        if (m_openedFiles.count(p)) {
            m_openedFiles.at(p).m_ref--;
            if (m_openedFiles.at(p).m_ref == 0) {
                m_openedFiles.erase(m_openedFiles.find(p));
            }
        }

        if (m_openedDirectories.count(p)) {
            m_openedDirectories.at(p).m_ref--;
            if (m_openedDirectories.at(p).m_ref == 0) {
                m_openedDirectories.erase(m_openedDirectories.find(p));
            }
        }

        if (p.empty()) return;
        p.pop_back();
        for (;!p.empty();p.pop_back()) {
            m_openedDirectories.at(p).m_ref--;
            if (m_openedDirectories.at(p).m_ref == 0) {
                m_openedDirectories.erase(m_openedDirectories.find(p));
            }
        }
        m_openedDirectories.at(p).m_ref--;
    }

    inline std::array<uint16_t,32> buildName(const std::string& str) {
        std::array<uint16_t,32> ans = { 0 };
        for (size_t i=0;i<str.size()&&i<ans.size();i++) {
            ans[i] = str[i];
        }
        return ans;
    }

    inline bool openDirectory(const FSPath& path) {
        FSPath p;
        if (m_openedDirectories.count(p) == 0) {
            m_openedDirectories.insert(std::make_pair(p, OpenedDirectory(p, 0)));
        }
        auto ptr_parentDir = &m_openedDirectories.at(p);
        ptr_parentDir->m_ref++;

        for (size_t i=0;i<path.size();i++) {
            p.push_back(path[i]);
            if (m_openedDirectories.count(p) > 0) {
                m_openedDirectories.at(p).m_ref++;
            } else {
                const auto child = m_dirtable.searchChild(ptr_parentDir->m_entryid, this->buildName(path[i]));
                if (child.has_value()) {
                    const auto child_id = child.value();
                    if (m_dirtable.getEntryType(child_id) != EntryType::UserStorage) {
                        p.pop_back();
                        this->decRef(p);
                        return false;
                    }

                    m_openedDirectories.insert(std::make_pair(p, OpenedDirectory(p, child_id)));
                } else {
                    p.pop_back();
                    this->decRef(p);
                    return false;
                }
            }

            ptr_parentDir = &m_openedDirectories.at(p);
        }

        return true;
    }

    inline bool openFile(FSPath dir, const std::string& basename) {
        assert(m_openedDirectories.count(dir) > 0);

        {
            dir.push_back(basename);
            if (m_openedFiles.count(dir) > 0) {
                m_openedFiles.at(dir).m_ref++;
                return true;
            }
            dir.pop_back();
        }

        const auto& parentdir = m_openedDirectories.at(dir);
        const auto child = m_dirtable.searchChild(parentdir.m_entryid, this->buildName(basename));
        if (child.has_value()) {
            const auto child_id = child.value();
            if (m_dirtable.getEntryType(child_id) != EntryType::UserStream) {
                this->decRef(dir);
                this->m_errcode = ErrorCode::nonfile_already_exists;
                return false;
            }

            dir.push_back(basename);
            m_openedFiles.insert(std::make_pair(dir, OpenedFile(dir, *this, child_id)));
            m_openedFiles.at(dir).m_ref++;
        } else {
            this->decRef(dir);
            this->m_errcode = ErrorCode::not_found;
            return false;
        }

        return true;
    }

    inline bool openFile(FSPath path) {
        assert(!path.empty());
        const auto basename = path.back();
        path.pop_back();

        if (!this->openDirectory(path)) return false;

        return openFile(path, basename);
    }

public:
    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;

    explicit FileSystem(T&& block):
        m_block(std::move(block)),
        m_header(BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_sat(m_header, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_dirtable(m_header, m_sat, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_ssat(m_header, m_sat, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_sstream(m_header, m_dirtable, m_sat, BlockDeviceRefWrapper<DeviceType>(m_block)),
        m_handler_count(100)
    {
    }

    static FileSystem format(T&& block, uint16_t majorVersion, uint16_t sectorShift, uint16_t shortSectorShift)
    {
        return FileSystem(std::move(block), majorVersion, sectorShift, shortSectorShift);
    }

    inline ErrorCode get_error() const
    {
        return this->m_errcode;
    }

    bool mkdir(const FSPath& dirname) {
        assert(!dirname.empty());
        auto pp = dirname;
        pp.pop_back();
        if (!this->openDirectory(pp))
            return false;

        const auto& parentDir = m_openedDirectories.at(pp);
        const auto entryid = m_dirtable.createEntry(parentDir.m_entryid, this->buildName(dirname.back()), EntryType::UserStorage);
        this->decRef(pp);
        if (!entryid.has_value()) {
            this->m_errcode = ErrorCode::already_exists;
            return false;
        }

        m_dirtable.setSectorID(entryid.value(), 0);
        return true;
    }

    bool rmdir(const FSPath& dirname) {
        assert(!dirname.empty());
        if (!this->openDirectory(dirname))
            return false;

        const auto& dir = m_openedDirectories.at(dirname);
        if (dir.m_ref > 1) {
            m_errcode = ErrorCode::file_opened;
            this->decRef(dirname);
            return false;
        }

        if (m_dirtable.getSubdirectoryEntry(dir.m_entryid) != static_cast<uint32_t>(-1)) {
            m_errcode = ErrorCode::not_empty;
            this->decRef(dirname);
            return false;
        }

        auto pp = dirname;
        pp.pop_back();
        const auto& uu = m_openedDirectories.at(pp);
        m_dirtable.deleteEntry(uu.m_entryid, dir.m_entryid).value();
        this->decRef(dirname);
        return true;
    }

    inline std::optional<file_t> open(const FSPath& filename, int mode) { return this->open(filename, static_cast<fileopenmode>(mode)); }

    std::optional<file_t> open(const FSPath& filename, fileopenmode mode) {
        auto pp = filename;
        const auto basename = pp.back();
        pp.pop_back();

        if (!this->openDirectory(pp)) return std::nullopt;

        const auto& parentDir = m_openedDirectories.at(pp);
        if (!this->openFile(pp, basename)) {
            if (m_errcode == ErrorCode::nonfile_already_exists) {
                return std::nullopt;
            }

            assert(m_errcode == ErrorCode::not_found);
            if (mode & fileopenmode::CREATE) {
                m_dirtable.createEntry(parentDir.m_entryid, this->buildName(basename), EntryType::UserStream).value();
                const auto result = this->openFile(pp, basename);
                assert(result);
            } else {
                return std::nullopt;
            }
        }

        const auto handler = m_handler_count++;
        FileEntry entry {&m_openedFiles.at(filename), 0, mode};
        if (mode & fileopenmode::APPEND) {
            entry.m_offset = entry.m_file->m_stream.size();
        } else if (mode & fileopenmode::WRITE) {
            entry.m_file->m_stream.truncate(0);
        }

        m_fileEntires[handler] = std::move(entry);
        return handler;
    }

    bool close(file_t file) {
        if (m_fileEntires.count(file) == 0)
            return false;

        const auto entry = m_fileEntires[file];
        m_fileEntires.erase(m_fileEntires.find(file));
        decRef(entry.m_file->m_path);
        return true;
    }

    std::optional<dir_t> opendir(const FSPath& path) {
        if (!this->openDirectory(path)) return std::nullopt;

        const int handler = m_handler_count++;
        DirectoryEntry entry {m_openedDirectories.at(path)};
        m_dirEntires[handler] = entry;

        return handler;
    }

    bool closedir(dir_t dir) {
        if (this->m_dirEntires.count(dir) == 0) {
            this->m_errcode = ErrorCode::invalid_handle;
            return false;
        }

        auto dirEntry = this->m_dirEntires[dir];
        const auto path = dirEntry.m_dir->m_path;
        this->decRef(path);
        return true;
    }

    bool unlink(const FSPath& filename) {
        assert(!filename.empty());
        if (!this->openFile(filename))
            return false;

        auto& file = m_openedFiles.at(filename);
        if (file.m_ref > 1) {
            m_errcode = ErrorCode::file_opened;
            this->decRef(filename);
            return false;
        }

        auto pp = filename;
        pp.pop_back();
        const auto& uu = m_openedDirectories.at(pp);
        file.m_stream.truncate(0);
        m_dirtable.deleteEntry(uu.m_entryid, file.m_stream.entryid()).value();
        this->decRef(filename);
        return true;
    }

    /**
     * move a file/directory to new path
     *
     * @param path original path, should be a valid path to file or directory
     * @param newpath new path which should not exist, but whose parent directory must exist
     * @return true if everything is fine otherwise false
     */
    bool move(const FSPath& path, const FSPath& newpath) {
        if (path.empty() || newpath.empty()) {
            m_errcode = ErrorCode::cannot_move;
            return false;
        }

        if (m_openedFiles.count(path) > 0) {
            m_errcode = ErrorCode::file_opened;
            return false;
        }

        auto newpp = newpath;
        newpp.pop_back();
        if (!this->openDirectory(newpp)) {
            m_errcode = ErrorCode::invalid_path;
            return false;
        }

        const auto nu = m_dirtable.searchChild(m_openedDirectories.at(newpp).m_entryid, this->buildName(newpath.back()));
        if (nu.has_value()) {
            m_errcode = ErrorCode::already_exists;
            this->decRef(newpp);
            return false;
        }

        auto pp = path;
        pp.pop_back();
        if (!this->openDirectory(pp)) {
            m_errcode = ErrorCode::invalid_path;
            this->decRef(newpp);
            return false;
        }

        const auto ku = m_dirtable.searchChild(m_openedDirectories.at(pp).m_entryid, this->buildName(path.back()));
        if (!ku.has_value()) {
            m_errcode = ErrorCode::not_found;
            this->decRef(pp);
            this->decRef(newpp);
            return false;
        }

        const auto nd = m_dirtable.deleteEntry(m_openedDirectories.at(pp).m_entryid, ku.value()).value();
        m_dirtable.setName(nd, this->buildName(newpath.back()));
        m_dirtable.insertEntry(m_openedDirectories.at(newpp).m_entryid, nd).value();
        this->decRef(pp);
        this->decRef(newpp);
        return true;
    }

    size_t read(file_t file, void* buf, size_t n) const {
        if (m_fileEntires.count(file) == 0) {
            m_errcode = ErrorCode::invalid_handle;
            return 0;
        }

        auto& ff = m_fileEntires.at(file);
        if ((ff.m_mode & fileopenmode::READ) == 0) {
            m_errcode = ErrorCode::permission_denied;
            return 0;
        }

        const auto ans = ff.m_file->m_stream.read(ff.m_offset, buf, n);
        ff.m_offset += ans;

        return ans;
    }

    size_t write(file_t file, const void* buf, size_t n) {
        if (m_fileEntires.count(file) == 0) {
            m_errcode = ErrorCode::invalid_handle;
            return 0;
        }

        auto& ff = m_fileEntires.at(file);
        if ((ff.m_mode & (fileopenmode::WRITE | fileopenmode::CREATE | fileopenmode::APPEND)) == 0) {
            m_errcode = ErrorCode::permission_denied;
            return 0;
        }

        const auto ans = ff.m_file->m_stream.write(ff.m_offset, buf, n);
        ff.m_offset += ans;

        return ans;
    }

    bool seek(file_t file, long offset, seekwhence pos) {
        if (m_fileEntires.count(file) == 0) {
            m_errcode = ErrorCode::invalid_handle;
            return false;
        }

        auto& ff = m_fileEntires.at(file);

        size_t toffset = ff.m_offset;
        if (pos == seekwhence::CUR) {
            toffset = ff.m_offset + offset;
        } else if (pos == seekwhence::END) {
            toffset = ff.m_file->m_stream.size() + offset;
        } else {
            toffset = offset;
        }

        ff.m_offset = toffset;
        return true;
    }

    bool truncate(file_t file, size_t new_size) {
        if (m_fileEntires.count(file) == 0) {
            m_errcode = ErrorCode::invalid_handle;
            return false;
        }

        auto& ff = m_fileEntires.at(file);
        ff.m_file->m_stream.truncate(new_size);
        ff.m_offset = std::min(ff.m_offset, new_size);

        return true;
    }

    std::optional<StatInfo> stat(const FSPath& path) {
        if (path.empty()) {
            return StatInfo { EntryType::RootStorage, 0 };
        }

        auto pp = path;
        pp.pop_back();
        if (!this->openDirectory(pp)) {
            m_errcode = ErrorCode::invalid_path;
            return std::nullopt;
        }

        const auto nu = m_dirtable.searchChild(m_openedDirectories.at(pp).m_entryid, this->buildName(path.back()));
        if (!nu.has_value()) {
            m_errcode = ErrorCode::invalid_path;
            this->decRef(pp);
            return std::nullopt;
        }

        const auto type = static_cast<EntryType>(m_dirtable.getEntryType(nu.value()));
        size_t size = m_dirtable.getSize(nu.value());
        return StatInfo { type, size };
    }
};

class MemorySpace {
    public:
        inline explicit MemorySpace(size_t size): m_space(size, 0) { }

        inline size_t read (addr_t addr, void* buf, size_t n) const {
            if (addr + n > this->maxsize()) {
                throw OutOfRange();
        }

        memcpy(buf, this->m_space.data() + addr, n);
        return n;
    }

    inline size_t write(addr_t addr, const void* buf, size_t n) {
        if (addr + n > this->maxsize()) {
            throw OutOfRange();
        }

        memcpy(this->m_space.data() + addr, buf, n);
        return n;
    }

    inline size_t maxsize() const {
        return this->m_space.size();
    }

private:
    std::vector<char> m_space;
};
}
