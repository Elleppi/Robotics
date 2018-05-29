#include "FuzzyLogicCDB.h"
#include "pidREF.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	Aria::init();
	ArRobot robot;
	ArSensorReading *sonarSensor[8];
	int sonarRange[8];
	list<int> sonar2, sonar3, sonar4, sonar5, sonar6, sonar7;

	/*****ROBOT INITIALIZATION*****/
	ArArgumentParser argParser(&argc, argv);
	argParser.loadDefaultArguments();

	ArRobotConnector robotConnector(&argParser, &robot);

	if (robotConnector.connectRobot())
		cout << "Robot connected !!" << endl;
	else
		cout << "connection failed";

	robot.runAsync(false);
	robot.lock();
	robot.enableMotors();
	robot.unlock();

	int answer;

	cout << "\nType:\n1 for CDB\n2 for REF\n3 for OA\n4 for PID\n" << endl;
	cin >> answer;

	while (1) {
		/*Take 5 sensor readings*/
		for (int k = 0; k < 5; k++) {
			for (int i = 2; i < 8; i++) {
				sonarSensor[i] = robot.getSonarReading(i);
				sonarRange[i] = sonarSensor[i]->getRange();
			}
			sonar2.push_back(sonarRange[2]);
			sonar3.push_back(sonarRange[3]);
			sonar4.push_back(sonarRange[4]);
			sonar5.push_back(sonarRange[5]);
			sonar6.push_back(sonarRange[6]);
			sonar7.push_back(sonarRange[7]);
		}

		if (answer == 1)
			runCDB(getAverage(sonar2), getAverage(sonar3), getAverage(sonar4), getAverage(sonar5), getAverage(sonar6), getAverage(sonar7), &robot);
		if (answer == 2)
			runREF(getAverage(sonar6), getAverage(sonar7), &robot);
		if (answer == 3)
			runOA(getAverage(sonar2), getAverage(sonar3), getAverage(sonar4), getAverage(sonar5), &robot);
		if (answer == 4)
			runPID(getAverage(sonar4), getAverage(sonar5), getAverage(sonar6), getAverage(sonar7), &robot);

		/*Clear the sensor readings lists*/
		sonar2.clear();
		sonar3.clear();
		sonar4.clear();
		sonar5.clear();
		sonar6.clear();
		sonar7.clear();

		ArUtil::sleep(100);
	}

	robot.lock();
	robot.stop();
	robot.unlock();

	Aria::exit();

	return 0;
}