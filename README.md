# Demo a bug in (Py)Tango? 

Example to reproduce a problem with the concurrent subscription to attributes and
destruction of DeviceProxies. This problem is reproducible with both python2 and python3.

The architecture is as following. We have one DS called `DeviceServer` which exports in
total two devices, each of a different class. These two classes are:
* `Device1` with one attribute `attr1` and one command `cmd1`
* `Device2` with one attribute `attr2` and one command `cmd2`

## Demo with attributes readout

When `attr1` is read it creates a disposable `DeviceProxy` to `sys/tg_test/1` and
subscribes to `ATTR_CONF_EVENT` of `double_scalar`.

When `attr2` is read it just creates a disposable `DeviceProxy` to `sys/tg_test/1`.

There are also two client scripts `client1.py` and `client2.py` which read in a loop
`attr1` and `attr2` respectively.

When running in parallel the `DeviceServer`, `client1.py` and `client2.py`,
after just few seconds both clients stop with: `TRANSIENT CORBA system exception: TRANSIENT_CallTimedout`
and the device server gets hung. Any Tango request to it also gives:
`TRANSIENT CORBA system exception: TRANSIENT_CallTimedout` 
 
When analyzing the threads of the device server there are two of them which seems to
block each other:

```
Thread 15 (Thread 0x7f0b917fa700 (LWP 17627)):
#0  pthread_cond_wait@@GLIBC_2.3.2 () at ../sysdeps/unix/sysv/linux/x86_64/pthread_cond_wait.S:185
#1  0x00007f0bb1f86c60 in omni_condition::wait() () from /usr/lib/libomnithread.so.3
#2  0x00007f0bb3954758 in ReadersWritersLock::readerIn (this=<optimized out>) at ../../../lib/cpp/server/readers_writers_lock.h:66
#3  ReaderLock::ReaderLock (l=..., this=<synthetic pointer>) at ../../../lib/cpp/server/readers_writers_lock.h:176
#4  Tango::ZmqEventConsumer::get_subscribed_event_ids (this=<optimized out>, _dev=_dev@entry=0x7f0b70005bd0, 
    _ids=std::vector of length 0, capacity 0) at zmqeventconsumer.cpp:3341
#5  0x00007f0bb389173c in Tango::DeviceProxy::~DeviceProxy (this=0x7f0b70005bd0, __in_chrg=<optimized out>) at devapi_base.cpp:2493
#6  0x00007f0bb38918f9 in Tango::DeviceProxy::~DeviceProxy (this=0x7f0b70005bd0, __in_chrg=<optimized out>) at devapi_base.cpp:2531
#7  0x00007f0bb446778a in boost::detail::sp_counted_base::release (this=0x7f0b700052f0)
    at /usr/include/boost/smart_ptr/detail/sp_counted_base_gcc_x86.hpp:146
#8  0x00007f0bb447e6cd in boost::detail::sp_counted_base::release (this=<optimized out>)
    at /usr/include/boost/python/object/pointer_holder.hpp:52
#9  boost::detail::shared_count::~shared_count (this=0x7f0b700063e8, __in_chrg=<optimized out>)
    at /usr/include/boost/smart_ptr/detail/shared_count.hpp:473
#10 boost::shared_ptr<Tango::DeviceProxy>::~shared_ptr (this=0x7f0b700063e0, __in_chrg=<optimized out>)
    at /usr/include/boost/smart_ptr/shared_ptr.hpp:336
#11 boost::python::objects::pointer_holder<boost::shared_ptr<Tango::DeviceProxy>, Tango::DeviceProxy>::~pointer_holder (
    this=0x7f0b700063d0, __in_chrg=<optimized out>) at /usr/include/boost/python/object/pointer_holder.hpp:52
#12 0x00007f0bb346570c in ?? () from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#13 0x0000556036c18613 in ?? ()
#14 0x0000556036bcb4c8 in PyEval_EvalFrameEx ()
#15 0x0000556036bb716f in PyEval_EvalCodeEx ()
#16 0x0000556036c011ef in ?? ()
#17 0x0000556036c4ae17 in PyObject_Call ()
#18 0x0000556036bc784e in PyEval_EvalFrameEx ()
#19 0x0000556036bceb06 in ?? ()
#20 0x0000556036bca9e6 in PyEval_EvalFrameEx ()
#21 0x0000556036bb78a7 in PyEval_EvalCodeEx ()
#22 0x0000556036c010e3 in ?? ()
#23 0x0000556036c4ae17 in PyObject_Call ()
#24 0x0000556036b6834e in ?? ()
#25 0x0000556036c4ae17 in PyObject_Call ()
#26 0x0000556036bb5f30 in PyEval_CallObjectWithKeywords ()
#27 0x0000556036c96ab1 in PyEval_CallMethod ()
#28 0x00007f0bb44eb15b in boost::python::call_method<void, boost::reference_wrapper<Tango::Attribute> > (self=0x7f0b9a168420, 
    name=0x556037d648d0 "__read_attr2_wrapper__", a0=...) at /usr/include/boost/python/call_method.hpp:66
#29 0x00007f0bb44e2a78 in PyAttr::read (this=0x556037d79f88, dev=<optimized out>, att=...)
    at /home/zreszela/workspace/pytango/ext/server/attr.cpp:76
#30 0x00007f0bb39fb910 in Tango::Device_3Impl::read_attributes_no_except (this=this@entry=0x556037d95d70, names=..., aid=..., 
    second_try=second_try@entry=true, idx=std::vector of length 1, capacity 1 = {...}) at device_3.cpp:541
#31 0x00007f0bb3a17df7 in Tango::Device_5Impl::read_attributes_5 (this=0x556037d95d70, names=..., source=<optimized out>, cl_id=...)
    at device_5.cpp:340
#32 0x00007f0bb3bc805b in _0RL_lcfn_6fe2f94a21a10053_84000000 (cd=0x7f0b917f9aa0, svnt=<optimized out>) at tangoSK.cpp:6272
#33 0x00007f0bb2278b69 in omniCallHandle::upcall(omniServant*, omniCallDescriptor&) () from /usr/lib/libomniORB4.so.1
#34 0x00007f0bb3bca1b0 in Tango::_impl_Device_5::_dispatch (this=<optimized out>, _handle=...) at tangoSK.cpp:7158
#35 0x00007f0bb2271f6d in omni::omniOrbPOA::dispatch(omniCallHandle&, omniLocalIdentity*) () from /usr/lib/libomniORB4.so.1
#36 0x00007f0bb2251116 in omniLocalIdentity::dispatch(omniCallHandle&) () from /usr/lib/libomniORB4.so.1
#37 0x00007f0bb2291d20 in omni::GIOP_S::handleRequest() () from /usr/lib/libomniORB4.so.1
#38 0x00007f0bb2292a98 in omni::GIOP_S::dispatcher() () from /usr/lib/libomniORB4.so.1
#39 0x00007f0bb228f6c5 in omni::giopWorker::real_execute() () from /usr/lib/libomniORB4.so.1
#40 0x00007f0bb228fd4f in omni::giopWorker::execute() () from /usr/lib/libomniORB4.so.1
#41 0x00007f0bb224602d in omniAsyncWorkerInfo::run() () from /usr/lib/libomniORB4.so.1
#42 0x00007f0bb3b84c4c in Tango::create_PyPerThData (info=...) at utils.cpp:3234
#43 0x00007f0bb2245f24 in omniAsyncWorkerInfo::run() () from /usr/lib/libomniORB4.so.1
#44 0x00007f0bb22465ff in omniAsyncWorker::run(void*) () from /usr/lib/libomniORB4.so.1
#45 0x00007f0bb1f876d9 in omni_thread_wrapper () from /usr/lib/libomnithread.so.3
#46 0x00007f0bb5a5e494 in start_thread (arg=0x7f0b917fa700) at pthread_create.c:333
#47 0x00007f0bb4c51acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:97

Thread 10 (Thread 0x7f0b93fff700 (LWP 17597)):
#0  pthread_cond_timedwait@@GLIBC_2.3.2 () at ../sysdeps/unix/sysv/linux/x86_64/pthread_cond_timedwait.S:225
#1  0x0000556036bc4e6a in ?? ()
#2  0x0000556036bc50d3 in PyEval_RestoreThread ()
#3  0x0000556036c95c4a in PyGILState_Ensure ()
#4  0x00007f0bb43c76ca in AutoPythonGIL::AutoPythonGIL (safe=true, this=<synthetic pointer>)
    at /home/zreszela/workspace/pytango/ext/pytgutils.h:48
#5  (anonymous namespace)::_push_event<Tango::AttrConfEventData> (ev=0x7f0b80018040, self=0x7f0b8002cdf0)
    at /home/zreszela/workspace/pytango/ext/callback.cpp:228
#6  PyCallBackPushEvent::push_event (this=0x7f0b8002cdf0, ev=0x7f0b80018040) at /home/zreszela/workspace/pytango/ext/callback.cpp:332
#7  0x00007f0bb3932c2f in Tango::EventConsumer::get_fire_sync_event (this=this@entry=0x7f0b8000c5a0, device=device@entry=0x7f0b800067f0, 
    callback=callback@entry=0x7f0b8002cdf0, ev_queue=ev_queue@entry=0x0, event=event@entry=Tango::ATTR_CONF_EVENT, 
    event_name="idl5_attr_conf", obj_name="double_scalar", cb=..., 
    callback_key="tango://pc255.cells.es:10000/sys/tg_test/1/double_scalar.idl5_attr_conf") at event.cpp:3265
#8  0x00007f0bb3934492 in Tango::EventConsumer::connect_event (this=this@entry=0x7f0b8000c5a0, device=device@entry=0x7f0b800067f0, 
    obj_name="double_scalar", event=event@entry=Tango::ATTR_CONF_EVENT, callback=callback@entry=0x7f0b8002cdf0, 
    ev_queue=ev_queue@entry=0x0, filters=std::vector of length 0, capacity 0, event_name="idl5_attr_conf", event_id=<optimized out>)
    at event.cpp:1809
#9  0x00007f0bb3935de4 in Tango::EventConsumer::subscribe_event (this=0x7f0b8000c5a0, device=device@entry=0x7f0b800067f0, 
    attribute="double_scalar", event=event@entry=Tango::ATTR_CONF_EVENT, callback=callback@entry=0x7f0b8002cdf0, 
    ev_queue=ev_queue@entry=0x0, filters=std::vector of length 0, capacity 0, stateless=false) at event.cpp:1261
#10 0x00007f0bb3936350 in Tango::EventConsumer::subscribe_event (this=<optimized out>, device=device@entry=0x7f0b800067f0, 
    attribute="double_scalar", event=event@entry=Tango::ATTR_CONF_EVENT, callback=callback@entry=0x7f0b8002cdf0, 
    filters=std::vector of length 0, capacity 0, stateless=false) at event.cpp:1140
#11 0x00007f0bb3849312 in Tango::DeviceProxy::subscribe_event (this=0x7f0b800067f0, attr_name="double_scalar", 
    event=Tango::ATTR_CONF_EVENT, callback=0x7f0b8002cdf0, filters=std::vector of length 0, capacity 0, stateless=<optimized out>)
    at devapi_base.cpp:7708
#12 0x00007f0bb4467b1a in PyDeviceProxy::subscribe_event_attrib (py_self=..., attr_name="double_scalar", event=Tango::ATTR_CONF_EVENT, 
    py_cb_or_queuesize=..., py_filters=..., stateless=<optimized out>, extract_as=PyTango::ExtractAsNumpy)
    at /home/zreszela/workspace/pytango/ext/device_proxy.cpp:609
#13 0x00007f0bb447670a in boost::python::detail::invoke<boost::python::to_python_value<int const&>, int (*)(boost::python::api::object, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, Tango::EventType, boost::python::api::object, boost::python::api::object&, bool, PyTango::ExtractAs), boost::python::arg_from_python<boost::python::api::object>, boost::python::arg_from_python<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>, boost::python::arg_from_python<Tango::EventType>, boost::python::arg_from_python<boost::python::api::object>, boost::python::arg_from_python<boost::python::api::object&>, boost::python::arg_from_python<bool>, boost::python::arg_from_python<PyTango::ExtractAs> > (ac6=..., ac5=..., ac4=..., ac3=<synthetic pointer>..., ac2=..., 
    ac1=..., ac0=<synthetic pointer>..., f=<optimized out>, rc=...) at /usr/include/boost/python/detail/invoke.hpp:75
#14 boost::python::detail::caller_arity<7u>::impl<int (*)(boost::python::api::object, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, Tango::EventType, boost::python::api::object, boost::python::api::object&, bool, PyTango::ExtractAs), boost::python::default_call_policies, boost::mpl::vector8<int, boost::python::api::object, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, Tango::EventType, boost::python::api::object, boost::python::api::object&, bool, PyTango::ExtractAs> >::operator() (args_=<optimized out>, this=<optimized out>) at /usr/include/boost/python/detail/caller.hpp:223
#15 boost::python::objects::caller_py_function_impl<boost::python::detail::caller<int (*)(boost::python::api::object, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, Tango::EventType, boost::python::api::object, boost::python::api::object&, bool, PyTango::ExtractAs), boost::python::default_call_policies, boost::mpl::vector8<int, boost::python::api::object, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, Tango::EventType, boost::python::api::object, boost::python::api::object&, bool, PyTango::ExtractAs> > >::operator() (this=<optimized out>, args=<optimized out>, kw=<optimized out>)
    at /usr/include/boost/python/object/py_function.hpp:38
#16 0x00007f0bb34694ad in boost::python::objects::function::call(_object*, _object*) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#17 0x00007f0bb34696a8 in ?? () from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#18 0x00007f0bb347033b in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#19 0x00007f0bb44a3b83 in boost::python::detail::translate_exception<Tango::NotAllowed, void (*)(Tango::NotAllowed const&)>::operator() (
    this=<optimized out>, translate=0x7f0bb449d1d0 <translate_not_allowed(Tango::NotAllowed const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
---Type <return> to continue, or q <return> to quit---
#20 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NotAllowed const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::NotAllowed, void (*)(Tango::NotAllowed const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#21 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::NotAllowed, void (*)(Tango::NotAllowed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NotAllowed const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>) at /usr/include/boost/bind/bind.hpp:1318
#22 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::NotAllowed, void (*)(Tango::NotAllowed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NotAllowed const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., a0=..., a1=...)
    at /usr/include/boost/function/function_template.hpp:138
#23 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#24 0x00007f0bb44a3b33 in boost::python::detail::translate_exception<Tango::DeviceUnlocked, void (*)(Tango::DeviceUnlocked const&)>::operator() (this=<optimized out>, translate=0x7f0bb449d150 <translate_device_unlocked(Tango::DeviceUnlocked const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
#25 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::DeviceUnlocked const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::DeviceUnlocked, void (*)(Tango::DeviceUnlocked const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#26 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::DeviceUnlocked, void (*)(Tango::DeviceUnlocked const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::DeviceUnlocked const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#27 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::DeviceUnlocked, void (*)(Tango::DeviceUnlocked const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::DeviceUnlocked const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., 
    a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#28 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#29 0x00007f0bb44a3ae3 in boost::python::detail::translate_exception<Tango::EventSystemFailed, void (*)(Tango::EventSystemFailed const&)>::operator() (this=<optimized out>, translate=0x7f0bb449d0d0 <translate_event_system_failed(Tango::EventSystemFailed const&)>, f=..., 
    handler=...) at /usr/include/boost/python/detail/translate_exception.hpp:48
#30 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::EventSystemFailed const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::EventSystemFailed, void (*)(Tango::EventSystemFailed const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#31 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::EventSystemFailed, void (*)(Tango::EventSystemFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::EventSystemFailed const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#32 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::EventSystemFailed, void (*)(Tango::EventSystemFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::EventSystemFailed const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (
    function_obj_ptr=..., a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#33 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#34 0x00007f0bb44a3a93 in boost::python::detail::translate_exception<Tango::AsynReplyNotArrived, void (*)(Tango::AsynReplyNotArrived const&)>::operator() (this=<optimized out>, translate=0x7f0bb449d050 <translate_asyn_reply_not_arrived(Tango::AsynReplyNotArrived const&)>, 
    f=..., handler=...) at /usr/include/boost/python/detail/translate_exception.hpp:48
---Type <return> to continue, or q <return> to quit---
#35 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::AsynReplyNotArrived const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::AsynReplyNotArrived, void (*)(Tango::AsynReplyNotArrived const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#36 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::AsynReplyNotArrived, void (*)(Tango::AsynReplyNotArrived const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::AsynReplyNotArrived const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#37 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::AsynReplyNotArrived, void (*)(Tango::AsynReplyNotArrived const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::AsynReplyNotArrived const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (
    function_obj_ptr=..., a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#38 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#39 0x00007f0bb44a3a43 in boost::python::detail::translate_exception<Tango::AsynCall, void (*)(Tango::AsynCall const&)>::operator() (
    this=<optimized out>, translate=0x7f0bb449cfd0 <translate_asyn_call(Tango::AsynCall const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
#40 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::AsynCall const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::AsynCall, void (*)(Tango::AsynCall const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>) at /usr/include/boost/bind/bind.hpp:388
#41 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::AsynCall, void (*)(Tango::AsynCall const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::AsynCall const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>) at /usr/include/boost/bind/bind.hpp:1318
#42 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::AsynCall, void (*)(Tango::AsynCall const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::AsynCall const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., a0=..., a1=...)
    at /usr/include/boost/function/function_template.hpp:138
#43 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#44 0x00007f0bb44a39f3 in boost::python::detail::translate_exception<Tango::NonSupportedFeature, void (*)(Tango::NonSupportedFeature const&)>::operator() (this=<optimized out>, translate=0x7f0bb449cf50 <translate_non_supported_feature(Tango::NonSupportedFeature const&)>, 
    f=..., handler=...) at /usr/include/boost/python/detail/translate_exception.hpp:48
#45 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NonSupportedFeature const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::NonSupportedFeature, void (*)(Tango::NonSupportedFeature const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#46 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::NonSupportedFeature, void (*)(Tango::NonSupportedFeature const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NonSupportedFeature const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#47 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::NonSupportedFeature, void (*)(Tango::NonSupportedFeature const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NonSupportedFeature const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (
    function_obj_ptr=..., a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#48 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#49 0x00007f0bb44a39a3 in boost::python::detail::translate_exception<Tango::WrongData, void (*)(Tango::WrongData const&)>::operator() (
    this=<optimized out>, translate=0x7f0bb449ced0 <translate_wrong_data(Tango::WrongData const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
#50 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::WrongData const&)> >::operator()<bool, boost::python:---Type <return> to continue, or q <return> to quit---
:detail::translate_exception<Tango::WrongData, void (*)(Tango::WrongData const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#51 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::WrongData, void (*)(Tango::WrongData const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::WrongData const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>) at /usr/include/boost/bind/bind.hpp:1318
#52 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::WrongData, void (*)(Tango::WrongData const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::WrongData const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., a0=..., a1=...)
    at /usr/include/boost/function/function_template.hpp:138
#53 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#54 0x00007f0bb44a3953 in boost::python::detail::translate_exception<Tango::NonDbDevice, void (*)(Tango::NonDbDevice const&)>::operator()
    (this=<optimized out>, translate=0x7f0bb449ce50 <translate_non_db_device(Tango::NonDbDevice const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
#55 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NonDbDevice const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::NonDbDevice, void (*)(Tango::NonDbDevice const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#56 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::NonDbDevice, void (*)(Tango::NonDbDevice const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NonDbDevice const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>) at /usr/include/boost/bind/bind.hpp:1318
#57 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::NonDbDevice, void (*)(Tango::NonDbDevice const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::NonDbDevice const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., a0=..., 
    a1=...) at /usr/include/boost/function/function_template.hpp:138
#58 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#59 0x00007f0bb44a3903 in boost::python::detail::translate_exception<Tango::WrongNameSyntax, void (*)(Tango::WrongNameSyntax const&)>::operator() (this=<optimized out>, translate=0x7f0bb449cdd0 <translate_wrong_name_syntax(Tango::WrongNameSyntax const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
#60 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::WrongNameSyntax const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::WrongNameSyntax, void (*)(Tango::WrongNameSyntax const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#61 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::WrongNameSyntax, void (*)(Tango::WrongNameSyntax const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::WrongNameSyntax const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#62 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::WrongNameSyntax, void (*)(Tango::WrongNameSyntax const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::WrongNameSyntax const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., 
    a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#63 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#64 0x00007f0bb44a38b3 in boost::python::detail::translate_exception<Tango::CommunicationFailed, void (*)(Tango::CommunicationFailed const&)>::operator() (this=<optimized out>, translate=0x7f0bb449cd50 <translate_communication_failed(Tango::CommunicationFailed const&)>, 
    f=..., handler=...) at /usr/include/boost/python/detail/translate_exception.hpp:48
#65 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::CommunicationFailed const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::CommunicationFailed, void (*)(Tango::CommunicationFailed const&)>, boost::_bi::rrlist2<boost:---Type <return> to continue, or q <return> to quit---
:python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#66 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::CommunicationFailed, void (*)(Tango::CommunicationFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::CommunicationFailed const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#67 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::CommunicationFailed, void (*)(Tango::CommunicationFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::CommunicationFailed const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (
    function_obj_ptr=..., a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#68 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#69 0x00007f0bb44a3863 in boost::python::detail::translate_exception<Tango::ConnectionFailed, void (*)(Tango::ConnectionFailed const&)>::operator() (this=<optimized out>, translate=0x7f0bb449ccd0 <translate_connection_failed(Tango::ConnectionFailed const&)>, f=..., 
    handler=...) at /usr/include/boost/python/detail/translate_exception.hpp:48
#70 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::ConnectionFailed const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::ConnectionFailed, void (*)(Tango::ConnectionFailed const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#71 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::ConnectionFailed, void (*)(Tango::ConnectionFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::ConnectionFailed const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:1318
#72 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::ConnectionFailed, void (*)(Tango::ConnectionFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::ConnectionFailed const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (
    function_obj_ptr=..., a0=..., a1=...) at /usr/include/boost/function/function_template.hpp:138
#73 0x00007f0bb347030a in boost::python::detail::exception_handler::operator()(boost::function0<void> const&) const ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#74 0x00007f0bb44a3813 in boost::python::detail::translate_exception<Tango::DevFailed, void (*)(Tango::DevFailed const&)>::operator() (
    this=<optimized out>, translate=0x7f0bb449cc50 <translate_dev_failed(Tango::DevFailed const&)>, f=..., handler=...)
    at /usr/include/boost/python/detail/translate_exception.hpp:48
#75 boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::DevFailed const&)> >::operator()<bool, boost::python::detail::translate_exception<Tango::DevFailed, void (*)(Tango::DevFailed const&)>, boost::_bi::rrlist2<boost::python::detail::exception_handler const&, boost::function0<void> const&> > (f=..., a=<synthetic pointer>..., this=<optimized out>)
    at /usr/include/boost/bind/bind.hpp:388
#76 boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::DevFailed, void (*)(Tango::DevFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::DevFailed const&)> > >::operator()<boost::python::detail::exception_handler const&, boost::function0<void> const&> (a2=..., a1=..., this=<optimized out>) at /usr/include/boost/bind/bind.hpp:1318
#77 boost::detail::function::function_obj_invoker2<boost::_bi::bind_t<bool, boost::python::detail::translate_exception<Tango::DevFailed, void (*)(Tango::DevFailed const&)>, boost::_bi::list3<boost::arg<1>, boost::arg<2>, boost::_bi::value<void (*)(Tango::DevFailed const&)> > >, bool, boost::python::detail::exception_handler const&, boost::function0<void> const&>::invoke (function_obj_ptr=..., a0=..., a1=...)
    at /usr/include/boost/function/function_template.hpp:138
#78 0x00007f0bb34700df in boost::python::handle_exception_impl(boost::function0<void>) ()
   from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#79 0x00007f0bb3466959 in ?? () from /usr/lib/x86_64-linux-gnu/libboost_python-py35.so.1.62.0
#80 0x0000556036c4ae17 in PyObject_Call ()
#81 0x0000556036bca4a6 in PyEval_EvalFrameEx ()
#82 0x0000556036bb716f in PyEval_EvalCodeEx ()
#83 0x0000556036c011ef in ?? ()
---Type <return> to continue, or q <return> to quit---
#84 0x0000556036c4ae17 in PyObject_Call ()
#85 0x0000556036bc784e in PyEval_EvalFrameEx ()
#86 0x0000556036bb716f in PyEval_EvalCodeEx ()
#87 0x0000556036c011ef in ?? ()
#88 0x0000556036c4ae17 in PyObject_Call ()
#89 0x0000556036bc784e in PyEval_EvalFrameEx ()
#90 0x0000556036bceb06 in ?? ()
#91 0x0000556036bca639 in PyEval_EvalFrameEx ()
#92 0x0000556036bcf02f in ?? ()
#93 0x0000556036bca9e6 in PyEval_EvalFrameEx ()
#94 0x0000556036bb716f in PyEval_EvalCodeEx ()
#95 0x0000556036c011ef in ?? ()
#96 0x0000556036c4ae17 in PyObject_Call ()
#97 0x0000556036bc784e in PyEval_EvalFrameEx ()
#98 0x0000556036bceb06 in ?? ()
#99 0x0000556036bca9e6 in PyEval_EvalFrameEx ()
#100 0x0000556036bb78a7 in PyEval_EvalCodeEx ()
#101 0x0000556036c010e3 in ?? ()
#102 0x0000556036c4ae17 in PyObject_Call ()
#103 0x0000556036b6834e in ?? ()
#104 0x0000556036c4ae17 in PyObject_Call ()
#105 0x0000556036bb5f30 in PyEval_CallObjectWithKeywords ()
#106 0x0000556036c96ab1 in PyEval_CallMethod ()
#107 0x00007f0bb44eb15b in boost::python::call_method<void, boost::reference_wrapper<Tango::Attribute> > (self=0x7f0b9a168340, 
    name=0x556037d6ec90 "__read_attr1_wrapper__", a0=...) at /usr/include/boost/python/call_method.hpp:66
#108 0x00007f0bb44e2a78 in PyAttr::read (this=0x556037d71b68, dev=<optimized out>, att=...)
    at /home/zreszela/workspace/pytango/ext/server/attr.cpp:76
#109 0x00007f0bb39fb910 in Tango::Device_3Impl::read_attributes_no_except (this=this@entry=0x556037d71f40, names=..., aid=..., 
    second_try=second_try@entry=true, idx=std::vector of length 1, capacity 1 = {...}) at device_3.cpp:541
#110 0x00007f0bb3a17df7 in Tango::Device_5Impl::read_attributes_5 (this=0x556037d71f40, names=..., source=<optimized out>, cl_id=...)
    at device_5.cpp:340
#111 0x00007f0bb3bc805b in _0RL_lcfn_6fe2f94a21a10053_84000000 (cd=0x7f0b93ffeaa0, svnt=<optimized out>) at tangoSK.cpp:6272
#112 0x00007f0bb2278b69 in omniCallHandle::upcall(omniServant*, omniCallDescriptor&) () from /usr/lib/libomniORB4.so.1
#113 0x00007f0bb3bca1b0 in Tango::_impl_Device_5::_dispatch (this=<optimized out>, _handle=...) at tangoSK.cpp:7158
#114 0x00007f0bb2271f6d in omni::omniOrbPOA::dispatch(omniCallHandle&, omniLocalIdentity*) () from /usr/lib/libomniORB4.so.1
#115 0x00007f0bb2251116 in omniLocalIdentity::dispatch(omniCallHandle&) () from /usr/lib/libomniORB4.so.1
#116 0x00007f0bb2291d20 in omni::GIOP_S::handleRequest() () from /usr/lib/libomniORB4.so.1
#117 0x00007f0bb2292a98 in omni::GIOP_S::dispatcher() () from /usr/lib/libomniORB4.so.1
#118 0x00007f0bb228f6c5 in omni::giopWorker::real_execute() () from /usr/lib/libomniORB4.so.1
#119 0x00007f0bb228fd4f in omni::giopWorker::execute() () from /usr/lib/libomniORB4.so.1
#120 0x00007f0bb224602d in omniAsyncWorkerInfo::run() () from /usr/lib/libomniORB4.so.1
#121 0x00007f0bb3b84c4c in Tango::create_PyPerThData (info=...) at utils.cpp:3234
#122 0x00007f0bb2245f24 in omniAsyncWorkerInfo::run() () from /usr/lib/libomniORB4.so.1
#123 0x00007f0bb22465ff in omniAsyncWorker::run(void*) () from /usr/lib/libomniORB4.so.1
#124 0x00007f0bb1f876d9 in omni_thread_wrapper () from /usr/lib/libomnithread.so.3
#125 0x00007f0bb5a5e494 in start_thread (arg=0x7f0b93fff700) at pthread_create.c:333
#126 0x00007f0bb4c51acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:97
```

You can find backtrace of all threads in [`threads_backtrace_attr.txt`](https://github.com/reszelaz/test-tango/blob/master/threads_backtrace_attr.txt) file.

## Demo with commands and background jobs execution

When `cmd1` is executed it starts a thread in order to execute a background job.
This job creates a disposable `DeviceProxy` to `sys/tg_test/1` and
subscribes to `ATTR_CONF_EVENT` of `double_scalar` exactly in the same way as `attr1` readout.

When `cmd2` is executed it starts a thread in order to execute a background job.
This job creates a disposable `DeviceProxy` to `sys/tg_test/1` exactly the same way as `attr2` readout.

There are also two client scripts `client3.py` and `client4.py` which execute in a loop
`cmd1` and `cmd2` respectively.

When running in parallel the `DeviceServer`, `client3.py` and `client4.py`,
after just few seconds both clients stop with: `TRANSIENT CORBA system exception: TRANSIENT_CallTimedout`
and the device server gets hung. Any Tango request to it also gives:
`TRANSIENT CORBA system exception: TRANSIENT_CallTimedout`

You can find backtrace of all threads in [`threads_backtrace_cmd.txt`](https://github.com/reszelaz/test-tango/blob/master/threads_backtrace_attr.txt) file.
They point to the same problem as in case of reading attributes.

## Steps to reproduce the problem
1. Register in Tango Database one DeviceServer DS with instance name `test`
   with 2 devices of `Device1` and `Device2` classes, with the following names: `test/device1/1`, `test/device2/1` respectively.
    ```console
    tango_admin --add-server DeviceServer/test Device1 test/device1/1    
    tango_admin --add-server DeviceServer/test Device2 test/device2/1
    ```
2. Start DeviceServer: `python3 DeviceServer.py test`
3. Start clients:
  * Using attributes readout:
    1. Start client1: `python3 client1.py`
    2. Start client2: `python3 client2.py`
  * Using commands and background jobs execution:
    1. Start client3: `python3 client3.py`
    2. Start client4: `python3 client4.py`
4. Wait no more than 10 s...

I was not able to reproduce the problem with C++ (I have not tried using commands). See the files in [`cpp`](https://github.com/reszelaz/test-tango/tree/master/cpp)
subdirectory of this project, you can compile it with `make -f Makefile.multi`.
But I'm not at all used to program in C++ so it may be not exactly equivalent.

## Demo with continuous listening to events and parallel destructing DeviceProxy

When `Start` command of `DeviceEventGenerator` is executed it starts a thread
which will execute a background job. This job continuously pushes change events on
attribute `attr_with_events`.

When `Start` command of `Device3` is executed it starts a thread which will
execute a background job. This job will once subscribe the `attr_with_events`
event and will continue listening to the events.

`client5.py` after executing the above commands will continuously read `attr2`
of `Device2`. This attribute readout just creates a disposable `DeviceProxy`
to `sys/tg_test/1`.

When running in parallel the `DeviceServerEventGenerator`, `DeviceServer2` and
`client5.py` it seems to not hang in contrary to the other two demos.

## Steps to reproduce the problem
1. Register in Tango Database one `DeviceServerEventGenerator` DS with instance name `test`
   with 1 device of `DeviceEventGenerator` class, with the following name: `test/deviceeventgenerator/1`.
    ```console
    tango_admin --add-server DeviceServerEventGenerator/test DeviceEventGenerator test/deviceeventgenerator/1
    ```
2. Register in Tango Database one DeviceServer2 DS with instance name `test`
   with 2 devices of `Device2` and `Device3` classes, with the following names: `test/device2/2`, `test/device3/1` respectively.
    ```console
    tango_admin --add-server DeviceServer2/test Device2 test/device2/2    
    tango_admin --add-server DeviceServer2/test Device3 test/device3/1
    ```
3. Start DeviceServerEventGenerator: `python3 DeviceServerEventGenerator.py test`
4. Start DeviceServer2: `python3 DeviceServer2.py test`
3. Start client5: `python3 client5.py`
4. Wait...

I was not able to reproduce the problem with C++ (I have not tried using commands). See the files in [`cpp`](https://github.com/reszelaz/test-tango/tree/master/cpp)
subdirectory of this project, you can compile it with `make -f Makefile.multi`.
But I'm not at all used to program in C++ so it may be not exactly equivalent.