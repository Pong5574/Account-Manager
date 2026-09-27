#include <iostream>
#include <string>

#include "sqlite3.h"

struct acc_data {
    std::string username;
    std::string password;
    std::string service;
};

int main () {
    sqlite3* db;

    const int result = sqlite3_open("Database.db", &db);
    if (result != SQLITE_OK) {
        std::cerr << "Open database failed\n";
        return 1;
    }

    const char* sql_crt_tbl = "CREATE TABLE IF NOT EXISTS Account (ID INTEGER PRIMARY KEY AUTOINCREMENT, Username TEXT, Password TEXT, Service TEXT)";
    char* error;

    const int exec_result = sqlite3_exec(db, sql_crt_tbl, nullptr, nullptr, &error);
    if (exec_result != SQLITE_OK) {
        std::cerr << '(' << error << ')' << " Creation table failed\n";
        sqlite3_free(error);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_free(error);

    while (true) {
        std::string input;

        std::cout << ">";
        std::getline(std::cin, input);

        if (input == "crt acc") {
            acc_data new_acc;

            std::cout << "Enter Account[Service]>";
            std::cin >> new_acc.service;

            std::cout << "Enter Account[Username]>";
            std::cin >> new_acc.username;

            std::cout << "Enter Account[Password]>";
            std::cin >> new_acc.password;

            const char* sql_insert = "INSERT INTO Account (Username , Password, Service) VALUES (?, ?, ?);";
            sqlite3_stmt* stmt;

            sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr);

            sqlite3_bind_text(stmt, 1, new_acc.username.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, new_acc.password.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, new_acc.service.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Insert failed\n";
            }

            sqlite3_finalize(stmt);
        } else if (input == "del acc") {
            int number = 0;

            std::cout << '>';
            std::cin >> number;

            const char* sql_select = "SELECT Username, Password, Service FROM Account WHERE ID = ?;";
            sqlite3_stmt* stmt;

            sqlite3_prepare_v2(db, sql_select, -1, &stmt, nullptr);

            sqlite3_bind_int(stmt, 1, number);

            if (sqlite3_step(stmt) != SQLITE_ROW) {
                std::cerr << "Select ID = " << number << " failed\n";

                sqlite3_finalize(stmt);

                continue;
            }

            const char* username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* service = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            std::cout << "Delete [" << service << "][" << username << "][" << password << "] Y/n?\n";

            sqlite3_finalize(stmt);

            char values;

            std::cout << '>';
            std::cin >> values;

            const char* sql_delete = "DELETE FROM Account WHERE ID = ?";

            if (values == 'Y' || values == 'y') {
                sqlite3_prepare_v2(db, sql_delete, -1, &stmt, nullptr);

                sqlite3_bind_int(stmt, 1, number);

                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    std::cerr << "Delete ID = " << number << " failed\n";
                }

                sqlite3_finalize(stmt);
            }
        } else if (input == "show all acc") {
            const char* sql_all = "SELECT ID, Username, Password, Service FROM Account;";

            sqlite3_stmt* stmt;

            sqlite3_prepare_v2(db, sql_all, -1, &stmt, nullptr);

            int count = 0;

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int ID = sqlite3_column_int(stmt, 0);

                const char* username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                const char* password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                const char* service = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

                std::cout << "ID = " << ID << " | Username = " << username << " | Password = " << password << " | Service = " << service << '\n';

                count++;
            }

            if (count == 0) {
                std::cout << "Table empty\n";
            }

            sqlite3_finalize(stmt);
        }
    }

    sqlite3_close(db);

    return 0;
}