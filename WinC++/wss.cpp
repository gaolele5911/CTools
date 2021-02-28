#include "stdafx.h"
#include "wss.h"
#include "WebSocket.h"
#include <guiddef.h>
#include <combaseapi.h>
#include "MessageConfig.h"
#include "WebSocketDataAnalysis.h"

broadcast_servers* broadcast_servers::_instance = nullptr;

void on_http(servers* s, websocketpp::connection_hdl hdl) {
	servers::connection_ptr con = s->get_con_from_hdl(hdl);
	websocketpp::http::parser::request rt = con->get_request();
	const std::string& strUri = rt.get_uri();
	const std::string& strMethod = rt.get_method();
	const std::string& strBody = rt.get_body();	//只针对post时有数据
	const std::string& strHost = rt.get_header("host");
	const std::string& strContentType = rt.get_header("Content-type");
	const std::string& strVersion = rt.get_version();
	websocketpp::http::parser::header_list listhtpp = rt.get_headers();
	if (strMethod.compare("GET") == 0)
	{
		//to do 
	}
	else if (strMethod.compare("POST") == 0)
	{
		//to do 
	}

	con->set_body("Please Use wss!");
	con->set_status(websocketpp::http::status_code::ok);
}

context_ptr on_tls_init(tls_mode mode, websocketpp::connection_hdl hdl) {
	namespace asio = websocketpp::lib::asio;

	std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
	std::cout << "using TLS mode: " << (mode == MOZILLA_MODERN ? "Mozilla Modern" : "Mozilla Intermediate") << std::endl;

	context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

	try {
		if (mode == MOZILLA_MODERN) {
			// Modern disables TLSv1
			ctx->set_options(asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3 |
				asio::ssl::context::no_tlsv1 |
				asio::ssl::context::single_dh_use);
		}
		else {
			ctx->set_options(asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3 |
				asio::ssl::context::single_dh_use);
		}
		//这里面证书密码可以不需要验证，看生成的证书
		//ctx->set_password_callback(bind(&get_password));
		ctx->use_certificate_chain_file(".\\sailing\\sailing.crt");
		ctx->use_private_key_file(".\\sailing\\sailing.key", asio::ssl::context::pem);

		// Example method of generating this file:
		// `openssl dhparam -out dh.pem 2048`
		// Mozilla Intermediate suggests 1024 as the minimum size to use
		// Mozilla Modern suggests 2048 as the minimum size to use.
		//ctx->use_tmp_dh_file("dh.pem");

		std::string ciphers;

		if (mode == MOZILLA_MODERN) {
			ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";
		}
		else {
			ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
		}

		if (SSL_CTX_set_cipher_list(ctx->native_handle(), ciphers.c_str()) != 1) {
			std::cout << "Error setting cipher list" << std::endl;
		}
	}
	catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
	return ctx;
}



broadcast_servers::broadcast_servers() 
{
	_instance = this;
	m_server.init_asio();
	m_server.set_open_handler(bind(&broadcast_servers::on_open, this, ::_1));
	m_server.set_close_handler(bind(&broadcast_servers::on_close, this, ::_1));
	m_server.set_message_handler(bind(&broadcast_servers::on_message, this, ::_1, ::_2));
	m_server.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload);

	m_server.set_http_handler(bind(&on_http, &m_server, ::_1));
	m_server.set_tls_init_handler(bind(&on_tls_init, MOZILLA_INTERMEDIATE, ::_1));

	m_connections.clear();
}

broadcast_servers::~broadcast_servers()
{
	_instance = nullptr;
}

void broadcast_servers::run(unsigned int port)
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
	m_thread_io_loop = websocketpp::lib::make_shared<websocketpp::lib::thread>(&servers::run, &m_server);
}

void broadcast_servers::stop()
{
	m_quit = true;
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(actions(EXIT));
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

void broadcast_servers::on_open(connection_hdl hdl) 
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(actions(CONNECT, hdl));
		servers::connection_ptr con = m_server.get_con_from_hdl(hdl);
		loginfo("连接ip:%s", con->get_remote_endpoint().c_str());
	}
	m_action_cond.notify_one();
}

void broadcast_servers::on_close(connection_hdl hdl) 
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(actions(DISCONNECT, hdl));
		servers::connection_ptr con = m_server.get_con_from_hdl(hdl);
		loginfo("断开连接ip:%s", con->get_remote_endpoint().c_str());
	}
	m_action_cond.notify_one();
}

void broadcast_servers::on_message(connection_hdl hdl, servers::message_ptr msg) 
{
	{
		lock_guard<mutex> guard(m_action_lock);
		m_actions.push(actions(RECV_MESSAGE, hdl, msg));
	}
	m_action_cond.notify_one();
}

void broadcast_servers::send_err_msg(websocketpp::connection_hdl & hdl, const string& cmd, int id)
{
	std::time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
	string str = format("{\"cmd\":\"%s\",\"id\":\"%d\",\"time\":\"%s\",\"ocx_notify\" : 1}", cmd.c_str(),id, XLTool::TimetoString(&tt).c_str());
	m_server.send(hdl, (void*)str.c_str(), str.size(), websocketpp::frame::opcode::TEXT);
}

void broadcast_servers::process_messages()
{
	while (!m_quit){
		unique_lock<mutex> lock(m_action_lock);
		m_action_cond.wait(lock, [&]() {
			return !m_actions.empty() || m_quit;
		});
		if(m_quit) break;
		actions a = m_actions.front();
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
int broadcast_servers::get_connection_size() 
{
	lock_guard<mutex> guard(m_connection_lock);
	return m_connections.size();
}

void broadcast_servers::send_msg(int id, const string& msg)
{
	if (!_instance) return;
	{
		lock_guard<mutex> guard(_instance->m_action_lock);
		_instance->m_actions.push(actions(SEND_MESSAGE, msg, id));
	}
	_instance->m_action_cond.notify_one();
}