#include "ServiceGraph.cpp"

class RuntimeServiceGraph : public ServiceGraph {
	
	map<string, string> machine_bridge_ips;

public:

	/**
	 * Sets the bridge IP created for the machine with the given machine ID.
	 *
	 * @param macid 	A machine id.
	 * @param bridge_ip The IP of the bridge.
	 */
	void set_bridge_ip(string macid, string bridge_ip) {
		machine_bridge_ips[macid] = bridge_ip;
	}

	/**
	 * Gets the bridge IP created for the machine with the given machine ID.
	 *
	 * @param macid A machine id.
	 */
	string get_bridge_ip(string macid) {
		if (machine_bridge_ips.count(macid) == 0) {
			throw invalid_argument("The given machine id does not exist");
		}
		return machine_bridge_ips[macid];
	}
};