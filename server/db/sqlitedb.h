#ifndef __SQLITEDB_CONNECTOR
#define __SQLITEDB_CONNECTOR

#include "db.h"
#include "sqlite3.h"

namespace DbImpl
{
	class SqliteConnection : public ConnectionPrivate
	{
	public:
		SqliteConnection(const std::string & connectionString);
		void connect() override;
		void disconnect() override;
		sqlite3* connection() const;

	private:
		std::string connectionString_;
		sqlite3 *conn_;
	};

	class SqliteStatementBase : public StatementPrivate
	{
	public:
		SqliteStatementBase(sqlite3 *conn);

	protected:
    	sqlite3_stmt* _exec();

	protected:
		sqlite3 *conn_;
	};

	class SqliteStatement : public SqliteStatementBase, public TransactionPrivate
	{
	public:
    	SqliteStatement(sqlite3 *conn);
    	int64_t lastInsertedId() const;
		void exec() override;
		void commit() override;
		void rollback() override;

	private:
		bool bRunningTransaction_;
	};

	class SqliteSelector : public SqliteStatementBase, SelectorPrivate
	{
	public:
		SqliteSelector(sqlite3 *conn, bool isSingle);

		void select(std::function<void(const Db::DbRow & dbRow)> selectFunction) override;

	private: 
		bool isSingle_;
	};
}

#endif



