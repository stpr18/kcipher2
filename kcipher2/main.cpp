#include <iostream>
#include <iomanip>
#include "KCipher2.h"

int main()
{
	//test vector
	//https://tools.ietf.org/html/rfc7008

	{
		std::cout << "-----------------------" << std::endl;
		KCipher2 cipher({ 0x0F1E2D3C, 0x4B5A6978, 0x8796A5B4, 0xC3D2E1F0 }, { 0xF0E0D0C0, 0xB0A09080, 0x70605040, 0x30201000 });
		auto stream = cipher.get_stream();
		std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << stream.first << " " << stream.second << std::endl;
	}
	
	{
		std::cout << "-----------------------" << std::endl;
		KCipher2 cipher({ 0, 0, 0, 0 }, { 0, 0, 0, 0 });
		for (int i = 0; i < 8; ++i, cipher.next()) {
			auto stream = cipher.get_stream();
			std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << stream.first << " " << stream.second << std::endl;
		}
	}

	{
		std::cout << "-----------------------" << std::endl;
		KCipher2 cipher({ 0x3D62E9B1, 0x8E5B042F, 0x42DF43CC, 0x7175C96E }, { 0x777CEFE4, 0x541300C8, 0xADCACA8A, 0x0B48CD55 });
		for (int i = 0; i < 8; ++i, cipher.next()) {
			auto stream = cipher.get_stream();
			std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << stream.first << " " << stream.second << std::endl;
		}
	}

	std::system("pause");
	return EXIT_SUCCESS;
}
