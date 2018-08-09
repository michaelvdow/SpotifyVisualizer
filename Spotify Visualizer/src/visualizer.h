#ifndef _VISUALIZER_H_
#define _VISUALIZER_H_
#include <cpprest/http_client.h>
#include "segment.h"
#include "beat.h"
#include "section.h"

class visualizer
{
public:
	visualizer();
	
	void Update();
	void CreateRandomBeatConstants();
	void SetCurrentTime(int progress_ms, long long timestamp);
	void Process(web::json::value &json);
	double GetCurrentBeatLength();
	int GetCurrentProgressMs();

	std::vector<double> GetBars();
	int num_of_bars_ = 13;

private:
	std::vector<segment> segments_;
	std::vector<beat> beats_;
	std::vector<beat> tatums_;
	std::vector<section> sections_;

	std::vector<double> bars_;

	std::vector<double> next_random_beats;
	std::vector<double> last_random_beats;
	std::vector<double> current_random_beats;

	//consts
	double loudness_constant_ = 0.02;
	double pitches_constant_ = 0.5;
	double beat_constant_ = 0.6;

	//index info
	int progress_ms_;
	long long timestamp_;
	int last_beat_index_;
	int last_segment_index_;
	int last_tatum_index_;
	int last_section_index_;

	double current_beat_length_;

	double visualizer::BarSmoother(double percentage);


};
#endif