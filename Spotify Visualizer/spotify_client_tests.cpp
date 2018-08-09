#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "spotify_client.h"

TEST_CASE("Get Audio Analysis") {
	spotify_client client;
	client.GetTrackAnalysis(L"06AKEBrKUckW0KREUWRnvT");
}