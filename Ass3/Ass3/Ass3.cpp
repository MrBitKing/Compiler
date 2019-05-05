

//#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC

#include <vector>
#include <iostream>
#include <crtdbg.h>

using namespace std;


void calculate();

int main() {


	char choice;
	bool running = true;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	while (running) {

		cout << "Earthquake Simulation" << endl;
		cout << "1. Run the simulation\n" << "2. Quit"<< endl;

		cin >> choice;

		switch (choice) {

		case '1':
			calculate();
			break;


		case '2':
			running = false;
			cout << "Quit" << endl;
			break;
		}
		cout << endl;
	}
	return 0;
}

void calculate(){


	float frequencie;
	float amplitude;
	float duration;
	double step = 0.1;
	double x0;
	double y0;
	int position;

	vector <float> gxy;
	vector <float> yprime;
	vector <float> gprime;
	vector <float> y;

	cout << "Frequency of the seismic wave in cycles per second(Hz)(e.g. 1.0) : ";	cin >> frequencie;

	cout << "Amplitude of the seismic wave in metres(e.g. 1.0) : ";	cin >> amplitude;

	cout << "How many seconds to run the simulation( e.g. 10) : ";
	cin >> duration;

	while (position < duration) {

		gxy[position] = x0 + y0;
		yprime[position] = y0 + (gxy[position] * step);
		gprime[position] = x0 + step + yprime[position];
		y = 


	}

}
