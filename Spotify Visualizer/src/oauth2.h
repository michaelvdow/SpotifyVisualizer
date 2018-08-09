#ifndef _OAUTH2_H_
#define _OAUTH2_H_
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include <iostream>
#include <windows.h>
#include <Shellapi.h>
#include "cpprest/http_listener.h"
#include <cpprest/http_client.h>

using namespace web::http::client;
using namespace web::http::oauth2::experimental;
using namespace web::http;
using namespace web::http::experimental::listener;

//CODE referenced from cpprestsdk example for this file:
//https://github.com/Microsoft/cpprestsdk/blob/bd4afa3b18bf5fddbe9fe2b88b527137655f6cfc/Release/samples/Oauth2Client/Oauth2Client.cpp

class oauth2_code_listener {
public:
	oauth2_code_listener(uri listen_uri, oauth2_config& config);

	~oauth2_code_listener();

	pplx::task<bool> listen_for_code();

private:

	std::unique_ptr<http_listener> listener;
	pplx::task_completion_event<bool> m_tce;
	oauth2_config& m_config;
	std::mutex m_resplock;
};

//
// Base class for OAuth 2.0 sessions of this sample.
//
class oauth2_session {
public:
	oauth2_session(utility::string_t name,
		utility::string_t client_key,
		utility::string_t client_secret,
		utility::string_t auth_endpoint,
		utility::string_t token_endpoint,
		utility::string_t redirect_uri) :
		m_oauth2_config(client_key,
			client_secret,
			auth_endpoint,
			token_endpoint,
			redirect_uri),
		m_name(name),
		m_listener(new oauth2_code_listener(redirect_uri, m_oauth2_config)) {};

	void run();

protected:
	virtual void run_internal() = 0;

	pplx::task<bool> authorization_code_flow();

	http_client_config m_http_config;
	oauth2_config m_oauth2_config;

private:
	bool is_enabled() const;

	void open_browser(utility::string_t auth_uri);

	void open_browser_auth();

	utility::string_t m_name;
	std::unique_ptr<oauth2_code_listener> m_listener;
};

class spotify_session : public oauth2_session {
public:
	static const utility::string_t spotify_key;
	static const utility::string_t spotify_secret;
	spotify_session() :
		oauth2_session(L"Spotify",
			spotify_key,
			spotify_secret,
			L"https://accounts.spotify.com/authorize",
			L"https://accounts.spotify.com/api/token",
			L"http://localhost:8889/") {}
	Concurrency::task<web::http::http_response> spotify_session::send_api_request(std::wstring value);

protected:
	void run_internal();
};

#endif