#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <mysql.h>
#pragma comment(lib, "libmySQL.lib")


class Database
{
	public:
		void Connect();
};

#endif