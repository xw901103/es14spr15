#include <iostream>

int main(int argc, char **argv)
{
	int harmonic = 0, angle = 0;
	double maginitude = 0.0;
	
	if (argc == 7)
	{
		for (int i = 0; i < argc; ++i)
		{
			if (argv[i][0] == '-')
			{
				if (argv[i][1] == 'h')
					harmonic = atoi(argv[i + 1]);
				else if (argv[i][1] == 'm')
					maginitude = atof(argv[i + 1]);
				else if (argv[i][1] == 'a')
					angle = atoi(argv[i + 1]);
			}
		}
		int mag = static_cast<int>(1 / maginitude);
		int result = harmonic << 20;
		result = result | (angle & 0x3FF) << 10;
		result = result | (mag & 0x3FF);
		std::cout << std::hex << result << std::endl;
	}
	else
	{
		std::cout << "usage: -h num -m num -a num" << std::endl;
		std::cout << "-h for harmonic number -m for magnitude -a for angle" << std::endl;
	}
	return 0;
}
