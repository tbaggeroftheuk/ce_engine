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
    TDFType Type;
    std::vector<uint8_t> Data;
};

class TDFFile {
public:
    std::unordered_map<std::string, TDFValue> Entries;

    // --- Save / Load ---
    void Save(const std::string& path, uint8_t version = 0x10) const {
        std::ofstream Out(path, std::ios::binary);
        if (!Out) throw std::runtime_error("Cannot open file");

        Out.write("TDF", 3);
        Out.put(version);

        std::vector<uint8_t> Index;
        std::vector<uint8_t> Data;

        for (auto& [key, val] : Entries) {
            uint32_t Offset = static_cast<uint32_t>(Data.size());

            Index.push_back(static_cast<uint8_t>(key.size()));
            Index.insert(Index.end(), key.begin(), key.end());
            Index.push_back(static_cast<uint8_t>(val.Type));

            Index.insert(Index.end(),
                         reinterpret_cast<uint8_t*>(&Offset),
                         reinterpret_cast<uint8_t*>(&Offset) + 4);

            Data.insert(Data.end(), val.Data.begin(), val.Data.end());
        }
        Index.push_back(0x00);

        Out.write(reinterpret_cast<char*>(Index.data()), Index.size());
        Out.write(reinterpret_cast<char*>(Data.data()), Data.size());
    }

    void Load(const std::string& path) {
        std::ifstream In(path, std::ios::binary);
        if (!In) throw std::runtime_error("Cannot open file");

        char Magic[3];
        In.read(Magic, 3);
        if (std::memcmp(Magic, "TDF", 3) != 0)
            throw std::runtime_error("Invalid TDF");

        uint8_t Version;
        In.read(reinterpret_cast<char*>(&Version), 1);

        Entries.clear();

        std::vector<std::tuple<std::string, TDFType, uint32_t>> Index;
        while (true) {
            uint8_t KeyLen;
            In.read(reinterpret_cast<char*>(&KeyLen), 1);
            if (KeyLen == 0) break;

            std::string Key(KeyLen, '\0');
            In.read(Key.data(), KeyLen);

            uint8_t Type;
            In.read(reinterpret_cast<char*>(&Type), 1);

            uint32_t Offset;
            In.read(reinterpret_cast<char*>(&Offset), 4);

            Index.emplace_back(Key, static_cast<TDFType>(Type), Offset);
        }

        std::streampos DataStart = In.tellg();

        for (auto& [key, type, offset] : Index) {
            In.seekg(DataStart + std::streamoff(offset));

            TDFValue Val{ type, {} };
            ReadValue(In, Val);

            Entries[key] = Val;
        }
    }

    // --- Access / Modify ---
    void Set(const std::string& key, const TDFValue& val) {
        Entries[key] = val;
    }

    bool Remove(const std::string& key) {
        return Entries.erase(key) > 0;
    }

    bool Has(const std::string& key) const {
        return Entries.find(key) != Entries.end();
    }

    void AppendToArray(const std::string& key, const TDFValue& val) {
        auto It = Entries.find(key);
        if (It == Entries.end()) throw std::runtime_error("Key not found");

        TDFValue& Target = It->second;
        if (!IsArray(Target.Type) || ElementType(Target.Type) != val.Type)
            throw std::runtime_error("Type mismatch or target is not an array");

        uint32_t Len;
        std::memcpy(&Len, Target.Data.data(), 4);
        Len++;

        size_t ElemSize = ElementSize(Target.Type, val.Type);
        size_t OldLen = ElemSize * (Len - 1);
        size_t NewSize = 4 + ElemSize * Len;
        Target.Data.resize(NewSize);

        std::memcpy(Target.Data.data() + 4 + OldLen,
                    val.Data.data(),
                    ElemSize);

        std::memcpy(Target.Data.data(), &Len, 4);
    }

    void DeleteFromArray(const std::string& key, size_t idx) {
        auto It = Entries.find(key);
        if (It == Entries.end()) throw std::runtime_error("Key not found");

        TDFValue& Target = It->second;
        if (!IsArray(Target.Type)) throw std::runtime_error("Target is not an array");

        uint32_t Len;
        std::memcpy(&Len, Target.Data.data(), 4);
        if (idx >= Len) throw std::runtime_error("Index out of range");

        size_t ElemSize = ElementSize(Target.Type, ElementType(Target.Type));
        for (size_t I = idx + 1; I < Len; ++I) {
            std::memcpy(Target.Data.data() + 4 + (I - 1) * ElemSize,
                        Target.Data.data() + 4 + I * ElemSize,
                        ElemSize);
        }

        Len--;
        Target.Data.resize(4 + Len * ElemSize);
        std::memcpy(Target.Data.data(), &Len, 4);
    }

    // --- Static constructors ---
    static TDFValue MakeNull() { return { TDFType::Null, {} }; }
    static TDFValue MakeBool(bool v) { return { TDFType::Bool, { uint8_t(v ? 1 : 0) } }; }

    static TDFValue MakeInt(int32_t v) {
        TDFValue T{ TDFType::Int32, {} };
        T.Data.resize(4);
        std::memcpy(T.Data.data(), &v, sizeof(v));
        return T;
    }

    static TDFValue MakeUInt(uint32_t v) {
        TDFValue T{ TDFType::UInt32, {} };
        T.Data.resize(4);
        std::memcpy(T.Data.data(), &v, sizeof(v));
        return T;
    }

    static TDFValue MakeFloat(float v) {
        TDFValue T{ TDFType::Float, {} };
        T.Data.resize(4);
        std::memcpy(T.Data.data(), &v, sizeof(v));
        return T;
    }

    static TDFValue MakeString(const std::string& s) {
        TDFValue T{ TDFType::String, {} };
        T.Data.assign(s.begin(), s.end());
        T.Data.push_back(0x00);
        return T;
    }

    static TDFValue MakeBoolArray(const std::vector<bool>& arr) {
        TDFValue T{ TDFType::ArrBool, {} };
        uint32_t Len = static_cast<uint32_t>(arr.size());
        T.Data.resize(4 + Len);
        std::memcpy(T.Data.data(), &Len, 4);
        for (size_t I = 0; I < Len; ++I) T.Data[4 + I] = arr[I] ? 1 : 0;
        return T;
    }

    static TDFValue MakeIntArray(const std::vector<int32_t>& arr) {
        TDFValue T{ TDFType::ArrInt32, {} };
        uint32_t Len = static_cast<uint32_t>(arr.size());
        T.Data.resize(4 + Len * 4);
        std::memcpy(T.Data.data(), &Len, 4);
        std::memcpy(T.Data.data() + 4, arr.data(), Len * 4);
        return T;
    }

    static TDFValue MakeUIntArray(const std::vector<uint32_t>& arr) {
        TDFValue T{ TDFType::ArrUInt32, {} };
        uint32_t Len = static_cast<uint32_t>(arr.size());
        T.Data.resize(4 + Len * 4);
        std::memcpy(T.Data.data(), &Len, 4);
        std::memcpy(T.Data.data() + 4, arr.data(), Len * 4);
        return T;
    }

    static TDFValue MakeFloatArray(const std::vector<float>& arr) {
        TDFValue T{ TDFType::ArrFloat, {} };
        uint32_t Len = static_cast<uint32_t>(arr.size());
        T.Data.resize(4 + Len * 4);
        std::memcpy(T.Data.data(), &Len, 4);
        std::memcpy(T.Data.data() + 4, arr.data(), Len * 4);
        return T;
    }

    static TDFValue MakeStringArray(const std::vector<std::string>& arr) {
        TDFValue T{ TDFType::ArrString, {} };
        std::vector<uint8_t> Tmp;
        uint32_t Len = static_cast<uint32_t>(arr.size());
        Tmp.resize(4);
        std::memcpy(Tmp.data(), &Len, 4);
        for (auto& S : arr) {
            Tmp.insert(Tmp.end(), S.begin(), S.end());
            Tmp.push_back(0x00);
        }
        T.Data = std::move(Tmp);
        return T;
    }

private:
    static void ReadValue(std::ifstream& in, TDFValue& v) {
        if (v.Type == TDFType::Null) return;

        if (IsArray(v.Type)) {
            uint32_t Len;
            in.read(reinterpret_cast<char*>(&Len), 4);
            size_t ElemSize = ElementSize(v.Type, ElementType(v.Type));
            v.Data.resize(4 + ElemSize * Len);
            std::memcpy(v.Data.data(), &Len, 4);

            if (v.Type == TDFType::ArrString) {
                for (uint32_t I = 0; I < Len; ++I) {
                    char C;
                    do {
                        in.read(&C, 1);
                        v.Data.push_back(C);
                    } while (C != 0x00);
                }
            } else {
                in.read(reinterpret_cast<char*>(v.Data.data() + 4), ElemSize * Len);
            }
            return;
        }

        if (v.Type == TDFType::String) {
            uint8_t C;
            do {
                in.read(reinterpret_cast<char*>(&C), 1);
                v.Data.push_back(C);
            } while (C != 0x00);
            return;
        }

        size_t Sz = ElementSize(v.Type, v.Type);
        v.Data.resize(Sz);
        in.read(reinterpret_cast<char*>(v.Data.data()), Sz);
    }

    static bool IsArray(TDFType t) {
        return (uint8_t)t >= 0xE0 && (uint8_t)t <= 0xE4;
    }

    static TDFType ElementType(TDFType t) {
        switch (t) {
            case TDFType::ArrBool:   return TDFType::Bool;
            case TDFType::ArrInt32:  return TDFType::Int32;
            case TDFType::ArrUInt32: return TDFType::UInt32;
            case TDFType::ArrFloat:  return TDFType::Float;
            case TDFType::ArrString: return TDFType::String;
            default: return TDFType::Null;
        }
    }

    static size_t ElementSize([[maybe_unused]] TDFType arrType, TDFType elemType) {
        switch (elemType) {
            case TDFType::Bool:   return 1;
            case TDFType::Int32:
            case TDFType::UInt32:
            case TDFType::Float:  return 4;
            case TDFType::String: return 1;
            default: return 0;
        }
    }
};