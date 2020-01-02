#include <time.h> 
#include <regex>
#include <sstream>
#include "db.h"

Db::DbRow::DbRow()
{
}

void Db::DbRow::append(const std::any & value)
{
	rowData_.push_back(value);
	nullData_.push_back(bool(false));
}

void Db::DbRow::appendNull()
{
	rowData_.push_back(std::string());
	nullData_.push_back(bool(true));
}

bool Db::DbRow::isNull(int index) const
{
	checkIndex(index);
	bool nullValue = nullData_.at(index);
	return nullValue;
}

void Db::DbRow::checkIndex(int index) const
{
	if (index > (int)rowData_.size() || index > (int)nullData_.size())
		throw std::runtime_error("Selector index is out of the row size");
}

void Db::DbRow::blobValue(int , std::string & ) const
{
	assert(0);
}

void Db::DbRow::blobValue(int , std::string & , const std::string &) const
{
	assert(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char * DbImpl::StatementPrivate::query() const
{
	return query_.c_str();
}

const std::vector<DbImpl::StatementPrivate::Data> & DbImpl::StatementPrivate::data() const
{
	return data_;
}

void DbImpl::StatementPrivate::replaceBindedParameter(const std::string & bindedName, bool isNull, int & index)
{
	size_t sz = 0;
	std::string bindName(":" + bindedName);
	bool bFound = false;
	while (true)
	{
    	std::string bindValue = isNull ? "NULL" : "?";//: ("$" + std::to_string(index));
		sz = query_.find(bindName, sz);
		if (sz == std::string::npos)
			break;

		// check whole word
		if (query_.size() > sz + bindName.size())
		{
			char ch = query_.at(sz + bindName.size());
			if (std::isalpha(ch) || std::isdigit(ch) || ch == '_')
			{
				sz += bindName.size();
				continue;
			}
		}

		bFound = true;
		query_.replace(sz, bindName.length(), bindValue);
		sz += bindValue.size();
	}

	if (!bFound)
		throw std::runtime_error("Unknown binded parameter " + bindedName);

	if (!isNull)
		index++;
}

void DbImpl::StatementPrivate::prepare(const std::string & query)
{
	query_ = query;
	data_.clear();
}

void DbImpl::StatementPrivate::append(const std::string & appendQuery)
{
	query_ += appendQuery;
}

void DbImpl::StatementPrivate::bind(const std::string & key, int32_t value, std::optional<int32_t> nullValue)
{
	bindData(key, value, nullValue, DataType::Int);
}

void DbImpl::StatementPrivate::bind(const std::string & key, int64_t value, std::optional<int64_t> nullValue)
{
	bindData(key, value, nullValue, DataType::BigInt);
}

void DbImpl::StatementPrivate::bind(const std::string & key, bool value, std::optional<bool> nullValue)
{
	bindData(key, value, nullValue, DataType::Bool);
}

void DbImpl::StatementPrivate::bind(const std::string & key, double value, std::optional<double> nullValue)
{
	bindData(key, value, nullValue, DataType::Double);
}

void DbImpl::StatementPrivate::bind(const std::string & key, const std::string & value, std::optional<std::string> nullValue)
{
	bindData(key, value, nullValue, DataType::String);
}

void DbImpl::StatementPrivate::bindBlob(const std::string & key, const std::string & value, std::optional<std::string> nullValue)
{
	bindData(key, value, nullValue, DataType::Blob);
}

void DbImpl::StatementPrivate::bindAndAppend(const std::string & appendedQuery, const std::string & key, int32_t value, std::optional<int> nullValue)
{
	query_ += appendedQuery;
	bind(key, value, nullValue);
}

void DbImpl::StatementPrivate::bindAndAppend(const std::string & appendedQuery, const std::string & key, int64_t value, std::optional<int64_t> nullValue)
{
	query_ += appendedQuery;
	bind(key, value, nullValue);
}

void DbImpl::StatementPrivate::bindAndAppend(const std::string & appendedQuery, const std::string & key, bool value, std::optional<bool> nullValue)
{
	query_ += appendedQuery;
	bind(key, value, nullValue);
}

void DbImpl::StatementPrivate::bindAndAppend(const std::string & appendedQuery, const std::string & key, double value, std::optional<double> nullValue)
{
	query_ += appendedQuery;
	bind(key, value, nullValue);
}

void DbImpl::StatementPrivate::bindAndAppend(const std::string & appendedQuery, const std::string & key, const std::string & value, std::optional<std::string> nullValue)
{
	query_ += appendedQuery;
	bind(key, value, nullValue);
}

void DbImpl::StatementPrivate::bindBlobAndAppend(const std::string & appendedQuery, const std::string & key, const std::string & value, std::optional<std::string> nullValue)
{
	query_ += appendedQuery;
	bind(key, value, nullValue);
}
