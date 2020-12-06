#pragma once

class Actuators;

namespace Program
{
	class Basic
	{
	public:
		Basic(Actuators* act_mgr);
	private:
		Actuators* _act_mgr;
	};
}
