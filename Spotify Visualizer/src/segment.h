#ifndef _SEGMENT_H_
#define _SEGMENT_H_
#include <vector>
class segment
{
public:
	segment(double start, double duration, double loudness_start, double loudness_max_time, double loudness_max, std::vector<double> pitches, std::vector<double> timbre) :
		start_(start), duration_(duration), loudness_start_(loudness_start), loudness_max_time_(loudness_max_time), loudness_max_(loudness_max), pitches_(pitches), timbre_(timbre) {};
	segment();
	
	double GetStart();
	double GetDuration();
	double GetLoudnessStart();
	double GetLoudnessMaxTime();
	double GetLoudnessMax();
	std::vector<double> GetPitches();
	std::vector<double> GetTimbre();

private:
	double start_;
	double duration_;
	double loudness_start_;
	double loudness_max_time_;
	double loudness_max_;
	std::vector<double> pitches_;
	std::vector<double> timbre_;

};

#endif