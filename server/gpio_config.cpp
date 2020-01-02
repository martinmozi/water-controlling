#include "gpio_config.h"
#include "jsonVariant.hpp"
#include "log.h"

#include <fstream>

namespace
{
    std::string jsonSchema = R"(
    {
        "type": "array",
        "items":     
        {
            "type": "object",
            "properties": 
            {
                "index": { "type": "number", "minimum": 0},
                "name": { "type": "string" }
            },
            "required": [ "index", "name" ]
        }
    })";
}

GpioConfig::GpioConfig(const std::string & configPath)
{
    init(configPath);
}

void GpioConfig::init(const std::string & configPath)
{
    std::ifstream t(configPath);
    std::string jsonStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    std::string errorStr;
    JsonSerialization::Variant variant;
    if (!JsonSerialization::Variant::fromJson(jsonStr, jsonSchema, variant, &errorStr))
    {
        Log::error("Unable to parse gpio config with error: %s", errorStr.c_str());
        exit(1);
    }

    const JsonSerialization::VariantVector & variantVector = variant.toVector();
    for (const JsonSerialization::Variant & v : variantVector)
    {
        const JsonSerialization::VariantMap & variantMap = v.toMap();
        gpioData_.insert(std::make_pair(variantMap.at("index").toInt64(), variantMap.at("name").toString()));
    }
}

const std::map<int64_t, std::string>& GpioConfig::gpioData() const
{
    return gpioData_;
}