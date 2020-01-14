#include "restapi_impl.h"

RestApiImpl::RestApiImpl(const std::string & configPath, const std::string & dbPath)
:   gpioConfig_(configPath),
    db_(dbPath)
{
    db_.connect();
}

void RestApiImpl::setTime(std::time_t time)
{
    if (time != (time_t)-1)
        stime(&time);
}
std::time_t RestApiImpl::getTime() const
{
    time_t rawtime;
    time(&rawtime);
    return rawtime;
}
std::vector<Interval> RestApiImpl::intervalList() const
{
    std::vector<Interval> intervalVector;
    Db::Selector dbSelector(db_);
    std::string query = "SELECT id, name, _from, _to, days, active FROM interval ";

    dbSelector.prepare(query);
    dbSelector.select([&intervalVector](const Db::DbRow & row)
    {
        int days;
        Interval interval;
        row.value(0, interval.id);
        row.value(1, interval.name);
        row.value(2, interval.from);
        row.value(3, interval.to);
        row.value(4, days);
        interval.days = (char)days;
        row.value(5, interval.active);

        intervalVector.push_back(interval);
    });

    return intervalVector;
}
void RestApiImpl::addInterval(const Interval & interval, int64_t & id)
{
    Db::Statement dbStatement(db_);
    std::string query = "INSERT INTO interval (name, _from, _to, days, active) VALUES (:name, :from, :to, :days, :active)";
    dbStatement.prepare(query);
    dbStatement.bind("name", interval.name);
    dbStatement.bind("from", (int64_t)interval.from);
    dbStatement.bind("to", (int64_t)interval.to);
    dbStatement.bind("days", interval.days);
    dbStatement.bind("active", interval.active);
    dbStatement.exec();
    id = dbStatement.lastInsertedId();
    dbStatement.commit();
}

void RestApiImpl::modifyInterval(const Interval & interval)
{
    Db::Statement dbStatement(db_);
    std::string query = "UPDATE interval SET name=:name, _from=:from, _to=:to, days=:days, active=:active WHERE id=:id";
    dbStatement.prepare(query);
    dbStatement.bind("name", interval.name);
    dbStatement.bind("from", (int64_t)interval.from);
    dbStatement.bind("to", (int64_t)interval.to);
    dbStatement.bind("days", interval.days);
    dbStatement.bind("active", interval.active);
    dbStatement.bind("id", interval.id);
    dbStatement.exec();
    dbStatement.commit();
}

void RestApiImpl::removeInterval(const int64_t & id)
{
    Db::Statement dbStatement(db_);
    std::string query = "DELETE FROM interval WHERE id=:id";
    dbStatement.prepare(query);
    dbStatement.bind("id", id);
    dbStatement.exec();
    dbStatement.commit();
}

GpioStatus RestApiImpl::getGpioStatus(int index)
{
	Db::SingleSelector dbSelector(db_);
	std::string query = "SELECT status FROM gpiostatus WHERE gpioindex=:gpioindex";

	GpioStatus gpioStatus = GpioStatus::NoType;
    dbSelector.prepare(query);
	dbSelector.bind("gpioindex", index);
	dbSelector.select([&gpioStatus](const Db::DbRow & row)
	{
		int status;
		row.value(0, status);
		gpioStatus = (GpioStatus)status;
	});
	
	return gpioStatus;
}

void RestApiImpl::setGpioStatus(int index, GpioStatus gpioStatus)
{
	std::string query;
	if (getGpioStatus(index) == GpioStatus::NoType)
		query = "INSERT INTO gpiostatus (gpioindex, status) VALUES(:gpioindex, :status)";
	else
		query = "UPDATE gpiostatus SET status=:status WHERE gpioindex=:gpioindex";

	Db::Statement dbStatement(db_);
	dbStatement.prepare(query);
	dbStatement.bind("gpioindex", index);
	dbStatement.bind("status", (int)gpioStatus);
	dbStatement.exec();
	waterManager_.setGpioStatus(index, gpioStatus);
	dbStatement.commit();
}
