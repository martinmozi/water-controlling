#ifndef __dbConnection_H
#define __dbConnection_H

#include "sqlitedb.h"

namespace Db
{
	class Connection : public DbImpl::SqliteConnection
	{
	public:
		Connection(const std::string & connectionString) : DbImpl::SqliteConnection(connectionString) {}
		virtual ~Connection() { disconnect(); }
	};

	class Statement : public DbImpl::SqliteStatement
	{
	public:
		Statement(const Connection & connection) : DbImpl::SqliteStatement(connection.connection()) {}
		virtual ~Statement() { rollback(); }
	};

    class Selector : public DbImpl::SqliteSelector
	{
	public:
		Selector(const Connection & connection) : DbImpl::SqliteSelector(connection.connection(), false) {}
	};

    class SingleSelector : public DbImpl::SqliteSelector
	{
	public:
		SingleSelector(const Connection & connection) : DbImpl::SqliteSelector(connection.connection(), true) {}
	};
}

#endif
