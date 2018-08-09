#include <cpprest/filestream.h>
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

spotify_client::spotify_client() {
	api.run();
}

web::http::http_response spotify_client::GetTrackAnalysis(std::wstring id) {
	return api.send_api_request(L"audio-analysis/" + id).get();
}

web::http::http_response spotify_client::GetUsersCurrentlyPlayingTrack() {
	return api.send_api_request(L"me/player/currently-playing").get();
}