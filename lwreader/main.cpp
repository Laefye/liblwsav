#include <liblwsav.h>
#include <iostream>

using namespace std;
using namespace LWSAV;

int main()
{
	Save* data = new Save("data.logicworld");
	int status = data->open();
	if (status == ERROR_NOT_FOUND)
	{
		cout << "data.logicworld not found" << endl;
		return 0;
	}
	status = data->read();
	if (status == ERROR_INVALID_FORMAT)
	{
		cout << "Invalid format" << endl;
		return 0;
	}
	cout << "Game Version: " << data->get_info()->game_verison[0] << "." << data->get_info()->game_verison[1] << "." << data->get_info()->game_verison[2] << "." << data->get_info()->game_verison[3] << endl;
	for (auto component : data->get_data()->components)
	{
		cout << "Component: " << data->get_info()->map_components[component.type] << endl;
	}
	delete data;
	return 0;
}
