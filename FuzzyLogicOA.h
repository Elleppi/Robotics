#include <string>
#include <iostream>
#include <list>

using namespace std;

struct OA {
	static const int nI = 2;		//Number of Inputs
	static const int nO = 2;		//Number of Outputs
	static const int MFi = 3;		//Number Membership Functions Input
	static const int MFo = 3;		//Number Membership Functions Output

											/*sensor2/3*/	/*sensor4/5*/	/*LMS*/		/*RMS*/
	string fuzzyRules[MFi*MFi][nI + nO] = { { "CLOSE",		"CLOSE",		"SLOW",		"FAST" },
											{ "CLOSE",		"MEDIUM",		"FAST",		"SLOW" },
											{ "CLOSE",		"FAR",			"FAST",		"SLOW" },
											{ "MEDIUM",		"CLOSE",		"SLOW",		"FAST" },
											{ "MEDIUM",		"MEDIUM",		"MEDIUM",	"MEDIUM" },
											{ "MEDIUM",		"FAR",			"SLOW",		"MEDIUM" },
											{ "FAR",		"CLOSE",		"SLOW",		"FAST" },
											{ "FAR",		"MEDIUM",		"SLOW",		"MEDIUM" },
											{ "FAR",		"FAR",			"MEDIUM",	"MEDIUM" } };

	struct MembershipFunction {
		float a;
		float b;
		float c;
		float d;
		string name;
		list<float> firingStrengths;
		float inputDOM;

		void setMembershipFunction(float aa, float bb, float cc, float dd, string nameMF)
		{
			a = aa;
			b = bb;
			c = cc;
			d = dd;
			name = nameMF;
		}

		float getDOM(int x) {
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
				float DOM1;
				float temp = a;
				float count = discretizationStep(n);

				for (int k = 0; k < n; k++) {
					DOM1 = getDOM(temp);
					numerator += temp * DOM1;
					denominator += DOM1;

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
			for (int i = 0; i < MFi; i++) {
				M[i].inputDOM = M[i].getDOM(crisp);
			}
		}
	} *i1 = new Input, *i2 = new Input;

	struct Output {
		MembershipFunction M[MFo];
		string name;

		float getHeightDefuzzyfication() {
			float numerator = 0, denominator = 0;

			for (int i = 0; i < MFo; i++) {
				for (std::list<float>::iterator it = M[i].firingStrengths.begin(); it != M[i].firingStrengths.end(); it++) {
					if (*it != 0) {
						numerator += *it*M[i].getCenterOfGravity();
						denominator += *it;
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
		if (l.empty())
			return 0;

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
					min.push_back(i1->M[k].inputDOM);
				}
				if (i2->M[k].name.compare(fuzzyRules[i][1]) == 0) {
					min.push_back(i2->M[k].inputDOM);
				}
			}
			for (int k = 0; k < MFo; k++) {
				if (o1->M[k].name.compare(fuzzyRules[i][2]) == 0) {
					o1->M[k].firingStrengths.push_back(getMin(min));
				}
				if (o2->M[k].name.compare(fuzzyRules[i][3]) == 0) {
					o2->M[k].firingStrengths.push_back(getMin(min));
				}
			}
			min.clear();
		}
	}

	void resetValues() {
		for (int i = 0; i < MFo; i++) {
			i1->M[i].inputDOM = 0;
			i2->M[i].inputDOM = 0;
			o1->M[i].firingStrengths.clear();
			o2->M[i].firingStrengths.clear();
		}
	}

	void initializeIO() {
		MembershipFunction m1[MFi], m2[MFo];

		/*Membership Functions for Sensor 3 & Sensor 4*/
		m1[0].setMembershipFunction(0, 0, 750, 1500, "CLOSE");
		m1[1].setMembershipFunction(1250, 1500, 1500, 1750, "MEDIUM");
		m1[2].setMembershipFunction(1500, 2000, 5000, 5000, "FAR");

		/*Membership Functions of Left and Right Motor Speed*/
		m2[0].setMembershipFunction(0, 0, 20, 60, "SLOW");
		m2[1].setMembershipFunction(0, 60, 60, 120, "MEDIUM");
		m2[2].setMembershipFunction(60, 200, 350, 350, "FAST");

		i1->setInput(m1[0], m1[1], m1[2], "S3");
		i2->setInput(m1[0], m1[1], m1[2], "S4");
		o1->setOutput(m2[0], m2[1], m2[2], "LMS");
		o2->setOutput(m2[0], m2[1], m2[2], "RMS");
	}

	void executeFuzzyfication(int S2, int S3, int S4, int S5) {
		if (S2 + 118 < S3)
			i1->setDegreeOfMembership(S2);
		else
			i1->setDegreeOfMembership(S3);
		if (S5 + 118 < S4)
			i2->setDegreeOfMembership(S5);
		else
			i2->setDegreeOfMembership(S4);

		/*Get Firing Strenghts of each row*/
		getFiringStreghts();
	}

} *oa = new OA;

void runOA(int s2, int s3, int s4, int s5, ArRobot *robot) {
	int LMS, RMS;

	::oa->initializeIO();

	::oa->executeFuzzyfication(s2, s3, s4, s5);
	LMS = ::oa->o1->getHeightDefuzzyfication();
	RMS = ::oa->o2->getHeightDefuzzyfication();
	robot->setVel2(LMS, RMS);

	cout << "distance2 = " << s2 << endl;
	cout << "distance3 = " << s3 << endl;
	cout << "distance4 = " << s4 << endl;
	cout << "distance5 = " << s5 << endl;
	cout << "LMS = " << LMS << ", RMS = " << RMS << endl;

	if (RMS > LMS)
		cout << "turning LEFT" << endl;
	else
		cout << "turning RIGHT" << endl;
	cout << "---------" << endl;

	::oa->resetValues();
}