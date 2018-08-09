#pragma once
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>
#include "oauth2.h"
using namespace web::http::client;

class spotify_client {
public:
	spotify_client();
	web::http::http_response spotify_client::GetTrackAnalysis(std::wstring id);
	web::http::http_response spotify_client::GetUsersCurrentlyPlayingTrack();

private:
	spotify_session api;
};
