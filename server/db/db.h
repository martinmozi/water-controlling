#ifndef __DB_CONNECTOR
#define __DB_CONNECTOR

#include <vector>
#include <string>
#include <functional>
#if __cplusplus > 201402L
#include <any>
#include <optional>
#else
#include <boost/any.hpp>
#include <boost/optional.hpp>
namespace std
{
    template<typename T> T any_cast(const boost::any & operand)
    {
        return boost::any_cast<T>(operand);
    }

    using any = boost::any;
    
    template<typename T> using optional = boost::optional<T>;
}
#endif

namespace Db
{
	class DbRow
	{
		std::vector<std::any> rowData_;
		std::vector<bool> nullData_;

	public:
		void append(const std::any & value);
		void appendNull();

	public:
		DbRow();
		bool isNull(int index) const;
		void blobValue(int, std::string & ) const;
		void blobValue(int, std::string &, const std::string &) const;

		template<typename T> void value(int index, T & t, T nullValue) const
		{
			if (isNull(index))
				t = nullValue;
			else
				value(index, t);
		}

    	template<typename T> void value(int index, T & t) const
    	{
        	checkIndex(index);
        	t = std::any_cast<T>(rowData_.at(index));
    	}

	private:
		void checkIndex(int index) const;
	};
}

namespace DbImpl
{
	enum DataType
	{
		Null,
		Bool,
		BigInt,
		Int,
		Double,
		String,
		Blob,
	};

	class ConnectionPrivate
	{
	public:
		virtual void connect() = 0;
		virtual void disconnect() = 0;
	};

	class TransactionPrivate
	{
	public:
		virtual void exec() = 0;
		virtual void commit() = 0;
		virtual void rollback() = 0;
	};

	class SelectorPrivate
	{
	public:
		virtual void select(std::function<void(const Db::DbRow & dbRow)> selectFunction) = 0;
	};

	class StatementPrivate
	{
	protected:
		struct Data
		{
			std::string name;
			std::any value;
			DataType type;
		};

	protected:
		void replaceBindedParameter(const std::string & bindedName, bool isNull, int & index);
		const char * query() const;
		const std::vector<Data> & data() const;

	public:
		StatementPrivate() {}
		virtual ~StatementPrivate() {}

		void prepare(const std::string & query);
		void append(const std::string & appendQuery);
		void bind(const std::string & key, int32_t value, std::optional<int32_t> nullValue = {});
		void bind(const std::string & key, int64_t value, std::optional<int64_t> nullValue = {});
		void bind(const std::string & key, bool value, std::optional<bool> nullValue = {});
		void bind(const std::string & key, double value, std::optional<double> nullValue = {});
		void bind(const std::string & key, const std::string & value, std::optional<std::string> nullValue = {});
		void bindBlob(const std::string & key, const std::string & value, std::optional<std::string> nullValue = {});
		void bindAndAppend(const std::string & appendedQuery, const std::string & key, int32_t value, std::optional<int> nullValue = {});
		void bindAndAppend(const std::string & appendedQuery, const std::string & key, int64_t value, std::optional<int64_t> nullValue = {});
		void bindAndAppend(const std::string & appendedQuery, const std::string & key, bool value, std::optional<bool> nullValue = {});
		void bindAndAppend(const std::string & appendedQuery, const std::string & key, double value, std::optional<double> nullValue = {});
		void bindAndAppend(const std::string & appendedQuery, const std::string & key, const std::string & value, std::optional<std::string> nullValue = {});
		void bindBlobAndAppend(const std::string & appendedQuery, const std::string & key, const std::string & value, std::optional<std::string> nullValue = {});

	private:
		template<class T> void bindData(const std::string & key, const T & value, std::optional<T> nullValue, DataType type)
		{
			Data data;
			data.name = key;
			if (nullValue && *nullValue == value)
			{
				//tuto to nefunguje
//				2018-04-08 16:40:09 CEST ERROR:  bind message supplies 3 parameters, but prepared statement "" requires 4
//				2018-04-08 16:40:09 CEST STATEMENT:  INSERT INTO draft_participants(id, draftid, addressid, address, type) VALUES(nextval('seq_draft_participants') , $1, $4, $2, $3)

				data.type = DataType::Null;
			}
			else
			{
				data.type = type;
				data.value = value;
			}

			data_.push_back(data);
		}

	private:
		std::string query_;
		std::vector<Data> data_;

	};
}

#endif

