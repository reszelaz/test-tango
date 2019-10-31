/*----- PROTECTED REGION ID(Device1.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        Device1.cpp
//
// description : C++ source for the Device1 class and its commands.
//               The class is derived from Device. It represents the
//               CORBA servant object which will be accessed from the
//               network. All commands which can be executed on the
//               Device1 are implemented in this file.
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
#include <Device1Class.h>

/*----- PROTECTED REGION END -----*/	//	Device1.cpp

/**
 *  Device1 class description:
 *    
 */

//================================================================
//  The following table gives the correspondence
//  between command and method names.
//
//  Command name  |  Method name
//================================================================
//  State         |  Inherited (no method)
//  Status        |  Inherited (no method)
//================================================================

//================================================================
//  Attributes managed is:
//================================================================
//  attr1  |  Tango::DevDouble	Scalar
//================================================================

namespace Device1_ns
{
/*----- PROTECTED REGION ID(Device1::namespace_starting) ENABLED START -----*/

//	static initializations

/*----- PROTECTED REGION END -----*/	//	Device1::namespace_starting

//--------------------------------------------------------
/**
 *	Method      : Device1::Device1()
 *	Description : Constructors for a Tango device
 *                implementing the classDevice1
 */
//--------------------------------------------------------
Device1::Device1(Tango::DeviceClass *cl, string &s)
 : TANGO_BASE_CLASS(cl, s.c_str())
{
	/*----- PROTECTED REGION ID(Device1::constructor_1) ENABLED START -----*/
	init_device();
	
	/*----- PROTECTED REGION END -----*/	//	Device1::constructor_1
}
//--------------------------------------------------------
Device1::Device1(Tango::DeviceClass *cl, const char *s)
 : TANGO_BASE_CLASS(cl, s)
{
	/*----- PROTECTED REGION ID(Device1::constructor_2) ENABLED START -----*/
	init_device();
	
	/*----- PROTECTED REGION END -----*/	//	Device1::constructor_2
}
//--------------------------------------------------------
Device1::Device1(Tango::DeviceClass *cl, const char *s, const char *d)
 : TANGO_BASE_CLASS(cl, s, d)
{
	/*----- PROTECTED REGION ID(Device1::constructor_3) ENABLED START -----*/
	init_device();
	
	/*----- PROTECTED REGION END -----*/	//	Device1::constructor_3
}

//--------------------------------------------------------
/**
 *	Method      : Device1::delete_device()
 *	Description : will be called at device destruction or at init command
 */
//--------------------------------------------------------
void Device1::delete_device()
{
	DEBUG_STREAM << "Device1::delete_device() " << device_name << endl;
	/*----- PROTECTED REGION ID(Device1::delete_device) ENABLED START -----*/
	
	//	Delete device allocated objects
	
	/*----- PROTECTED REGION END -----*/	//	Device1::delete_device
	delete[] attr_attr1_read;
}

//--------------------------------------------------------
/**
 *	Method      : Device1::init_device()
 *	Description : will be called at device initialization.
 */
//--------------------------------------------------------
void Device1::init_device()
{
	DEBUG_STREAM << "Device1::init_device() create device " << device_name << endl;
	/*----- PROTECTED REGION ID(Device1::init_device_before) ENABLED START -----*/
	
	//	Initialization before get_device_property() call
	
	/*----- PROTECTED REGION END -----*/	//	Device1::init_device_before
	
	//	No device property to be read from database
	
	attr_attr1_read = new Tango::DevDouble[1];
	/*----- PROTECTED REGION ID(Device1::init_device) ENABLED START -----*/
	
	//	Initialize device
	
	/*----- PROTECTED REGION END -----*/	//	Device1::init_device
}


//--------------------------------------------------------
/**
 *	Method      : Device1::always_executed_hook()
 *	Description : method always executed before any command is executed
 */
//--------------------------------------------------------
void Device1::always_executed_hook()
{
	DEBUG_STREAM << "Device1::always_executed_hook()  " << device_name << endl;
	/*----- PROTECTED REGION ID(Device1::always_executed_hook) ENABLED START -----*/
	
	//	code always executed before all requests
	
	/*----- PROTECTED REGION END -----*/	//	Device1::always_executed_hook
}

//--------------------------------------------------------
/**
 *	Method      : Device1::read_attr_hardware()
 *	Description : Hardware acquisition for attributes
 */
//--------------------------------------------------------
void Device1::read_attr_hardware(TANGO_UNUSED(vector<long> &attr_list))
{
	DEBUG_STREAM << "Device1::read_attr_hardware(vector<long> &attr_list) entering... " << endl;
	/*----- PROTECTED REGION ID(Device1::read_attr_hardware) ENABLED START -----*/
	
	//	Add your own code
	
	/*----- PROTECTED REGION END -----*/	//	Device1::read_attr_hardware
}

//--------------------------------------------------------
/**
 *	Read attribute attr1 related method
 *	Description: 
 *
 *	Data type:	Tango::DevDouble
 *	Attr type:	Scalar
 */
//--------------------------------------------------------
void Device1::read_attr1(Tango::Attribute &attr)
{
	DEBUG_STREAM << "Device1::read_attr1(Tango::Attribute &attr) entering... " << endl;
	/*----- PROTECTED REGION ID(Device1::read_attr1) ENABLED START -----*/
	//	Set the attribute value
	MyCallBack *callback = new MyCallBack;

    Tango::DeviceProxy mydevice = Tango::DeviceProxy("sys/tg_test/1");

    int event_id;
    const string attr_name("double_scalar");
    event_id = mydevice.subscribe_event(attr_name,
                                        Tango::ATTR_CONF_EVENT,
                                        callback);
    cout << "id = " << event_id << endl;

	attr.set_value(attr_attr1_read);
	
	/*----- PROTECTED REGION END -----*/	//	Device1::read_attr1
}

//--------------------------------------------------------
/**
 *	Method      : Device1::add_dynamic_attributes()
 *	Description : Create the dynamic attributes if any
 *                for specified device.
 */
//--------------------------------------------------------
void Device1::add_dynamic_attributes()
{
	/*----- PROTECTED REGION ID(Device1::add_dynamic_attributes) ENABLED START -----*/
	
	//	Add your own code to create and add dynamic attributes if any
	
	/*----- PROTECTED REGION END -----*/	//	Device1::add_dynamic_attributes
}

//--------------------------------------------------------
/**
 *	Method      : Device1::add_dynamic_commands()
 *	Description : Create the dynamic commands if any
 *                for specified device.
 */
//--------------------------------------------------------
void Device1::add_dynamic_commands()
{
	/*----- PROTECTED REGION ID(Device1::add_dynamic_commands) ENABLED START -----*/
	
	//	Add your own code to create and add dynamic commands if any
	
	/*----- PROTECTED REGION END -----*/	//	Device1::add_dynamic_commands
}

// /*----- PROTECTED REGION ID(Device1::namespace_ending) ENABLED START -----*/

void MyCallBack::push_event(Tango::AttrConfEventData *myevent)
{
    
}

/*----- PROTECTED REGION END -----*/	//	Device1::namespace_ending
} //	namespace