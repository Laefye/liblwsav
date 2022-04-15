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
	if (status == ERROR_INVALID_FORMAT)
	{
		cout << "Invalid format" << endl;
		return 0;
	}
	for (auto k : data->get_data()->components)
	{
		cout << data->get_info()->map_components[k.type] << " Position: {x: " << k.pos[0] << ", y: " << k.pos[1] << ", z: " << k.pos[2] << "}" << endl;
	}
	delete data;
	return 0;
}
