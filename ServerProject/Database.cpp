#include "Database.h"
#include <iostream>

using namespace std;

void Database::Connect() {
	MYSQL mysql;
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, "localhost", "root", "0000", "mysql", 3306, NULL, 0))
		cout << "error";
	else
		cout << "success";
	mysql_close(&mysql);
}