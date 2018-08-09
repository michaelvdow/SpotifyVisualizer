#include "section.h"



section::section()
{
}

double section::GetStart() {
	return start_;
}

double section::GetDuration() {
	return duration_;
}

double section::GetLoudness() {
	return loudness_;
}

double section::GetTempo() {
	return tempo_;
}