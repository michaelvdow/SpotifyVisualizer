#include "visualizer.h"
#include <vector>
#include "segment.h"
#include "beat.h"
#include "section.h"
#include <cpprest/http_client.h>
#include <chrono>
#include <random>
#include <iostream>

visualizer::visualizer()
{
	bars_.resize(18);
	current_random_beats.resize(6);
	last_random_beats.resize(6);
	next_random_beats.resize(6);
	last_beat_index_ = 0;
	last_segment_index_ = 0;
	last_tatum_index_ = 0;
	last_section_index_ = 0;
}

void visualizer::Update() {
	//Update the current time
	long long epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	long long difference = epoch - timestamp_;
	progress_ms_ = progress_ms_ + difference;
	timestamp_ = epoch;
	//std::cout << (progress_ms_ / 1000.0) << std::endl;

	//Find the current segment based on the current time
	double time_seconds = progress_ms_ / 1000.0;
	if (segments_.at(last_segment_index_).GetStart() > time_seconds) {
		for (int i = last_segment_index_; i >= 0; i--) {
			if (segments_.at(i).GetStart() < time_seconds) {
				last_segment_index_ = i;
				break;
			}
		}
	} else {
		for (int i = last_segment_index_; i < segments_.size(); i++) {
			if (segments_.at(i).GetStart() > time_seconds) {
				last_segment_index_ = i - 1;
				break;
			}
		}
	}

	//Find the current section based on the current time
	if (sections_.at(last_section_index_).GetStart() > time_seconds) {
		for (int i = last_section_index_; i >= 0; i--) {
			if (sections_.at(i).GetStart() < time_seconds) {
				last_section_index_ = i;
				break;
			}
		}
	} else {
		for (int i = last_section_index_; i < sections_.size(); i++) {
			if (sections_.at(i).GetStart() > time_seconds) {
				last_section_index_ = i - 1;
				break;
			}
		}
	}

	////////////Setup the pitches bars

	//Get the current segment and pitches
	auto segment = segments_.at(last_segment_index_);
	auto section = sections_.at(last_section_index_);
	auto pitches = segment.GetPitches();
	auto timbre = segment.GetTimbre();
	auto loudness = 1/ abs(section.GetLoudness()) * loudness_constant_;
	
	//Set the size of each segment bar based on percentage and pitch size
	double segment_percentage = (time_seconds - segment.GetStart()) / segment.GetDuration();
	double segment_scale = BarSmoother(segment_percentage);

	double half = (time_seconds - segment.GetStart()) / segment.GetDuration();
	if (half >= 0.5) {
		auto next_pitches = segments_.at(last_segment_index_ + 1).GetPitches();
		for (int i = 0; i < pitches.size(); i++) {
			bars_.at(i) = 0.2 + loudness + pitches_constant_ * (segment_scale * pitches.at(i) + (1 - segment_scale) * next_pitches.at(i)) / 2;
		}
	} else {
		auto prev_pitches = segments_.at(last_segment_index_ - 1).GetPitches();
		for (int i = 0; i < pitches.size(); i++) {
			bars_.at(i) = 0.2 + loudness + pitches_constant_ * (segment_scale * pitches.at(i) + (1 - segment_scale) * prev_pitches.at(i)) / 2;
		}
	}
	
	////////////Setup the beats bars

	//Find the current beat based on the current time
	if (beats_.at(last_beat_index_).GetStart() > time_seconds) {
		for (int i = last_beat_index_; i >= 0; i--) {
			if (beats_.at(i).GetStart() < time_seconds) {
				if (last_beat_index_ != i) {
					CreateRandomBeatConstants();
				}
				last_beat_index_ = i;
				break;
			}
		}
	} else {
		for (int i = last_beat_index_; i < beats_.size(); i++) {
			if (beats_.at(i).GetStart() > time_seconds) {
				if (last_beat_index_ != i - 1) {
					CreateRandomBeatConstants();
				}
				last_beat_index_ = i - 1;
				break;
			}
		}
	}

	//Calculate beat bar values
	auto beat = beats_.at(last_beat_index_);
	double beat_percentage = abs((time_seconds - beat.GetStart()) / beat.GetDuration() - 0.5) + 0.5;
	double beat_half = (time_seconds - beat.GetStart()) / beat.GetDuration();
	double beat_scale = BarSmoother(beat_half);

	for (int i = 7; i < num_of_bars_; i++) {
		if (beat_half >= 0.5) {
			bars_.at(i) = loudness + beat_percentage * 0.4 * beat_constant_ + 0.5 * 
				(beat_scale * current_random_beats.at(i - 7) + (1 - beat_scale) * next_random_beats.at(i - 7)) / 2;
		}
		else {
			bars_.at(i) = loudness + beat_percentage * 0.4 * beat_constant_ + 0.5 *
				(beat_scale * current_random_beats.at(i - 7) + (1 - beat_scale) * last_random_beats.at(i - 7)) / 2;
		}
	}
	current_beat_length_ = beat.GetDuration();
}

//Generate random beat constants
void visualizer::CreateRandomBeatConstants() {
	last_random_beats = current_random_beats;
	current_random_beats = next_random_beats;
	next_random_beats.clear();

	std::default_random_engine generator;
	generator.seed(std::random_device()());
	std::uniform_real_distribution<double> distribution(0.6, 1);

	for (int i = 0; i < 6; i++) {
		next_random_beats.push_back(distribution(generator));
	}
}

//Smooth percentage value with quadratic
double visualizer::BarSmoother(double percentage) {
	return -2 * percentage * percentage + 2 * percentage + 0.5;
}

//Set the current time in the song based on timestamp
void visualizer::SetCurrentTime(int progress_ms, long long timestamp) {
	progress_ms_ = progress_ms;
	timestamp_ = timestamp;
}

//Process the json value into segment and beat objects
void visualizer::Process(web::json::value &json) {
	beats_.clear();
	segments_.clear();
	sections_.clear();
	tatums_.clear();

	auto beats_json = &json[L"beats"].as_array();
	auto segments_json = &json[L"segments"].as_array();
	auto sections_json = &json[L"sections"].as_array();

	//Loop through beats array and append to beats vector
	for (auto iter = beats_json->cbegin(); iter != beats_json->cend(); iter++) {
		auto beat_json = iter->as_object();
		beat new_beat(beat_json.at(L"start").as_double(), beat_json.at(L"duration").as_double());
		beats_.push_back(new_beat);
	}

	//Loop through sections array and append to sections vector
	for (auto iter = sections_json->cbegin(); iter != sections_json->cend(); iter++) {
		auto section_json = iter->as_object();
		section new_section(section_json.at(L"start").as_double(), section_json.at(L"duration").as_double(), 
			section_json.at(L"loudness").as_double(), section_json.at(L"tempo").as_double());
		sections_.push_back(new_section);
	}

	//Loop through segments array and append to segments vector
	for (auto iter = segments_json->cbegin(); iter != segments_json->cend(); iter++) {
		auto segment_json = iter->as_object();
		auto pitches_json = segment_json.at(L"pitches").as_array();
		auto timbre_json = segment_json.at(L"timbre").as_array();

		//Create pitches vector from json array
		std::vector<double> pitches;

		//Get just the major notes
		pitches.push_back(pitches_json.at(0).as_double());
		pitches.push_back(pitches_json.at(2).as_double());
		pitches.push_back(pitches_json.at(4).as_double());
		pitches.push_back(pitches_json.at(5).as_double());
		pitches.push_back(pitches_json.at(7).as_double());
		pitches.push_back(pitches_json.at(9).as_double());
		pitches.push_back(pitches_json.at(11).as_double());

		//Create tatums vector from json array
		std::vector<double> timbre;
		for (auto value : timbre_json) {
			timbre.push_back(value.as_double());
		}

		segment new_segment(segment_json.at(L"start").as_double(), segment_json.at(L"duration").as_double(), 
			segment_json.at(L"loudness_start").as_double(), segment_json.at(L"loudness_max_time").as_double(), 
			segment_json.at(L"loudness_max").as_double(), pitches, timbre);
		segments_.push_back(new_segment);
	}	
	last_beat_index_ = 0;
	last_segment_index_ = 0;
	last_tatum_index_ = 0;
	last_section_index_ = 0;
}

std::vector<double> visualizer::GetBars() {
	return bars_;
}

double visualizer::GetCurrentBeatLength() {
	return current_beat_length_;
}

int visualizer::GetCurrentProgressMs() {
	return progress_ms_;
}