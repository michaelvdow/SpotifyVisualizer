#pragma once
class beat
{
public:
	beat(double start, double duration): start_(start), duration_(duration) {};
	beat();

	double GetStart();
	double GetDuration();

private:
	double start_;
	double duration_;
};

