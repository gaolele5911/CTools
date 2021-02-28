#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>
#include <iostream>
#include <set>
#include <map>
#include <functional>
#include "XLTool.h"
#include "../Public/singleton.h"

typedef websocketpp::server<websocketpp::config::asio> server;

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

enum action_type
{
	CONNECT,
	DISCONNECT,
	RECV_MESSAGE,
	SEND_MESSAGE, // 暂不支持二进制
	EXIT,
};

class action
{
public:
	action(action_type t) : m_type(t) {}
	action(action_type t, const string& _data, int id) : m_type(t), data(_data), m_id(id) {}
	action(action_type t, connection_hdl h) : m_type(t), m_hdl(h) {}
	action(action_type t, connection_hdl h, server::message_ptr m) : m_type(t), m_hdl(h), m_msg(m) {}
	action_type get_type() const { return m_type; }
	websocketpp::connection_hdl get_hdl() const { return m_hdl; }
	server::message_ptr get_msg() const { return m_msg; }
	int get_id() const { return m_id; }
	string data;
private:
	action_type	m_type;
	websocketpp::connection_hdl m_hdl;
	int m_id;
	server::message_ptr	m_msg;
};

class broadcast_server
{
public:
	void run(unsigned int port);
	void stop();
	void process_messages();
	static void send_msg(int id, const string& msg);
	static broadcast_server* _instance;
private:
	friend class singleton<broadcast_server>;
	broadcast_server();
	virtual ~broadcast_server();
	server             m_server;
	std::map<connection_hdl, long, std::owner_less<connection_hdl>>	m_connections;
	std::queue<action> m_actions;
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
	void on_message(connection_hdl hdl, server::message_ptr msg);
	int  get_connection_size();
	long create_guid() { return m_guid++; };
	void send_err_msg(websocketpp::connection_hdl & hdl, const string& cmd, int id);
private:
	bool	m_quit = false;
	long	m_guid = 0;
};

typedef singleton<broadcast_server> TSocket;