#include <sqlite/sqlite.hpp>

namespace sqlite{
	stmt::stmt(std::string querry, database *db, std::vector<variant> values){
		this->db = db->handle;
		if(sqlite3_prepare_v2(this->db, querry.c_str(), -1, &handle, nullptr) == SQLITE_OK){
			setArgs(values);
		}
		else{
			std::string error = sqlite3_errmsg(this->db);
		}
	}
	stmt::~stmt(){
		sqlite3_finalize(handle);
	}
	void stmt::setArgs(std::vector<variant> values){
		reset();
		int i = 1;
		for(variant &value : values){
			int type = value.index();
			int r = SQLITE_OK;
			switch(type){
				case 1:{
					sqlite3_bind_int(handle, i, std::get<int>(value));
				} break;
				case 2:{
					r = sqlite3_bind_double(handle, i, std::get<double>(value));
				} break;
				case 3:{
					std::string val = std::get<std::string>(value);
					r = sqlite3_bind_text(handle, i, val.data(), val.length(), SQLITE_TRANSIENT);
				} break;
				case 4:{
					std::vector<unsigned char> val = std::get<std::vector<unsigned char>>(value);
					r = sqlite3_bind_blob(handle, i, val.data(), val.size(), SQLITE_TRANSIENT);
				} break;
			}
			if(r != SQLITE_OK){
			}
			i++;
		}
		std::string error = sqlite3_errmsg(this->db);
		if(error != "not an error" && error != "another row available" && error != "column index out of range"){
		}
	}

	database::database(std::string fileName)
	{
		sqlite3_open(fileName.data(), &handle);
	}

	database::~database()
	{
		sqlite3_close(handle);
	}
}
