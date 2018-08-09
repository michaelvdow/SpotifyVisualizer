#include "ofApp.h"
#include "../../Spotify Visualizer/src/spotify_client.h"
#include "../../Spotify Visualizer/src/visualizer.h"
#include <thread>
#include "ofxDatGui.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//Get id of current song
	auto json = client_.GetUsersCurrentlyPlayingTrack().extract_json().get();
	auto id = json[L"item"][L"id"].as_string();

	//Setup progress in audio visualizer
	int progress_ms = json[L"progress_ms"].as_integer();
	long long epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	audio_visualizer_.SetCurrentTime(progress_ms, epoch);

	//Get track analysis for audio visualizer
	auto track_analysis = client_.GetTrackAnalysis(id).extract_json().get();
	audio_visualizer_.Process(track_analysis);

	//Get song name
	current_song_ = utility::conversions::to_utf8string(json[L"item"][L"name"].serialize());
	current_artist_ = utility::conversions::to_utf8string(json[L"item"][L"artists"].as_array()[0][L"name"].serialize());

	//Setup window size
	ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
	update_frequency_ = 1000;
	is_playing_ = true;

	//Setup colors
	last_color_update_ = epoch;
	current_random_color_ = 0;
	color_ = ofColor::fromHex(0x1db954);
	random_colors_.push_back(ofColor::fromHex(0x1db954));
	random_colors_.push_back(ofColor::fromHex(0x42c99c));
	random_colors_.push_back(ofColor::fromHex(0x4dc0ea));
	random_colors_.push_back(ofColor::fromHex(0x6778fc));
	random_colors_.push_back(ofColor::fromHex(0xb167fc));
	random_colors_.push_back(ofColor::fromHex(0xf767eb));
	random_colors_.push_back(ofColor::fromHex(0xf7677c));
	random_colors_.push_back(ofColor::fromHex(0xf78f67));
	random_colors_.push_back(ofColor::fromHex(0xf7d767));
	random_colors_.push_back(ofColor::fromHex(0xc7f767));

	//Setup song and artist label
	left_gui_ = new ofxDatGui(ofxDatGuiAnchor::BOTTOM_LEFT);
	song_label_ = left_gui_->addLabel(current_song_);
	song_label_->setStripeColor(ofColor::forestGreen);
	song_label_->setStripeVisible(true);
	song_label_->setStripeWidth(18);
	left_gui_->addBreak()->setHeight(5);

	artist_label_ = left_gui_->addLabel(current_artist_);
	artist_label_->setStripeColor(ofColor::forestGreen);
	artist_label_->setStripeVisible(true);
	artist_label_->setStripeWidth(18);
	left_gui_->addBreak()->setHeight(15);

	//Setup color picker and random color chooser
	right_gui_ = new ofxDatGui(0, 0);
	right_gui_->addBreak()->setHeight(60);
	random_color_toggle_ = right_gui_->addToggle("Random colors", false);
	random_color_toggle_->setStripeColor(ofColor::forestGreen);
	random_color_toggle_->setStripeVisible(true);
	random_color_toggle_->setStripeWidth(18);
	random_color_toggle_->onToggleEvent(this, &ofApp::onToggleEvent);
	right_gui_->addBreak()->setHeight(5);

	color_picker_ = right_gui_->addColorPicker("Picker", ofColor::fromHex(0x1db954));
	color_picker_->setStripeColor(ofColor::forestGreen);
	color_picker_->setStripeVisible(true);
	color_picker_->setStripeWidth(18);
	color_picker_->onColorPickerEvent(this, &ofApp::onColorPickerEvent);
	right_gui_->addBreak()->setHeight(15);
}

//--------------------------------------------------------------
void ofApp::update(){
	//If song is not being processed, update visualizer
	if (!is_processing_ && is_playing_) {
		audio_visualizer_.Update();
	}

	//Every 5 seconds, check Spotify progress in the backgrounds
	long long epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if (epoch > last_updated_ + update_frequency_ && !is_processing_) {
		std::thread thread(std::bind(&ofApp::checkSpotifyProgress, this));
		thread.detach();
		last_updated_ = epoch;
	}
}

//--------------------------------------------------------------
//Draw the audio visualizer and labels
void ofApp::draw(){
	//Setup colors
	ofBackgroundHex(0x191414);
	song_label_->setStripeColor(color_);
	artist_label_->setStripeColor(color_);
	color_picker_->setStripeColor(color_);
	random_color_toggle_->setStripeColor(color_);
	ofSetColor(color_);
	
	//Draw the bars
	auto bars = audio_visualizer_.GetBars();
	auto screen_width = ofGetWindowWidth();
	auto screen_height = ofGetWindowHeight();

	double width = screen_width / (2 * audio_visualizer_.num_of_bars_);
	for (int i = 0; i < bars.size(); i++) {
		double max_bar_height = screen_width / 2;
		ofDrawRectangle(screen_width / 4 + i * (width + 1), -screen_height / 4 + max_bar_height - bars.at(i) * max_bar_height, width, bars.at(i) * max_bar_height);
	}

	//Update labels
	song_label_->setLabel(current_song_);
	artist_label_->setLabel(current_artist_);
}

//When toggle, create background thread to update random colors
void ofApp::onToggleEvent(ofxDatGuiToggleEvent e) {
	if (e.checked) {
		std::thread thread(std::bind(&ofApp::updateRandomColor, this));
		thread.detach();
	}
}

//The background thread to update the random color
void ofApp::updateRandomColor() {
	while (random_color_toggle_->getChecked()) {
		if (is_playing_) {
			//Get percentage into beat
			long long beat_length_millis = audio_visualizer_.GetCurrentBeatLength() * 1000;
			long long epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			long long difference = epoch - last_color_update_;
			double percentage = difference / (double)beat_length_millis;
			if (percentage > 1) {
				percentage = 1;
			}

			//Update color
			int next_color;
			if (current_random_color_ == random_colors_.size() - 1) {
				next_color = 0;
			}
			else {
				next_color = current_random_color_ + 1;
			}
			color_ = random_colors_.at(current_random_color_).lerp(random_colors_.at(next_color), (float)percentage);

			//Update to next color if beat has finished
			if (percentage == 1) {
				last_color_update_ = epoch;
				if (current_random_color_ == random_colors_.size() - 1) {
					current_random_color_ = 0;
				}
				else {
					current_random_color_++;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}

//Update color on color picker change if not randomized
void ofApp::onColorPickerEvent(ofxDatGuiColorPickerEvent e) {
	if (!random_color_toggle_->getChecked()) {
		color_ = e.color;
	}
}

//Check the progress of the current song
//If new song, process the song's audio analysis for the visualizer
void ofApp::checkSpotifyProgress() {
	auto json = client_.GetUsersCurrentlyPlayingTrack().extract_json().get();
	auto id = json[L"item"][L"id"].as_string();

	int progress_ms = json[L"progress_ms"].as_integer();
	long long epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if (abs(audio_visualizer_.GetCurrentProgressMs() - progress_ms) > 50) {
		audio_visualizer_.SetCurrentTime(progress_ms, epoch);
	}

	if (json[L"is_playing"].is_boolean()) {
		is_playing_ = json[L"is_playing"].as_bool();
	}

	//If new song, process the song's audio analysis while pausing the updating
	auto this_song = utility::conversions::to_utf8string(json[L"item"][L"name"].serialize());
	if (this_song != current_song_) {
		is_processing_ = true;

		//Process audio analysis
		auto track_analysis = client_.GetTrackAnalysis(id).extract_json().get();
		audio_visualizer_.Process(track_analysis);

		//Update labels
		current_song_ = this_song;
		current_artist_ = utility::conversions::to_utf8string(json[L"item"][L"artists"].as_array()[0][L"name"].serialize());

		is_processing_ = false;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'F' || key == 'f') {
		ofToggleFullscreen();
	} else if (key == 'H' || key == 'h') {
		left_gui_->setVisible(!left_gui_->getVisible());
		right_gui_->setVisible(!right_gui_->getVisible());
	}
}

//Delete pointers to label objects
ofApp::~ofApp() {
	delete left_gui_;
	delete right_gui_;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}