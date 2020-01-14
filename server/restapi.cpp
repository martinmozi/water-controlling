#include "restapi.h"
#include "interval.h"

namespace
{
    std::string timeSetSchema {R"(
    {
       "type": "object",
       "properties": 
       {
           "hour": { "type": "integer", "minimum" : 0, "maximum" : 23 },
           "minute": { "type": "integer", "minimum" : 0, "maximum" : 59 },
           "second": { "type": "integer", "minimum" : 0, "maximum" : 59 },
           "day": { "type": "integer", "minimum" : 1, "maximum" : 31 },
           "month": { "type": "integer", "minimum" : 1, "maximum" : 11 },
           "year": { "type": "integer", "minimum" : 2010, "maximum" : 2100 },
       },
       "required": [ "hour", "minute", "second", "day", "month", "year" ]
    })"};

    std::string intervalAddSchema {R"(
    {
       "type": "object",
       "properties": 
       {
           "name": { "type": "string" },
           "from": { "type": "integer" },
           "to": { "type": "integer" },
           "days": { "type": "integer", "minimum" : 0, "maximum" : 255 },
           "active": { "type": "boolean" },
           "gpioindex": { "type": "interger" }
       },
       "required": [ "name", "from", "to", "days", "active" ]
    })"};

    std::string intervalModifySchema {
    R"(
    {
       "type": "object",
       "properties": 
       {
           "id": { "type": "integer" },
           "name": { "type": "string" },
           "from": { "type": "integer" },
           "to": { "type": "integer" },
           "days": { "type": "integer", "minimum" : 0, "maximum" : 255 },
           "active": { "type": "boolean" },
           "gpioindex": { "type": "interger" }
       },
       "required": [ "id", "name", "from", "to", "days", "active" ]
    })"};

    std::string intervalRemoveSchema{
    R"(
    {
       "type": "object",
       "properties": 
       {
           "id": { "type": "integer" }
       },
       "required": [ "id" ]
    })"};
		    
	std::string getGpioSchema{
	R"(
	{
		"type": "object",
		"properties": 
		{
			"index": { "type": "integer" }
		},
		"required": [ "index" ]
	})"};
		    
	std::string setGpioSchema{
	R"(
	{
		"type": "object",
		"properties": 
		{
			"index": { "type": "integer" },
			"status": { "type": "integer" }
		},
		"required": [ "index", "status" ]
	})"};
}

RestApi::RestApi(const std::string & configPath, const std::string & dbPath)
:   restApiImpl_(configPath, dbPath),
    apiSchemaMap_
    {
        {"/time/get", {"", [this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return getTime(request); }}},
        {"/time/set", { timeSetSchema, [this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return setTime(request); }}},
        {"/interval/list", {"", [this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return intervalList(request); }}},
        {"/interval/add", {intervalAddSchema,[this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return addInterval(request); }}},
        {"/interval/modify", {intervalModifySchema,[this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return modifyInterval(request); }}},
        {"/interval/remove", {intervalRemoveSchema, [this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return removeInterval(request); }}},
        {"/gpio/get", {"", [this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return getGpio(request); }}},
        {"/gpio/set", {"", [this](const JsonSerialization::Variant & request)->JsonSerialization::Variant { return setGpio(request); }}}
    }
{
}

std::string RestApi::execute(const std::string & url, const std::string & payload)
{
    try
    {
        std::string outStr;
        const auto it = apiSchemaMap_.find(url);
        if (it == apiSchemaMap_.end())
            return errorResponse("Method is not supported").toJson();

        JsonSerialization::Variant request;
        if (!payload.empty())
        {
            if (!JsonSerialization::Variant::fromJson(payload, it->second.first, request, &outStr))
                return errorResponse(outStr).toJson();
        }

        JsonSerialization::Variant outVariant = it->second.second(request);
        return outVariant.toJson();
    }
    catch (const std::exception& ex)
    {
        return errorResponse(ex.what()).toJson();
    } 
}

JsonSerialization::Variant RestApi::errorResponse(const std::string errorStr)
{
    JsonSerialization::VariantMap variantMap
    {
        { "result", 1 },
        { "error", errorStr }
    };
    return JsonSerialization::Variant(variantMap);
}

JsonSerialization::Variant RestApi::resultVariant(JsonSerialization::VariantMap * pVariantMap)
{
    if (pVariantMap)
    {
        pVariantMap->insert(std::make_pair("result", 0));
        return JsonSerialization::Variant(*pVariantMap);
    }

    return JsonSerialization::Variant(JsonSerialization::VariantMap{{ "result", 0 }});
}

JsonSerialization::Variant RestApi::getTime(const JsonSerialization::Variant &)
{
    std::time_t ti = restApiImpl_.getTime();
    auto rawTime = restApiImpl_.getTime();
    struct tm * t = localtime(&rawTime);
    JsonSerialization::VariantMap variantMap
    {
        { "hour", t->tm_hour },
		{ "minute", t->tm_min },
		{ "second", t->tm_sec },
        { "day", t->tm_mday },
        { "month", t->tm_mon + 1 },
        { "year", t->tm_year + 1900 },
    };

    return resultVariant(&variantMap);
}

JsonSerialization::Variant RestApi::setTime(const JsonSerialization::Variant & request)
{
    struct tm t = { 0 };
    const JsonSerialization::VariantMap & variantMap = request.toMap();
    t.tm_hour = (int)variantMap.at("hour").toInt64();
    t.tm_min = (int)variantMap.at("minute").toInt64();
    t.tm_sec = (int)variantMap.at("second").toInt64();
    t.tm_mday = (int)variantMap.at("day").toInt64();
    t.tm_mon = (int)variantMap.at("month").toInt64() - 1;
    t.tm_year = (int)variantMap.at("year").toInt64() - 1900;
    return resultVariant(nullptr);
}

JsonSerialization::Variant RestApi::addInterval(const JsonSerialization::Variant & request)
{
    Interval interval(readIntervalVariant(request.toMap()));
    restApiImpl_.addInterval(interval, interval.id);
    JsonSerialization::VariantMap variantMap{{ "id", interval.id }};
    return resultVariant(&variantMap);
}

JsonSerialization::Variant RestApi::intervalList(const JsonSerialization::Variant & request)
{
    JsonSerialization::VariantVector variantVector;
    std::vector<Interval> intervalVector(restApiImpl_.intervalList());
    for (const Interval & interval : intervalVector)
    {
        JsonSerialization::VariantMap variantMap
        {
            { "id", interval.id },
	        { "name", interval.name },
            { "from", (int64_t)interval.from },
            { "to", (int64_t)interval.to },
            { "days", interval.days },
	        { "active", interval.active },
	        { "gpioIndex", interval.gpioIndex }
        };
    }

    JsonSerialization::VariantMap intervalsVariantMap;
    intervalsVariantMap["intervals"] = JsonSerialization::Variant(variantVector);
    return resultVariant(&intervalsVariantMap); 
}

JsonSerialization::Variant RestApi::modifyInterval(const JsonSerialization::Variant & request)
{
    Interval interval(readIntervalVariant(request.toMap()));
    restApiImpl_.modifyInterval(interval);
    return resultVariant(nullptr);
}

JsonSerialization::Variant RestApi::removeInterval(const JsonSerialization::Variant & request)
{
    int64_t id = request.toMap().at("id").toInt64();
    restApiImpl_.removeInterval(id);
    return resultVariant(nullptr);
}

JsonSerialization::Variant RestApi::getGpio(const JsonSerialization::Variant & request)
{
	int64_t index = request.toMap().at("index").toInt64();
	GpioStatus gpioStatus = restApiImpl_.getGpioStatus(index);
	JsonSerialization::VariantMap resultMap { {"status", (int64_t)gpioStatus}};
	return resultVariant(&resultMap);
}

JsonSerialization::Variant RestApi::setGpio(const JsonSerialization::Variant & request)
{
	int64_t index = request.toMap().at("index").toInt64();
	int64_t status = request.toMap().at("status").toInt64();
	GpioStatus gpioStatus;
	if (status > (int64_t)GpioStatus::NoType && status <= (int64_t)GpioStatus::ByInterval)
	    restApiImpl_.setGpioStatus(index, gpioStatus);
	else
	    throw std::runtime_error("Status is out of range");
	
	return resultVariant(nullptr);
}

Interval RestApi::readIntervalVariant(const JsonSerialization::VariantMap & variantMap) const
{
    Interval interval;
    interval.id = (variantMap.find("id") == variantMap.end()) ? -1 : variantMap.at("id").toInt64();
    interval.name = variantMap.at("name").toString();
    interval.from = variantMap.at("from").toInt64();
    interval.to = variantMap.at("to").toInt64();
    interval.days = (char)variantMap.at("days").toInt64();
    interval.active = variantMap.at("active").toBool();
    interval.gpioIndex = (variantMap.find("gpioindex") == variantMap.end()) ? -1 : variantMap.at("gpioindex").toInt64();

    return interval;
}
