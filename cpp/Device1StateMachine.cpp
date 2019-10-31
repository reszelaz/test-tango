/*----- PROTECTED REGION ID(Device1StateMachine.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        Device1StateMachine.cpp
//
// description : State machine file for the Device1 class
//
// project :     
//
// This file is part of Tango device class.
// 
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
// 
//
//
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================

#include <Device1.h>

/*----- PROTECTED REGION END -----*/	//	Device1::Device1StateMachine.cpp

//================================================================
//  States  |  Description
//================================================================


namespace Device1_ns
{
//=================================================
//		Attributes Allowed Methods
//=================================================

//--------------------------------------------------------
/**
 *	Method      : Device1::is_attr1_allowed()
 *	Description : Execution allowed for attr1 attribute
 */
//--------------------------------------------------------
bool Device1::is_attr1_allowed(TANGO_UNUSED(Tango::AttReqType type))
{

	//	Not any excluded states for attr1 attribute in read access.
	/*----- PROTECTED REGION ID(Device1::attr1StateAllowed_READ) ENABLED START -----*/
	
	/*----- PROTECTED REGION END -----*/	//	Device1::attr1StateAllowed_READ
	return true;
}


//=================================================
//		Commands Allowed Methods
//=================================================


/*----- PROTECTED REGION ID(Device1::Device1StateAllowed.AdditionalMethods) ENABLED START -----*/

//	Additional Methods

/*----- PROTECTED REGION END -----*/	//	Device1::Device1StateAllowed.AdditionalMethods

}	//	End of namespace
