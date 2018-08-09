#include "segment.h"

segment::segment() {
}

double segment::GetStart() {
	return start_;
}

double segment::GetDuration() {
	return duration_;
}

double segment::GetLoudnessStart() {
	return loudness_start_;
}

double segment::GetLoudnessMaxTime() {
	return loudness_max_time_;
}

double segment::GetLoudnessMax() {
	return loudness_max_;
}

std::vector<double> segment::GetPitches() {
	return pitches_;
}

std::vector<double> segment::GetTimbre() {
	return timbre_;
}

