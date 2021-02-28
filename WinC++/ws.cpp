#include "stdafx.h"
#include "ws.h"
#include "WebSocket.h"
#include <guiddef.h>
#include <combaseapi.h>
#include "MessageConfig.h"
#include "WebSocketDataAnalysis.h"

broadcast_server* broadcast_server::_instance = nullptr;
extern bool g_is_https;

broadcast_server::broadcast_server()
{
	_instance = this;
	m_server.init_asio();
	m_server.set_open_handler(bind(&broadcast_server::on_open, this, ::_1));
	m_server.set_close_handler(bind(&broadcast_server::on_close, this, ::_1));
	m_server.set_message_handler(bind(&broadcast_server::on_message, this, ::_1, ::_2));
	m_server.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload);
	m_connections.clear();
}

broadcast_server::~broadcast_server()
{
	_instance = nullptr;
}

void broadcast_server::run(unsigned int port)
{
	m_server.clear_access_channels(websocketpp::log::alevel::all);
	m_server.set_access_channels(websocketpp::log::alevel::connect);
	m_server.set_access_channels(websocketpp::log::alevel::disconnect);
	try {
		m_server.listen(port);
		m_port = port;
		m_server.start_accept();
	}
	catch (websocketpp::exception const & e) {
	}
	m_thread_io_loop = websocketpp::lib::make_shared<websocketpp::lib::thread>(&server::run, &m_server);
}

void broadcast_server::stop()
{
	m_quit = true;
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(action(EXIT));
	}
	m_action_cond.notify_one();
	if (m_thread_messsage_loop) m_thread_messsage_loop->join();
	m_thread_messsage_loop = nullptr;

	try{
		lock_guard<mutex> guard(m_connection_lock);
		m_server.stop_listening();
		for (auto it = m_connections.begin(); it != m_connections.end(); ++it)
			m_server.close(it->first, websocketpp::close::status::going_away, "");
		m_connections.clear();
	}catch (websocketpp::exception const & e){
		logerr("断开连接失败:%s", e.what());
		TerminateProcess(GetCurrentProcess(), 0);
	}
	m_server.stop();
	if (m_thread_io_loop)m_thread_io_loop->join();
	m_thread_io_loop = nullptr;
}

void broadcast_server::on_open(connection_hdl hdl) 
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(action(CONNECT, hdl));
		server::connection_ptr con = m_server.get_con_from_hdl(hdl);
		loginfo("连接ip:%s", con->get_remote_endpoint().c_str());
	}
	m_action_cond.notify_one();
}

void broadcast_server::on_close(connection_hdl hdl) 
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(action(DISCONNECT, hdl));
		server::connection_ptr con = m_server.get_con_from_hdl(hdl);
		loginfo("断开连接ip:%s", con->get_remote_endpoint().c_str());
	}
	m_action_cond.notify_one();
}

void broadcast_server::on_message(connection_hdl hdl, server::message_ptr msg) 
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(action(RECV_MESSAGE, hdl, msg));
	}
	m_action_cond.notify_one();
}

void broadcast_server::send_err_msg(websocketpp::connection_hdl & hdl, const string& cmd, int id)
{
	std::time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
	string str = format("{\"cmd\":\"%s\",\"id\":\"%d\",\"time\":\"%s\",\"ocx_notify\" : 1}", cmd.c_str(),id, XLTool::TimetoString(&tt).c_str());
	m_server.send(hdl, (void*)str.c_str(), str.size(), websocketpp::frame::opcode::TEXT);
}

void broadcast_server::process_messages()
{
	while (!m_quit){
		unique_lock<mutex> lock(m_action_lock);
		m_action_cond.wait(lock, [&]() {
			return !m_actions.empty() || m_quit;
		});
		if(m_quit) break;
		action a = m_actions.front();
		m_actions.pop();
		size_t actionsListSize = m_actions.size();
		lock.unlock();

		switch (a.get_type()){
		case CONNECT:{
			lock_guard<mutex> guard(m_connection_lock);
			m_connections.insert(make_pair(a.get_hdl(), create_guid()));
		}
		break;
		case DISCONNECT:{
			lock_guard<mutex> guard(m_connection_lock);
			auto it = m_connections.find(a.get_hdl());
			if (it != m_connections.end()){
				string* json = new string(format("{\"cmd\":\"destroy\",\"id\":\"%d\",\"cmd_id\":\"\"}", it->second));
				PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_WEB_MSG, (WPARAM)json, it->second);
				m_connections.erase(a.get_hdl());
			}
		}
		break;
		case RECV_MESSAGE:{
			if (a.get_msg()->get_opcode() == websocketpp::frame::opcode::text) {
				string* json = new string(a.get_msg()->get_payload());
				lock_guard<mutex> guard(m_connection_lock);
				auto it = m_connections.find(a.get_hdl());
				if (it != m_connections.end()) {
					//logdebug(">>>%s, id = %d", json->c_str(), it->second);
					//Trace(">>>%s, id = %d\n", json->c_str(), it->second);
					PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_WEB_MSG, (WPARAM)json, it->second);
				}
			}
		}
		break;
		case SEND_MESSAGE:{
			lock_guard<mutex> guard(m_connection_lock);
			auto it = find_if(m_connections.begin(), m_connections.end(), [&](std::map<connection_hdl, long>::const_reference ref)->bool {
				if (ref.second == a.get_id()) return true;
				return false;
			});
			if (it != m_connections.end()) {
				//logdebug("<<<%s, id = %d", a.data.c_str(), it->second);
				//Trace("<<<%s, id = %d\n", a.data.c_str(), it->second);
				try{
					m_server.send(it->first, (void*)a.data.c_str(), a.data.size(), websocketpp::frame::opcode::TEXT);
				}catch (...){
					Trace("<<<【消息发送失败】%s, id = %d\n", a.data.c_str(), it->second);
				}
				
			}
		}
		break;
		case EXIT:
			break;
		default:
			break;
		}
	}
}
int broadcast_server::get_connection_size() 
{
	lock_guard<mutex> guard(m_connection_lock);
	return m_connections.size();
}

void broadcast_server::send_msg(int id, const string& msg)
{
	if (!_instance) return;
	{
		lock_guard<mutex> guard(_instance->m_action_lock);
		_instance->m_actions.push(action(SEND_MESSAGE, msg, id));
	}
	_instance->m_action_cond.notify_one();
}