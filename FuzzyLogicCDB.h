#include <string>
#include <iostream>
#include <list>
#include "FuzzyLogicREF.h"
#include "FuzzyLogicOA.h"
#include <Aria.h>

using namespace std;

struct CDB {
	static const int nI = 3;			//Number of Inputs
	static const int nO = 1;			//Number of Outputs
	static const int MFi = 2;			//Number Membership Functions Input
	static const int MFo = 2;			//Number Membership Functions Output

												/*sensor3*/	/*sensor4*/	/*sensor7*/	/*BEHAVIOUR*/
	string fuzzyRules[MFi*MFi*MFi][nI + nO] = { { "CLOSE",	"CLOSE",	"CLOSE",	"OA" },
												{ "CLOSE",	"CLOSE",	"FAR",		"OA" },
												{ "CLOSE",	"FAR",		"CLOSE",	"REF" },
												{ "CLOSE",	"FAR",		"FAR",		"REF" },
												{ "FAR",	"CLOSE",	"CLOSE",	"OA" },
												{ "FAR",	"CLOSE",	"FAR",		"REF" },
												{ "FAR",	"FAR",		"CLOSE",	"REF" },
												{ "FAR",	"FAR",		"FAR",		"REF" } };

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

		void setInput(MembershipFunction m1, MembershipFunction m2, string nameI) {
			M[0] = m1;
			M[1] = m2;
			name = nameI;
		}

		void setDegreeOfMembership(int crisp) {
			for (int i = 0; i < MFi; i++) {
				M[i].inputDOM = M[i].getDOM(crisp);
				//cout << "DOM of " << M[i].name << " = " << M[i].inputDOM << endl;
			}
		}
	} *i1 = new Input, *i2 = new Input, *i3 = new Input;

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

		void setOutput(MembershipFunction m1, MembershipFunction m2, string nameO) {
			M[0] = m1;
			M[1] = m2;
			name = nameO;
		}
	} *o = new Output;

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

		for (int i = 0; i < MFi*MFi*MFi; i++) {
			for (int k = 0; k < MFi; k++) {
				if (i1->M[k].name.compare(fuzzyRules[i][0]) == 0) {
					min.push_back(i1->M[k].inputDOM);
					//cout << "i1 MF " << i1->M[k].name << " DOM = " << i1->M[k].inputDOM << endl;
				}
				if (i2->M[k].name.compare(fuzzyRules[i][1]) == 0) {
					min.push_back(i2->M[k].inputDOM);
					//cout << "i2 MF " << i2->M[k].name << " DOM = " << i2->M[k].inputDOM << endl;
				}
				if (i3->M[k].name.compare(fuzzyRules[i][2]) == 0) {
					min.push_back(i3->M[k].inputDOM);
					//cout << "i3 MF " << i3->M[k].name << " DOM = " << i3->M[k].inputDOM << endl;
				}
			}
			for (int k = 0; k < MFo; k++) {
				if (o->M[k].name.compare(fuzzyRules[i][3]) == 0) {
					o->M[k].firingStrengths.push_back(getMin(min));
					//cout << "o MF " << o->M[k].name << " min = " << getMin(min) << endl;
				}
			}
			//cout << "++++++" << endl;
			min.clear();
		}
	}

	void resetValues() {
		for (int i = 0; i < MFo; i++) {
			i1->M[i].inputDOM = 0;
			i2->M[i].inputDOM = 0;
			i3->M[i].inputDOM = 0;
			o->M[i].firingStrengths.clear();
		}
	}

	void initializeIO() {
		MembershipFunction m1[MFi], m2[MFi];

		/*Membership Functions for Sensor 3, Sensor 4 & Sensor 7*/
		m1[0].setMembershipFunction(0, 0, 500, 3000, "CLOSE");
		m1[1].setMembershipFunction(500, 3000, 5000, 5000, "FAR");

		/*Membership Functions for the behaviours*/
		m2[0].setMembershipFunction(0, 0, 5, 50, "REF");
		m2[1].setMembershipFunction(50, 95, 100, 100, "OA");

		i1->setInput(m1[0], m1[1], "S3");
		i2->setInput(m1[0], m1[1], "S4");
		i3->setInput(m1[0], m1[1], "S7");

		o->setOutput(m2[0], m2[1], "BEHAVIOUR");
	}

	void executeFuzzyfication(int S3, int S4, int S7) {
		i1->setDegreeOfMembership(S3);
		i2->setDegreeOfMembership(S4);
		i3->setDegreeOfMembership(S7);

		/*Get Firing Strenghts of each row*/
		getFiringStreghts();
	}

} *cdb = new CDB;

int getAverage(list<int> l) {
	int count = 0;

	for (std::list<int>::iterator i = l.begin(); i != l.end(); i++)
		count += *i;

	return count / l.size();
}

void runCDB(int s2, int s3, int s4, int s5, int s6, int s7, ArRobot *robot) {
	::cdb->initializeIO();
	::oa->initializeIO();
	::ref->initializeIO();

	::cdb->executeFuzzyfication(s3, s4, s7);
	::oa->executeFuzzyfication(s2, s3, s4, s5);
	::ref->executeFuzzyfication(s7, s6);

	int CDB = ::cdb->o->getHeightDefuzzyfication();
	int OAlms = ::oa->o1->getHeightDefuzzyfication();
	int OArms = ::oa->o2->getHeightDefuzzyfication();
	int REFlms = ::ref->o1->getHeightDefuzzyfication();
	int REFrms = ::ref->o2->getHeightDefuzzyfication();

	cout << "DEFUZZIFICATION = " << CDB << "% OA, " << (100 - CDB) << "% REF" << endl;
	/*cout << "OAlms defuzzy = " << OAlms << endl;
	cout << "OArms defuzzy = " << OArms << endl;
	cout << "REFlms defuzzy = " << REFlms << endl;
	cout << "REFrms defuzzy = " << REFrms << endl;*/

	int LMS = (CDB*OAlms + (100 - CDB)*REFlms) / 100;
	int RMS = (CDB*OArms + (100 - CDB)*REFrms) / 100;

	robot->setVel2(LMS, RMS);

	cout << "distance3 = " << s3 << endl;
	cout << "distance4 = " << s4 << endl;
	cout << "distance7 = " << s7 << endl;

	cout << "LMS = " << LMS << endl;
	cout << "RMS = " << RMS << endl;

	cout << "---------" << endl;

	::cdb->resetValues();
	::oa->resetValues();
	::ref->resetValues();
}