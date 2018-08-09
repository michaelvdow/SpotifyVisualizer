#ifndef _OFAPP_H_
#define _OFAPP_H_

#include "ofMain.h"
#include "../../Spotify Visualizer/src/visualizer.h"
#include "../../Spotify Visualizer/src/spotify_client.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

	public:
		~ofApp();
		visualizer audio_visualizer_;
		spotify_client client_;

		std::string current_song_;
		std::string current_artist_;

		long long last_updated_;
		int update_frequency_;
		
		bool is_processing_;
		bool is_playing_;
		
		ofColor color_;
		std::vector<ofColor> random_colors_;

		ofxDatGui *left_gui_;
		ofxDatGuiLabel *song_label_;
		ofxDatGuiLabel *artist_label_;

		ofxDatGui *right_gui_;
		ofxDatGuiColorPicker *color_picker_;
		ofxDatGuiToggle *random_color_toggle_;

		long long last_color_update_;
		int current_random_color_;
		void onColorPickerEvent(ofxDatGuiColorPickerEvent e);
		void onToggleEvent(ofxDatGuiToggleEvent e);
		void updateRandomColor();

		void setup();
		void update();
		void draw();
		void checkSpotifyProgress();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
#endif