#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <tuple>

enum class TDFType : uint8_t {
    Bool   = 0xB0,
    Int32  = 0xA0,
    UInt32 = 0xA1,
    Float  = 0xF0,
    String = 0xC0,
    Null   = 0xD0,

    ArrBool   = 0xE0,
    ArrInt32  = 0xE1,
    ArrUInt32 = 0xE2,
    ArrFloat  = 0xE3,
    ArrString = 0xE4
};

struct TDFValue {
    TDFType type;
    std::vector<uint8_t> data;
};

class TDFFile {
public:
    std::unordered_map<std::string, TDFValue> entries;

    // Save file
    void save(const std::string& path, uint8_t version = 0x10) const {
        std::ofstream out(path, std::ios::binary);
        if (!out) throw std::runtime_error("Cannot open file");

        out.write("TDF", 3);
        out.put(version);

        std::vector<uint8_t> index;
        std::vector<uint8_t> data;

        for (auto& [key, val] : entries) {
            uint32_t offset = (uint32_t)data.size();

            index.push_back((uint8_t)key.size());
            index.insert(index.end(), key.begin(), key.end());
            index.push_back((uint8_t)val.type);

            index.insert(index.end(),
                         reinterpret_cast<uint8_t*>(&offset),
                         reinterpret_cast<uint8_t*>(&offset) + 4);

            data.insert(data.end(), val.data.begin(), val.data.end());
        }
        index.push_back(0x00);

        out.write(reinterpret_cast<char*>(index.data()), index.size());
        out.write(reinterpret_cast<char*>(data.data()), data.size());
    }

    // Load file
    void load(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open file");

        char magic[3];
        in.read(magic, 3);
        if (std::memcmp(magic, "TDF", 3) != 0)
            throw std::runtime_error("Invalid TDF");

        uint8_t version;
        in.read(reinterpret_cast<char*>(&version), 1);

        entries.clear();

        std::vector<std::tuple<std::string, TDFType, uint32_t>> index;
        while (true) {
            uint8_t keyLen;
            in.read(reinterpret_cast<char*>(&keyLen), 1);
            if (keyLen == 0) break;

            std::string key(keyLen, '\0');
            in.read(key.data(), keyLen);

            uint8_t type;
            in.read(reinterpret_cast<char*>(&type), 1);

            uint32_t offset;
            in.read(reinterpret_cast<char*>(&offset), 4);

            index.emplace_back(key, (TDFType)type, offset);
        }

        std::streampos dataStart = in.tellg();

        for (auto& [key, type, offset] : index) {
            in.seekg(dataStart + std::streamoff(offset));

            TDFValue val;
            val.type = type;
            readValue(in, val);

            entries[key] = val;
        }
    }

    // --- Access / Modify ---

    void set(const std::string& key, const TDFValue& val) {
        entries[key] = val;
    }

    bool remove(const std::string& key) {
        return entries.erase(key) > 0;
    }

    bool has(const std::string& key) const {
        return entries.find(key) != entries.end();
    }

    // Append to an array
    void appendToArray(const std::string& key, const TDFValue& val) {
        auto it = entries.find(key);
        if (it == entries.end())
            throw std::runtime_error("Key not found");

        TDFValue& target = it->second;

        if (!isArray(target.type) || elementType(target.type) != val.type)
            throw std::runtime_error("Type mismatch or target is not an array");

        uint32_t len;
        std::memcpy(&len, target.data.data(), 4);
        len++;

        size_t elemSize = elementSize(target.type, val.type);

        size_t oldLen = elementSize(target.type, val.type) * (len - 1);
        size_t newSize = 4 + elemSize * len;
        target.data.resize(newSize);

        std::memcpy(target.data.data() + 4 + oldLen,
                    val.data.data() + (val.type == TDFType::String ? 0 : 0),
                    elemSize);

        std::memcpy(target.data.data(), &len, 4);
    }

    void deleteFromArray(const std::string& key, size_t idx) {
        auto it = entries.find(key);
        if (it == entries.end())
            throw std::runtime_error("Key not found");

        TDFValue& target = it->second;

        if (!isArray(target.type))
            throw std::runtime_error("Target is not an array");

        uint32_t len;
        std::memcpy(&len, target.data.data(), 4);

        if (idx >= len)
            throw std::runtime_error("Index out of range");

        size_t elemSize = elementSize(target.type, elementType(target.type));

        for (size_t i = idx + 1; i < len; ++i) {
            std::memcpy(target.data.data() + 4 + (i - 1) * elemSize,
                        target.data.data() + 4 + i * elemSize,
                        elemSize);
        }

        len--;
        target.data.resize(4 + len * elemSize);
        std::memcpy(target.data.data(), &len, 4);
    }

    // --- Static constructors ---

    static TDFValue makeNull() { return { TDFType::Null, {} }; }
    static TDFValue makeBool(bool v) { return { TDFType::Bool, { uint8_t(v ? 1 : 0) } }; }

    static TDFValue makeInt(int32_t v) {
        TDFValue t{ TDFType::Int32 };
        t.data.resize(4);
        std::memcpy(t.data.data(), &v, sizeof(v));
        return t;
    }

    static TDFValue makeUInt(uint32_t v) {
        TDFValue t{ TDFType::UInt32 };
        t.data.resize(4);
        std::memcpy(t.data.data(), &v, sizeof(v));
        return t;
    }

    static TDFValue makeFloat(float v) {
        TDFValue t{ TDFType::Float };
        t.data.resize(4);
        std::memcpy(t.data.data(), &v, sizeof(v));
        return t;
    }

    static TDFValue makeString(const std::string& s) {
        TDFValue t{ TDFType::String };
        t.data.assign(s.begin(), s.end());
        t.data.push_back(0x00);
        return t;
    }

    static TDFValue makeBoolArray(const std::vector<bool>& arr) {
        TDFValue t{ TDFType::ArrBool };
        uint32_t len = (uint32_t)arr.size();
        t.data.resize(4 + len);
        std::memcpy(t.data.data(), &len, 4);
        for (size_t i = 0; i < len; ++i) t.data[4 + i] = arr[i] ? 1 : 0;
        return t;
    }

    static TDFValue makeIntArray(const std::vector<int32_t>& arr) {
        TDFValue t{ TDFType::ArrInt32 };
        uint32_t len = (uint32_t)arr.size();
        t.data.resize(4 + len * 4);
        std::memcpy(t.data.data(), &len, 4);
        std::memcpy(t.data.data() + 4, arr.data(), len * 4);
        return t;
    }

    static TDFValue makeUIntArray(const std::vector<uint32_t>& arr) {
        TDFValue t{ TDFType::ArrUInt32 };
        uint32_t len = (uint32_t)arr.size();
        t.data.resize(4 + len * 4);
        std::memcpy(t.data.data(), &len, 4);
        std::memcpy(t.data.data() + 4, arr.data(), len * 4);
        return t;
    }

    static TDFValue makeFloatArray(const std::vector<float>& arr) {
        TDFValue t{ TDFType::ArrFloat };
        uint32_t len = (uint32_t)arr.size();
        t.data.resize(4 + len * 4);
        std::memcpy(t.data.data(), &len, 4);
        std::memcpy(t.data.data() + 4, arr.data(), len * 4);
        return t;
    }

    static TDFValue makeStringArray(const std::vector<std::string>& arr) {
        TDFValue t{ TDFType::ArrString };
        std::vector<uint8_t> tmp;
        uint32_t len = (uint32_t)arr.size();
        tmp.resize(4);
        std::memcpy(tmp.data(), &len, 4);
        for (auto& s : arr) {
            tmp.insert(tmp.end(), s.begin(), s.end());
            tmp.push_back(0x00);
        }
        t.data = std::move(tmp);
        return t;
    }

private:
    static void readValue(std::ifstream& in, TDFValue& v) {
        if (v.type == TDFType::Null) return;

        if (isArray(v.type)) {
            uint32_t len;
            in.read(reinterpret_cast<char*>(&len), 4);
            size_t elemSize = elementSize(v.type, elementType(v.type));
            v.data.resize(4 + elemSize * len);
            std::memcpy(v.data.data(), &len, 4);

            if (v.type == TDFType::ArrString) {
                for (uint32_t i = 0; i < len; ++i) {
                    char c;
                    do {
                        in.read(&c, 1);
                        v.data.push_back(c);
                    } while (c != 0x00);
                }
            } else {
                in.read(reinterpret_cast<char*>(v.data.data() + 4), elemSize * len);
            }
            return;
        }

        if (v.type == TDFType::String) {
            uint8_t c;
            do {
                in.read(reinterpret_cast<char*>(&c), 1);
                v.data.push_back(c);
            } while (c != 0x00);
            return;
        }

        size_t sz = elementSize(v.type, v.type);
        v.data.resize(sz);
        in.read(reinterpret_cast<char*>(v.data.data()), sz);
    }

    static bool isArray(TDFType t) {
        return (uint8_t)t >= 0xE0 && (uint8_t)t <= 0xE4;
    }

    static TDFType elementType(TDFType t) {
        switch (t) {
            case TDFType::ArrBool:   return TDFType::Bool;
            case TDFType::ArrInt32:  return TDFType::Int32;
            case TDFType::ArrUInt32: return TDFType::UInt32;
            case TDFType::ArrFloat:  return TDFType::Float;
            case TDFType::ArrString: return TDFType::String;
            default: return TDFType::Null;
        }
    }

    static size_t elementSize(TDFType arrType, TDFType elemType) {
        switch (elemType) {
            case TDFType::Bool:   return 1;
            case TDFType::Int32:
            case TDFType::UInt32:
            case TDFType::Float:  return 4;
            case TDFType::String: return 1; // strings are variable, handled separately
            default: return 0;
        }
    }
};
