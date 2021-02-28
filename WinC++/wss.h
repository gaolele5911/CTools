#pragma once


#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>
#include <iostream>
#include <set>
#include <map>
#include <functional>
#include "XLTool.h"
#include "../Public/singleton.h"


#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include "ws.h"

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

typedef websocketpp::server<websocketpp::config::asio_tls> servers;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;


using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

using namespace std;

enum tls_mode {
	MOZILLA_INTERMEDIATE = 1,
	MOZILLA_MODERN = 2
};

class actions 
{
public:
	actions(action_type t) : m_type(t){}
	actions(action_type t, const string& _data, int id) : m_type(t), data(_data), m_id(id) {}
	actions(action_type t, connection_hdl h) : m_type(t), m_hdl(h){}
	actions(action_type t, connection_hdl h, servers::message_ptr m): m_type(t), m_hdl(h), m_msg(m){}
	action_type get_type() const { return m_type; }
	websocketpp::connection_hdl get_hdl() const { return m_hdl; }
	servers::message_ptr get_msg() const { return m_msg; }
	int get_id() const { return m_id; }
	string data;
private:
	action_type	m_type;
	websocketpp::connection_hdl m_hdl;
	int m_id;
	servers::message_ptr	m_msg;
};

class broadcast_servers
{
public:
	void run(unsigned int port);
	void stop();
	void process_messages();
	static void send_msg(int id, const string& msg);
	static broadcast_servers* _instance;
private:
	friend class singleton<broadcast_servers>;
	broadcast_servers();
	virtual ~broadcast_servers();
	servers             m_server;
	std::map<connection_hdl, long, std::owner_less<connection_hdl>>	m_connections;
	std::queue<actions> m_actions;
	unsigned int       m_port;
	mutex              m_action_lock;
	mutex              m_connection_lock;
	condition_variable m_action_cond;
	XLTool	m_tool;
public:
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread_io_loop = nullptr;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread_messsage_loop = nullptr;
private:
	void on_open(connection_hdl hdl);
	void on_close(connection_hdl hdl);
	void on_message(connection_hdl hdl, servers::message_ptr msg);
	int  get_connection_size();
	long create_guid() { return m_guid++; };
	void send_err_msg(websocketpp::connection_hdl & hdl, const string& cmd, int id);
private:
	bool	m_quit = false;
	long	m_guid = 0;
};

typedef singleton<broadcast_servers> TSockets;