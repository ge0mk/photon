#pragma once

#include "sqlite3.h"
#include <vector>
#include <string>
#include <array>
#include <variant>
#include <cassert>
#include <cstring>

namespace sqlite{
	typedef std::variant<std::monostate, int, double, std::string, std::vector<unsigned char>> variant;

	class database;

	class stmt{
	public:
		stmt(std::string querry, database *db, std::vector<variant> values);
		~stmt();

		void setArgs(std::vector<variant> values);
		inline void reset(){
			sqlite3_reset(handle);
			m_status = SQLITE_ROW;
		}

		template<int n = 0>
		std::array<variant, n> next(){
			m_status = sqlite3_step(handle);
			std::string error = sqlite3_errmsg(this->db);
			if(error != "not an error" && error != "another row available" && error != "column index out of range"){
			}
			std::array<variant, n> row;
			if(sqlite3_column_count(handle) == n){
				for(int i = 0; i < n; i++){
					int type = sqlite3_column_type(handle, i);
					switch(type){
						case SQLITE_INTEGER:{
							row[i] = sqlite3_column_int(handle, i);
						} break;
						case SQLITE_FLOAT:{
							row[i] = sqlite3_column_double(handle, i);
						} break;
						case SQLITE_TEXT:{
							row[i] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(handle, i)));
						} break;
						case SQLITE_BLOB:{
							const void *raw = sqlite3_column_blob(handle, i);
							int size = sqlite3_column_bytes(handle, i);
							std::vector<unsigned char> data(size, '\0');
							memcpy(&data[0], raw, size);
							row[i] = data;
						} break;
						case SQLITE_NULL:{
							row[i] = std::monostate();
						} break;
						default: {
						}
					}
				}
			}
			else{
			}
			error = sqlite3_errmsg(db);
			if(error != "not an error" && error != "another row available" && error != "column index out of range"){
			}
			return row;
		}

		inline int status(){
			return m_status;
		}
	private:
		sqlite3_stmt *handle = nullptr;
		sqlite3* db;
		int m_status = SQLITE_ROW;
	};

	template<>
	std::array<variant, 0> stmt::next<0>(){
		m_status = sqlite3_step(handle);
		std::string error = sqlite3_errmsg(db);
		if(error != "not an error" && error != "another row available" && error != "column index out of range"){
		}
		return {};
	}

	class database{
	public:
		database(std::string fileName);
		~database();

		template<int n = 0>
		std::vector<std::array<variant, n>> exec(std::string querry, std::vector<variant> values = {}, int *changes = nullptr){
			stmt s(querry, this, values);
			std::vector<std::array<variant, n>> result;
			std::array<variant, n> row = s.next<n>();
			while(s.status() != SQLITE_DONE){
				result.push_back(row);
				row = s.next<n>();
				assert(s.status() != SQLITE_ERROR);
			}
			if(changes)
				*changes = sqlite3_changes(handle);
			return result;
		}

	private:
		sqlite3 *handle = nullptr;
		friend class stmt;
	};

	template<>
	std::vector<std::array<variant, 0>> database::exec<0>(std::string querry, std::vector<variant> values, int *changes){
		stmt s(querry, this, values);
		while(s.status() != SQLITE_DONE){
			s.next<0>();
			assert(s.status() != SQLITE_ERROR);
		}
		if(changes)
			*changes = sqlite3_changes(handle);
		return {};
	}
}
