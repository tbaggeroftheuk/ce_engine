#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

enum class TDFType : uint8_t {
    Bool, Int32, UInt32, Float, String, Null,
    ArrBool, ArrInt32, ArrUInt32, ArrFloat, ArrString
};

struct TDFValue {
    TDFType type;
    std::vector<uint8_t> data;
};

class TDFFile {
public:
    std::unordered_map<std::string, TDFValue> entries;

    // --- File IO ---
    void save(const std::string& path, uint8_t version = 0x10) const;
    void load(const std::string& path);

    // --- Access / Modify ---
    void set(const std::string& key, const TDFValue& val);
    bool remove(const std::string& key);
    bool has(const std::string& key) const;

    void appendToArray(const std::string& key, const TDFValue& val);
    void deleteFromArray(const std::string& key, size_t indexToRemove);

    // --- Static constructors ---
    static TDFValue makeNull();
    static TDFValue makeBool(bool v);
    static TDFValue makeInt(int32_t v);
    static TDFValue makeUInt(uint32_t v);
    static TDFValue makeFloat(float v);
    static TDFValue makeString(const std::string& s);

    static TDFValue makeBoolArray(const std::vector<bool>& arr);
    static TDFValue makeIntArray(const std::vector<int32_t>& arr);
    static TDFValue makeUIntArray(const std::vector<uint32_t>& arr);
    static TDFValue makeFloatArray(const std::vector<float>& arr);
    static TDFValue makeStringArray(const std::vector<std::string>& arr);

private:
    static void readValue(std::ifstream& in, TDFValue& v);
    static bool isArray(TDFType t);
    static TDFType elementType(TDFType arrType);
    static size_t elementSize(TDFType arrType, TDFType elemType);
};
