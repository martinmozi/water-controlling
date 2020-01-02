#include "sqlitedb.h"

//// weird postgres boolean handling has to be specialized
//template<> void Db::DbRow::value<bool>(int index, bool & t) const
//{
//	checkIndex(index);
//	t = (rowData_.at(index) == "t");
//}
namespace Db
{
}

namespace DbImpl
{
	SqliteConnection::SqliteConnection(const std::string & connectionString)
	:   connectionString_(connectionString),
		conn_(nullptr)
	{
	}

	void SqliteConnection::connect()
	{
    	if (conn_ != nullptr)
        	return;

        int rc = sqlite3_open(connectionString_.c_str(), &conn_);
        if (rc) 
        {
          	std::string err = std::string("Can't open database: ") + sqlite3_errmsg(conn_);
          	sqlite3_close(conn_);
            conn_ = nullptr;
          	throw std::runtime_error(err);
        }
	}

	void SqliteConnection::disconnect()
	{
		if (conn_)
		{
    		sqlite3_close(conn_);
			conn_ = nullptr;
		}
	}

	sqlite3* SqliteConnection::connection() const
	{
		return conn_;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SqliteStatementBase::SqliteStatementBase(sqlite3 * conn)
	:	conn_(conn)
	{
	}

	namespace
	{
		template <class T> T swapEndian(T u)
		{
			union
			{
				T u;
				unsigned char u8[sizeof(T)];
			} source, dest;

			source.u = u;

			for (size_t k = 0; k < sizeof(T); k++)
				dest.u8[k] = source.u8[sizeof(T) - k - 1];

			return dest.u;
		}
	}

    sqlite3_stmt* SqliteStatementBase::_exec()
	{
    	int rc = 0;
    	std::string queryStr(DbImpl::StatementPrivate::query());
		const std::vector<DbImpl::StatementPrivate::Data> & data = DbImpl::StatementPrivate::data();

		int index = 1;
		for (const DbImpl::StatementPrivate::Data & _data : data)
			replaceBindedParameter(_data.name, _data.type == DataType::Null, index);

    	sqlite3_stmt* stmt = nullptr;
    	queryStr = DbImpl::StatementPrivate::query();
    	if (sqlite3_prepare(conn_, queryStr.c_str(), queryStr.size(), &stmt, nullptr) != SQLITE_OK)
    	{
        	sqlite3_reset(stmt);
        	stmt = nullptr;
        	throw std::runtime_error(std::string("Unable to prepare statement: ") + sqlite3_errmsg(conn_));
    	}

        index = 1;
		for (const DbImpl::StatementPrivate::Data & _data : data)
		{
			switch (_data.type)
			{
			case DataType::Null:
				break;

			case DataType::Bool:
    			rc = sqlite3_bind_int(stmt, index, std::any_cast<bool>(_data.value) ? 1 : 0);
				break;

			case DataType::Int:
    			rc = sqlite3_bind_int(stmt, index, std::any_cast<int>(_data.value));
				break;

			case DataType::BigInt:
    			rc = sqlite3_bind_int64(stmt, index, std::any_cast<int64_t>(_data.value));
				break;

			case DataType::Double:
    			rc = sqlite3_bind_double(stmt, index, std::any_cast<double>(_data.value));
				break;

			case DataType::String:
    			{
        			std::string str(std::any_cast<std::string>(_data.value));
        			rc = sqlite3_bind_text(stmt, index, str.c_str(), str.size(), SQLITE_STATIC);
    			}
				break;

			case DataType::Blob:
    			{
        			std::string str(std::any_cast<std::string>(_data.value));
        			rc = sqlite3_bind_blob(stmt, index, str.c_str(), str.size(), SQLITE_STATIC);
    			}
			    break;

			default:
				break;
				
			}

    		if (rc != SQLITE_OK)
    		{
        		sqlite3_clear_bindings(stmt);
        		sqlite3_reset(stmt);
        		stmt = nullptr;
        		throw std::runtime_error(std::string("Unable to bind statement: ") + sqlite3_errmsg(conn_));
    		}
            index ++;
		}

    	return stmt;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SqliteStatement::SqliteStatement(sqlite3 * conn)
	:   SqliteStatementBase(conn),
		bRunningTransaction_(true)
	{
    	int rc = sqlite3_exec(conn_, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    	if (rc) 
    	{
        	std::string errorStr = std::string("Can't start transaction: ") + sqlite3_errmsg(conn_);
        	throw std::runtime_error(errorStr);
    	}
	}

    int64_t SqliteStatement::lastInsertedId() const
    {
        return sqlite3_last_insert_rowid(conn_);
    }

	void SqliteStatement::exec()
	{
    	sqlite3_stmt *stmt = _exec();
    	if (sqlite3_step(stmt) != SQLITE_DONE)
    	{
        	sqlite3_clear_bindings(stmt);
        	sqlite3_reset(stmt);
        	throw std::runtime_error(std::string("Unable execute statement step: ") + sqlite3_errmsg(conn_));
    	}
    	
    	sqlite3_clear_bindings(stmt);
    	sqlite3_reset(stmt);
	}

	void SqliteStatement::commit()
	{
    	if (bRunningTransaction_)
    	{
        	int rc = sqlite3_exec(conn_, "COMMIT;", NULL, NULL, NULL);
        	if (rc) 
        	{
            	std::string errorStr = std::string("Can't commit: ") + sqlite3_errmsg(conn_);
            	throw std::runtime_error(errorStr);
        	}
    	}
		bRunningTransaction_ = false;
	}

	void SqliteStatement::rollback()
	{
		if (bRunningTransaction_)
		{
    		int rc = sqlite3_exec(conn_, "ROLLBACK;", NULL, NULL, NULL);
    		if (rc) 
    		{
        		std::string errorStr = std::string("Can't rollback: ") + sqlite3_errmsg(conn_);
        		throw std::runtime_error(errorStr);
    		}
		}

		bRunningTransaction_ = false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SqliteSelector::SqliteSelector(sqlite3 *conn, bool isSingle)
	:   SqliteStatementBase(conn),
		isSingle_(isSingle)
	{
	}

	void SqliteSelector::select(std::function<void(const Db::DbRow & dbRow)> selectFunction)
	{
    	sqlite3_stmt *stmt = _exec();
    	while (sqlite3_step(stmt) == SQLITE_ROW)
    	{
        	int nFields = sqlite3_column_count(stmt);
        	Db::DbRow rowData;
        	for (int i = 0; i < nFields; i++)
        	{
            	switch (sqlite3_column_type(stmt, i))
                {
                case SQLITE_INTEGER:
                    rowData.append(sqlite3_column_int64(stmt, i));
                    break;

                case SQLITE_FLOAT:
                    rowData.append(sqlite3_column_double(stmt, i));
                    break;

                case SQLITE_TEXT:
                    rowData.append(std::string((const char*)sqlite3_column_text(stmt, i)));
                    break;

                case SQLITE_BLOB:
                    rowData.append(std::string((const char*)sqlite3_column_blob(stmt, i), sqlite3_column_bytes(stmt, i)));
                    break;

                default:
                    rowData.appendNull();
                    break;
                }

            	if (isSingle_)
            	{
                	if (i == 0)
                	{
                    	sqlite3_clear_bindings(stmt);
                    	sqlite3_reset(stmt);
                    	throw std::runtime_error("No data found in select");
                	}
                	else if (i > 1)
                	{
                    	sqlite3_clear_bindings(stmt);
                    	sqlite3_reset(stmt);
                    	throw std::runtime_error("Multiply rows for single selection were selected");
                	}
            	}

            	selectFunction(rowData);
        	}
    	}
    	
    	sqlite3_clear_bindings(stmt);
    	sqlite3_reset(stmt);
	}
}
