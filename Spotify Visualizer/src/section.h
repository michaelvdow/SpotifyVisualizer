#ifndef _SECTION_H_
#define _SECTION_H_
class section
{
public:
	section(double start, double duration, double loudness, double tempo) :
	start_(start), duration_(duration), loudness_(loudness), tempo_(tempo) {};
	section();

	double GetStart();
	double GetDuration();
	double GetLoudness();
	double GetTempo();

private:
	double start_;
	double duration_;
	double loudness_;
	double tempo_;
};

#endif