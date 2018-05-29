#include <string>
#include <iostream>
#include <list>
#include <Aria.h>

using namespace std;

struct REF {

	static const int nI = 2;		//Number of Inputs
	static const int nO = 2;		//Number of Outputs
	static const int MFi = 3;		//Number Membership Functions Input
	static const int MFo = 3;		//Number Membership Functions Output

											/*sensor7*/		/*sensor6*/		/*LMS*/		/*RMS*/
	string fuzzyRules[MFi*MFi][nI + nO] = { { "CLOSE",		"CLOSE",		"SLOW",		"FAST" },
											{ "CLOSE",		"MEDIUM",		"MEDIUM",	"SLOW" },
											{ "CLOSE",		"FAR",			"MEDIUM",	"SLOW" },
											{ "MEDIUM",		"CLOSE",		"SLOW",		"MEDIUM" },
											{ "MEDIUM",		"MEDIUM",		"MEDIUM",	"MEDIUM" },
											{ "MEDIUM",		"FAR",			"MEDIUM",	"SLOW" },
											{ "FAR",		"CLOSE",		"SLOW",		"FAST" },
											{ "FAR",		"MEDIUM",		"SLOW",		"MEDIUM" },
											{ "FAR",		"FAR",			"FAST",		"SLOW" } };

	struct MembershipFunction {
		float a;
		float b;
		float c;
		float d;
		string name;
		list<float> firingStrengths;
		float DOM;

		void setMembershipFunction(float aa, float bb, float cc, float dd, string nameMF)
		{
			a = aa;
			b = bb;
			c = cc;
			d = dd;
			name = nameMF;
		}

		float getDOM(float x) {
			if (x >= a && x <= b) {
				if (b == a)
					return 1;
				else
					return (x - a) / (b - a);
			}
			else if (x >= b && x <= c) {
				return 1;
			}
			else if (x >= c && x <= d) {
				if (d == c)
					return 1;
				else
					return (d - x) / (d - c);
			}
			else
				return 0;
		}

		float discretizationStep(int n) {
			return (d - a) / (n - 1);
		}

		float getCenterOfGravity() {
			/*Symmetric shape*/
			if (d - c == b - a) {
				if (c == b)
					return b;
				else
					return (c + b) / 2;
			}
			/*Asymmetric shape*/
			else {
				float numerator = 0, denominator = 0;
				int n = 10; //number of discretizations
				float DOMshape;
				float temp = a;
				float count = discretizationStep(n);

				for (int k = 0; k < n; k++) {
					DOMshape = getDOM(temp);
					numerator += temp * DOMshape;
					denominator += DOMshape;

					temp += count;
				}

				return numerator / denominator;
			}
		}
	};

	struct Input {
		MembershipFunction M[MFi];
		string name;

		void setInput(MembershipFunction m1, MembershipFunction m2, MembershipFunction m3, string nameI) {
			M[0] = m1;
			M[1] = m2;
			M[2] = m3;
			name = nameI;
		}

		void setDegreeOfMembership(int crisp) {
			//cout << "Input " << name << endl;

			for (int i = 0; i < MFi; i++) {
				M[i].DOM = M[i].getDOM(crisp);
				//cout << "DOM of " << M[i].name << " = " << M[i].DOM << endl;
			}
		}
	} *i1 = new Input, *i2 = new Input;

	struct Output {
		MembershipFunction M[MFo];
		string name;

		float getHeightDefuzzyfication() {
			float numerator = 0, denominator = 0;

			//cout << "output " << name << endl;
			for (int i = 0; i < MFo; i++) {
				for (std::list<float>::iterator it = M[i].firingStrengths.begin(); it != M[i].firingStrengths.end(); it++) {
					if (*it != 0) {
						numerator += *it*M[i].getCenterOfGravity();
						denominator += *it;
						//cout << "f = " << *it << ", CoG of " << M[i].name << " = " << M[i].getCenterOfGravity() << endl;
					}
				}
			}
			return numerator / denominator;
		}

		void setOutput(MembershipFunction m1, MembershipFunction m2, MembershipFunction m3, string nameO) {
			M[0] = m1;
			M[1] = m2;
			M[2] = m3;
			name = nameO;
		}
	} *o1 = new Output, *o2 = new Output;

	float getMax(list<float> l) {
		if (l.empty()) {
			//cout << "list empty" << endl;
			return 0;
		}

		l.sort();

		std::list<float>::iterator i = l.begin();
		std::advance(i, l.size() - 1);

		return *i;
	}

	float getMin(list<float> l) {
		if (l.empty())
			return 0;

		l.sort();

		std::list<float>::iterator i = l.begin();
		std::advance(i, 0);

		return *i;
	}

	void getFiringStreghts() {
		list<float> min;

		for (int i = 0; i < MFi*MFi; i++) {
			for (int k = 0; k < MFi; k++) {
				if (i1->M[k].name.compare(fuzzyRules[i][0]) == 0) {
					min.push_back(i1->M[k].DOM);
				}
				if (i2->M[k].name.compare(fuzzyRules[i][1]) == 0) {
					min.push_back(i2->M[k].DOM);
				}
			}
			for (int k = 0; k < MFo; k++) {
				if (o1->M[k].name.compare(fuzzyRules[i][2]) == 0) {
					o1->M[k].firingStrengths.push_back(getMin(min));
					//cout << "O1-Firing Strenght of " << o1->M[k].name << " = " << getMin(min) << endl;
				}
				if (o2->M[k].name.compare(fuzzyRules[i][3]) == 0) {
					o2->M[k].firingStrengths.push_back(getMin(min));
					//cout << "O2-Firing Strenght of " << o2->M[k].name << " = " << getMin(min) << endl;
				}
			}
			min.clear();
		}
	}

	void resetValues() {
		for (int i = 0; i < MFo; i++) {
			i1->M[i].DOM = 0;
			i2->M[i].DOM = 0;
			o1->M[i].firingStrengths.clear();
			o2->M[i].firingStrengths.clear();
		}
	}

	void initializeIO() {
		MembershipFunction m1[MFi], m2[MFi], m3[MFo];

		/*Membership Functions for Sensor 7*/
		m1[0].setMembershipFunction(0, 0, 250, 500, "CLOSE");
		m1[1].setMembershipFunction(400, 500, 500, 600, "MEDIUM");
		m1[2].setMembershipFunction(500, 750, 5000, 5000, "FAR");

		/*Membership Functions for Sensor 6*/
		m2[0].setMembershipFunction(20, 20, 346, 672, "CLOSE");
		m2[1].setMembershipFunction(542, 672, 672, 803, "MEDIUM");
		m2[2].setMembershipFunction(672, 999, 6547, 6547, "FAR");

		/*Membership Functions of Left and Right Motor Speed*/
		m3[0].setMembershipFunction(0, 0, 30, 250, "SLOW");
		m3[1].setMembershipFunction(0, 80, 80, 160, "MEDIUM");
		m3[2].setMembershipFunction(30, 160, 350, 350, "FAST");

		i1->setInput(m1[0], m1[1], m1[2], "S7");
		i2->setInput(m2[0], m2[1], m2[2], "S6");
		o1->setOutput(m3[0], m3[1], m3[2], "LMS");
		o2->setOutput(m3[0], m3[1], m3[2], "RMS");
	}

	void executeFuzzyfication(int S7, int S6) {
		i1->setDegreeOfMembership(S7);
		//cout << "-------" << endl;
		i2->setDegreeOfMembership(S6);
		//cout << "-------" << endl;

		/*Get Firing Strenghts of each row*/
		getFiringStreghts();
	}

} *ref = new REF;

void runREF(int s6, int s7, ArRobot *robot) {
	int LMS, RMS;

	::ref->initializeIO();

	::ref->executeFuzzyfication(s7, s6);

	LMS = ::ref->o1->getHeightDefuzzyfication();
	RMS = ::ref->o2->getHeightDefuzzyfication();
	robot->setVel2(LMS, RMS);

	cout << "distance6 = " << s6 << endl;
	cout << "distance7 = " << s7 << endl;
	cout << "LMS = " << LMS << ", RMS = " << RMS << endl;

	if (RMS > LMS)
		cout << "turning LEFT" << endl;
	else
		cout << "turning RIGHT" << endl;

	cout << "---------" << endl;

	::ref->resetValues();
}