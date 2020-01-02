#include <iostream>
#include <ctime>
#include <memory>
#include <algorithm>
#include "service.hpp"
#include "restapi.h"
#include "log.h"

#include <restapi-server/IRestApiServer.h>

class ZavlahaService : public Service
{
private:
    std::string configPath_;
    std::string dbPath_;
public:
    ZavlahaService(const std::vector<std::string> &args)
    {
        parseArgs(args);
    }
protected:
    int run() override
    {
        RestApi restApi(configPath_, dbPath_);
        auto iRestApiServer = libRestApi::createRestApiServer(8080, "/api");
        iRestApiServer->start([&restApi](libRestApi::HttpMethod method, const std::string & url, const std::string & request)->std::string
        {
            return restApi.execute(url, request);
        });

//        std::string jsonString;
//        try
//        {
//            // jsonString = restApi.execute("interval/list", "");
//            // jsonString = restApi.execute("interval/add", "{\"name\" :\"name\", \"from\": 1558945464173, \"to\": 1558945498566, \"days\": 4, \"active\":true}");
//            jsonString = restApi.execute("interval/modify", "{\"id\":1, \"name\" :\"first\", \"from\": 1558945464152, \"to\": 1558945498568, \"days\": 8, \"active\":true}");
//        }
//        catch(const std::exception& ex)
//        {
//            jsonString = ex.what();
//        }

        return 0;
    }
private:
    void removeFromMandatorySet(std::set<std::string>& mandatorySet, const std::string & value) const
    {
        auto it = mandatorySet.find(value);
        if (it != mandatorySet.end())
            mandatorySet.erase(it);
    }
    void parseArgs(const std::vector<std::string> &args)
    {
        // z tohoto budem mazat
        std::set<std::string> mandatoryArgs{ "--config", "--logPath", "--dbPath" };
        std::map<std::string, std::function<void(const std::string &)>> pairArgs = 
        { 
            { "--config",[this](const std::string & arg) { configPath_ = arg; } },
            { "--logPath",[](const std::string & arg) { Log::logPath = arg; } },
            { "--logPath", [this](const std::string & arg) { dbPath_ = arg; } }
        };

        std::map<std::string, std::function<void(void)>> singleArgs = 
        { 
            { "-c", [this](){ setConsoleMode(true); } }
        };

        bool waitPair = false;
        std::map<std::string, std::function<void(const std::string &)>>::const_iterator itOld;
        for(const std::string & arg : args)
        {
            const auto it = pairArgs.find(arg);
            if (it != pairArgs.end())
            {
                waitPair = true;
                itOld = it;
                continue;
            }

            const auto iter = singleArgs.find(arg);
            if (iter != singleArgs.end())
            {
                if (waitPair)
                {
                    Log::error("Bad single commandline argument %s when expecting pair argument", iter->first.c_str());
                    exit(1);
                }
                else
                {
                    iter->second();
                    removeFromMandatorySet(mandatoryArgs, iter->first);
                }
            }
            else
            {
                if (waitPair)
                {
                    itOld->second(arg);
                    removeFromMandatorySet(mandatoryArgs, itOld->first);
                    waitPair = false;
                }
                else
                {
                    Log::error("Unknown single commandline argument %s", it->first.c_str());
                    exit(1);
                }
            }
        }

        if (!mandatoryArgs.empty())
        {
            std::string mandatoryArgString;
            for (const auto& it : mandatoryArgs)
                mandatoryArgString += std::string(" ") + it;

            Log::error("Following arguments are mandatory: %s", mandatoryArgString.c_str());
            exit(1);
        }
    }
};

int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);

    ZavlahaService zavlahaService(args);
    return zavlahaService.start();
}

// api methods

/*
/api/time/get
	{
		"hour": 22,
		"minute": 19,
		"second": 2
        "days: [1.....7]
	}
/api/time/set
	{
		"hour": 22,
		"minute": 19,
		"second": 2
	}

/api/interval/get
	{
		"gpioNumber": 1,
		"interval" :
		[
			{
                "id" :
				"from" :
				{
					"hour": 22,
					"minute": 19,
					"second": 2
				},
				"to" :
				{
				"hour": 23,
				"minute": 20,
				"second": 2
				}
			}
		]
	}

/api/interval/set

/api/gpio/enable
	{
		"gpioNumber": 1,
		"enable": true
	}

*/