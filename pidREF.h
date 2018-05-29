#include <string>
#include <iostream>
#include <list>

using namespace std;

struct PID {
	static const int desiredDistance = 500;
	static const int baseVel = 80;
	static const int wheelsDistance = 260;
	double w = pow(10, -4);
	double P = 2.5;
	double D = -500;
	double I = 0.0005;

	int iterations = 0;
	int errorT1, errorT2, errorTOT = 0;
	double RW;
	int min, minSensor;
	list<int> list4, list5, list6, list7;

	/*Normalize all the distance to the "y" value*/
	double distance4(double distance) {
		return distance + 105;
	}

	double distance5(double distance) {
		return distance + 50;
	}

	double distance6(double distance) {
		return distance + 15;
	}

	double error(double desiredValue, double currentValue) {
		return desiredValue - currentValue;;
	}

	double output(double errorT1, double errorT2, double errorTot, double P, double D, double I) {
		return P * errorT2 + I * errorTot + D * (errorT2 - errorT1);
	}

	double rw(double out, double wheelsDistance, double w) {
		return out * w * wheelsDistance / 2.0;
	}
} *pid = new PID;

void runPID(int s4, int s5, int s6, int s7, ArRobot *robot) {
	s4 = ::pid->distance4(s4);
	s5 = ::pid->distance5(s5);
	s6 = ::pid->distance6(s6);

	::pid->min = s4;
	::pid->minSensor = 4;

	/*Select the min sensor*/
	if (s5 < ::pid->min) {
		::pid->min = s5;
		::pid->minSensor = 5;
	}
	if (s6 < ::pid->min) {
		::pid->min = s6;
		::pid->minSensor = 6;
	}
	if (s7 < ::pid->min) {
		::pid->min = s7;
		::pid->minSensor = 7;
	}

	/*Catch the current error*/
	::pid->errorT2 = ::pid->error(::pid->desiredDistance, ::pid->min);

	cout << "errorT2 = " << ::pid->errorT2 << endl;

	if (::pid->iterations == 0)
		::pid->errorT1 = ::pid->errorT2;

	/*Update the errorTOT*/
	::pid->errorTOT += ::pid->errorT2;
	/*Calculate the RW value*/
	::pid->RW = ::pid->rw(::pid->output(::pid->errorT2, ::pid->errorT1, ::pid->errorTOT, ::pid->P, ::pid->D, ::pid->I), ::pid->wheelsDistance, ::pid->w);

	robot->setVel2(::pid->baseVel - ::pid->RW, ::pid->baseVel + ::pid->RW);

	cout << "RW = " << ::pid->baseVel + ::pid->RW << endl;

	cout << "distance4 modified = " << s4 << endl;
	cout << "distance5 modified = " << s5 << endl;
	cout << "distance6 modified = " << s6 << endl;
	cout << "distance7 = " << s7 << endl;
	cout << "minSensor = " << ::pid->minSensor << endl;
	cout << "---------" << endl;

	/*Set previous error equal current error*/
	::pid->errorT1 = ::pid->errorT2;
	::pid->iterations++;
}