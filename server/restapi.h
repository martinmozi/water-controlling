#pragma once

#include "restapi_impl.h"
#include "jsonVariant.hpp"
#include <functional>

class RestApi
{
public:
    RestApi(const std::string & configPath, const std::string & dbPath);
    std::string execute(const std::string & url, const std::string & payload);

private:
    JsonSerialization::Variant getTime(const JsonSerialization::Variant &);
    JsonSerialization::Variant setTime(const JsonSerialization::Variant & request);
    JsonSerialization::Variant resultVariant(JsonSerialization::VariantMap * pVariantMap);
    JsonSerialization::Variant intervalList(const JsonSerialization::Variant & request);
    JsonSerialization::Variant addInterval(const JsonSerialization::Variant & request);
    JsonSerialization::Variant modifyInterval(const JsonSerialization::Variant & request);
    JsonSerialization::Variant removeInterval(const JsonSerialization::Variant & request);
    JsonSerialization::Variant getGpio(const JsonSerialization::Variant & request);
    JsonSerialization::Variant setGpio(const JsonSerialization::Variant & request);


    JsonSerialization::Variant errorResponse(const std::string errorStr);
    Interval readIntervalVariant(const JsonSerialization::VariantMap & variantMap) const;

private:
    RestApiImpl restApiImpl_;
    std::map<std::string, std::pair<std::string, std::function<JsonSerialization::Variant(const JsonSerialization::Variant&)>>> apiSchemaMap_;
};