#include <cpprest/json.h>
#include <iostream>
#include <windows.h>
#include <Shellapi.h>
#include "spotify_client.h"
#include "cpprest/http_listener.h"
#include "cpprest/http_client.h"
#include "oauth2.h"

using namespace utility;
using namespace web;
using namespace web::http::client;
using namespace web::http::oauth2::experimental;
using namespace web::http;
using namespace web::http::experimental::listener;

//CODE referenced from cpprestsdk example for this file:
//https://github.com/Microsoft/cpprestsdk/blob/bd4afa3b18bf5fddbe9fe2b88b527137655f6cfc/Release/samples/Oauth2Client/Oauth2Client.cpp

// Constants for spotify api keys
const utility::string_t spotify_session::spotify_key(L"c6ed71cf05be413ebbd3b84a93dab97d");
const utility::string_t spotify_session::spotify_secret(L"d0ad2ef0ebd44849b74ac86fbed4571c");

//Listens for the token from the redirected uri
oauth2_code_listener::oauth2_code_listener(uri listen_uri, oauth2_config& config) :
	listener(new http_listener(listen_uri)), m_config(config) {
	listener->support([this](http::http_request request) -> void {
		if (request.request_uri().path() == L"/" && request.request_uri().query() != L"") {
			m_resplock.lock();
			m_config.token_from_redirected_uri(request.request_uri()).then([this, request](pplx::task<void> token_task) -> void {
				try {
					token_task.wait();
					m_tce.set(true);
				}
				catch (const oauth2_exception& e) {
					std::cout << "Error: " << e.what() << std::endl;
					m_tce.set(false);
				}
			});

			request.reply(status_codes::OK, L"Ok.");

			m_resplock.unlock();
		}
		else {
			request.reply(status_codes::NotFound, L"Not found.");
		}
	});
	listener->open().wait();
}

//Closes the listener on destruction
oauth2_code_listener::~oauth2_code_listener() {
	listener->close().wait();
}

//Listens for the code returned from the redirect uri
pplx::task<bool> oauth2_code_listener::listen_for_code() {
	return pplx::create_task(m_tce);
}

// Runs authorization and retreives access tokens
void oauth2_session::run() {
	if (is_enabled()) {
		std::cout << "Running " << m_name.c_str() << " session..." << std::endl;

		if (!m_oauth2_config.token().is_valid_access_token()) {
			if (authorization_code_flow().get()) {
				std::wcout << m_oauth2_config.token().access_token();
				m_http_config.set_oauth2(m_oauth2_config);
			}
			else {
				std::cout << "Authorization failed for " << m_name.c_str() << "." << std::endl;
			}
		}

		run_internal();
	}
	else {
		std::cout << "Skipped " << m_name.c_str() << " session sample because app key or secret is empty. Please see instructions." << std::endl;
	}
}

// Opens browser and listens for redirect uri
pplx::task<bool> oauth2_session::authorization_code_flow() {
	open_browser_auth();
	return m_listener->listen_for_code();
}

// Checks if the client keys are not empty
bool oauth2_session::is_enabled() const {
	return !m_oauth2_config.client_key().empty() && !m_oauth2_config.client_secret().empty();
}

// Opens browser and will redirect upon user accept
void oauth2_session::open_browser_auth() {
	m_oauth2_config.set_scope(L"user-read-birthdate playlist-read-collaborative playlist-read-private user-library-read user-read-currently-playing user-read-recently-played user-modify-playback-state user-read-playback-state user-follow-read streaming");
	auto auth_uri(m_oauth2_config.build_authorization_uri(true));
	std::cout << "Opening browser in URI:" << std::endl;
	std::cout << std::string(auth_uri.begin(), auth_uri.end()) << std::endl;
	open_browser(auth_uri);
}

//Opens a browser at the uri provided
//https://stackoverflow.com/questions/17347950/open-url-from-c-code
void oauth2_session::open_browser(utility::string_t auth_uri) {
	ShellExecuteA(NULL, "open", conversions::utf16_to_utf8(auth_uri).c_str(), NULL, NULL, SW_SHOWNORMAL);
}


// Specialized class for Spotify OAuth 2.0 session.
void spotify_session::run_internal() {
	http_client api(L"https://api.spotify.com/v1/", m_http_config);
	std::cout << "Requesting account information:" << std::endl;
	std::wcout << "Information: " << api.request(methods::GET, L"me").get().extract_json().get()[L"birthdate"] << std::endl;
}

//Sends an api request through the spotify_session
Concurrency::task<web::http::http_response> spotify_session::send_api_request(std::wstring value) {
	http_client api(L"https://api.spotify.com/v1/", m_http_config);
	return api.request(methods::GET, value);
}
