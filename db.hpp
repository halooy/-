#include<iostream>
#include<string>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>
#include<mutex>

namespace order_sys
{
#define MYSQL_SERVER "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PASSWD "ysy519"
#define MYSQL_DBNAME "order_sys"
	static MYSQL * MysqlInit()
	{
		//��ʼ�����
		//mysql_init(�����ַ);
		MYSQL *mysql = NULL;
		mysql = mysql_init(NULL);
		if(mysql == NULL)
		{
			std::cout << "mysql init failed!\n";
			return NULL;
		}
		//���ӷ�����
		//mysql_real_connect(�����������IP���û��������룬���ݿ����ƣ��˿ڣ��׽����ļ����ͻ��˱�־);
		if(mysql_real_connect(mysql, MYSQL_SERVER, MYSQL_USER, MYSQL_PASSWD, MYSQL_DBNAME, 0, NULL, 0) == NULL)
		{
			std::cout << mysql_error(mysql) << std::endl;
			return NULL;
		}
		//�����ַ���
		//mysql_set_character_set(������ַ�������);
		if(mysql_set_character_set(mysql, "utf8mb4") != 0)
		{
			std::cout << mysql_error(mysql) << std::endl;
			return NULL;
		}
		//ѡ�����ݿ�(���ӷ�����ʱ�Ѿ�Ĭ��ѡ�����ݿ���)
		//mysql_select_db(��������ݿ�����);
		//mysql_select_db(mysql, MYSQL_DBNAME);
		return mysql;
	}
	static void MysqlRelease(MYSQL *mysql)
	{
		if(mysql != NULL)
		{
			mysql_close(mysql);
		}
	}
	static bool MysqlQuery(MYSQL *mysql, const std::string &sql)
	{
		//mysql_query(�����sql���);
		if(mysql_query(mysql, sql.c_str()) != 0)
		{
			std::cout << sql << std::endl;
			std::cout << mysql_error(mysql) << std::endl;
			return false;
		}
		return true;
	}
	class TableDish
	{
		private:
			MYSQL *_mysql;
			std::mutex _mutex;
		public:
			//mysql��ʼ��
			TableDish()
			{
				_mysql = MysqlInit();
				if(_mysql == NULL)
				{
					exit(-1);
				}
			}
			~TableDish()
			{
				MysqlRelease(_mysql);
				_mysql = NULL; 
			}
			bool Insert(const Json::Value &dish)
			{
				//��֯sql���
				#define DISH_INSERT "insert tb_dish values(null, '%s', %d, now());"
				char str_sql[4096] = {0};
				sprintf(str_sql, DISH_INSERT, dish["name"].asCString(), dish["price"].asInt());
				if(MysqlQuery(_mysql, str_sql) == false)
				{
					return false;
				}
				return true;
			}
			bool Delete(int dish_id)
			{
				#define DISH_DELETE "delete from tb_dish where id = %d;"
				char str_sql[4096] = {0};
				sprintf(str_sql, DISH_DELETE, dish_id);
				return MysqlQuery(_mysql, str_sql);
			}
			bool Update(const Json::Value &dish)
			{
                std::string priceStr = dish["price"].asString();
                int price = std::stoi(priceStr);
                #define DISH_UPDATE "update tb_dish set name = '%s', price = %d where id = %d;"
				char str_sql[4096] = {0};
				sprintf(str_sql, DISH_UPDATE,
					       	dish["name"].asCString(), price, 
						dish["id"].asInt());

                std::cout << "Generated SQL query: " << str_sql << std::endl;
				return MysqlQuery(_mysql, str_sql);
			}
			bool SelectAll(Json::Value *dishes)
			{
				#define DISH_SELECTALL "select * from tb_dish;"
				_mutex.lock();
				bool ret = MysqlQuery(_mysql, DISH_SELECTALL);
				if(ret = false)
				{
					_mutex.unlock();
					return false;
				}
				MYSQL_RES *res = mysql_store_result(_mysql);
				_mutex.unlock();
				if(res == NULL)
				{
					std::cout << "store result failed!\n";
					return false;
				}
				int num = mysql_num_rows(res);
				for(int i = 0; i < num; i++)
				{
					MYSQL_ROW row = mysql_fetch_row(res);
					Json::Value dish;
					dish["id"] = std::stoi(row[0]);
					dish["name"] = row[1];
					dish["price"] = std::stoi(row[2]);
					dish["ctime"] = row[3];
					dishes->append(dish);
				}
				mysql_free_result(res);
				return true;
			}
			bool SelectOne(int dish_id, Json::Value *dish)
			{
				#define DISH_SELECTONE "select * from tb_dish where id = %d;"
				char str_sql[4096] = {0};
				sprintf(str_sql, DISH_SELECTONE, dish_id);
				_mutex.lock();
				bool ret = MysqlQuery(_mysql, DISH_SELECTONE);
				if(ret = false)
				{     
					_mutex.unlock();
					return false;
				}
				MYSQL_RES *res = mysql_store_result(_mysql);
				_mutex.unlock();
				if(res == NULL)
				{
					std::cout << "store result failed!\n";
					return false;
				}
				int num = mysql_num_rows(res);
				if(num != 1)
				{
					std::cout << "result error\n";
					mysql_free_result(res);
					return false;
				}
				MYSQL_ROW row = mysql_fetch_row(res);
				(*dish)["id"] = dish_id;
				(*dish)["name"] = row[1];
				(*dish)["price"] = std::stoi(row[2]);
				(*dish)["ctime"] = row[3];
				mysql_free_result(res);
				return true;
			}
	};
	class TableOrder
	{
		private:
			MYSQL *_mysql;
			std::mutex _mutex;
		public:
			//mysql��ʼ��
			TableOrder()
			{
				_mysql = MysqlInit();
				if(_mysql == NULL)
				{
					exit(-1);
				}
			}
			~TableOrder()
			{
				if(_mysql != NULL)
				{
					MysqlRelease(_mysql);
					_mysql = NULL;
				}
			}
			bool Insert(const Json::Value &order)
			{
				#define ORDER_INSERT "insert tb_order values(null, '%s', 0, now());"
				char str_sql[4096] = {0};
				Json::FastWriter writer;
				std::string dishes = writer.write(order["dishes"]);
				dishes[dishes.size() - 1] = '\0';
				sprintf(str_sql, ORDER_INSERT, dishes.c_str());
				return MysqlQuery(_mysql, str_sql);
			}
			bool Delete(int order_id)
			{
				#define ORDER_DELETE "delete from tb_order where id = %d;"
				char str_sql[4096] = {0};
				sprintf(str_sql, ORDER_DELETE, order_id);
				return MysqlQuery(_mysql, str_sql);
			}
			bool Update(const Json::Value &order)
			{
				#define ORDER_UPDATE "update tb_order set dishes = '%s', status = %d where id = %d;"
				char str_sql[4096] = {0};
				Json::FastWriter writer;
				std::string dishes = writer.write(order["dishes"]);
				sprintf(str_sql, ORDER_UPDATE, 
						dishes.c_str(), order["status"].asInt(),
						order["id"].asInt());
				return MysqlQuery(_mysql, str_sql);
			}
			bool SelectAll(Json::Value *orders)
			{
				#define ORDER_SELECTALL "select * from tb_order;"
				_mutex.lock();
				bool ret = MysqlQuery(_mysql, ORDER_SELECTALL);
				if(ret == false)
				{
					_mutex.unlock();
					return false;
				}
				MYSQL_RES *res = mysql_store_result(_mysql);
				_mutex.unlock();
				if(res == NULL)
				{
					std::cout << mysql_error(_mysql) << std::endl;
					return false;
				}
				int num = mysql_num_rows(res);
				for(int i = 0; i < num; i++)
				{
					MYSQL_ROW row = mysql_fetch_row(res);
					Json::Value order;
					order["id"] = std::stoi(row[0]);
					Json::Reader reader;
					Json::Value dishes;
					reader.parse(row[1], dishes);
					order["dishes"] = dishes;
					order["status"] = std::stoi(row[2]);
					order["mtime"] = row[3];
					orders->append(order);
				}
				mysql_free_result(res);
				return true;
			}
			bool SelectOne(int order_id, Json::Value *order)
			{
				#define ORDER_SELECTONE "select * from tb_order where id = %d;"
				char str_sql[4096] = {0};
				sprintf(str_sql, ORDER_SELECTONE, order_id);
				_mutex.lock();
				bool ret = MysqlQuery(_mysql, str_sql);
				if(ret == false)
				{
					_mutex.unlock();
					return false;
				}
				MYSQL_RES *res = mysql_store_result(_mysql);
				_mutex.unlock();
				if(res == NULL)
				{
					std::cout << "store failed!\n";
					return false;
				}
				int num = mysql_num_rows(res);
				if(num != 1)
				{
					std::cout << "one result error\n";
					mysql_free_result(res);
					return false;
				}
				MYSQL_ROW row = mysql_fetch_row(res);
				(*order)["id"] = order_id;
				Json::Reader reader;
				Json::Value dishes;
				reader.parse(row[1], dishes);
				(*order)["dishes"] = dishes;
				(*order)["status"] = std::stoi(row[2]);
				(*order)["mtime"] = row[3];
				mysql_free_result(res);
				return true;
			}
	};
}
