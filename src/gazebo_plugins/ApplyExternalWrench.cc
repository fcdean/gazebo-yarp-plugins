#include "gazebo_yarp_plugins/ApplyExternalWrench.hh"

namespace gazebo
{

GZ_REGISTER_MODEL_PLUGIN ( ApplyExternalWrench )

ApplyExternalWrench::ApplyExternalWrench()
{
    this->_wrench_to_apply.force.resize ( 3,0 );
    this->_wrench_to_apply.torque.resize ( 3,0 );
    _link_name="neck_1";
}
ApplyExternalWrench::~ApplyExternalWrench()
{
    _rpcThread.stop();
}

void ApplyExternalWrench::UpdateChild()
{
    // Reading apply wrench command
    yarp::os::Bottle tmpBottle;

    // Copying command
    this->_lock.lock();
    tmpBottle = this->_rpcThread.get_cmd();
    this->_lock.unlock();

    // Parsing command
    this->_link_name = tmpBottle.get ( 0 ).asString();
    for ( int i=1; i<4; i++ ) {
        _wrench_to_apply.force[i-1] = tmpBottle.get ( i ).asDouble();
    }
    for ( int i=4; i<7; i++ ) {
        _wrench_to_apply.torque[i-4] = tmpBottle.get ( i ).asDouble();
    }

    this->_onLink  = _myModel->GetLink ( this->_link_name );
    if ( !this->_onLink ) {
        std::cout<<"ERROR ApplyWrench plugin: link named "<< this->_link_name<< "not found"<<std::endl;
        return;
    }

    // Copying command to force and torque Vector3 variables
    math::Vector3 force ( this->_wrench_to_apply.force[0], this->_wrench_to_apply.force[1], this->_wrench_to_apply.force[2] );
    math::Vector3 torque ( this->_wrench_to_apply.torque[0], this->_wrench_to_apply.torque[1], this->_wrench_to_apply.torque[2] );
//     printf ( "Applying wrench:( Force: %s  Torque: %s ) on link %s",_wrench_to_apply.force.toString().c_str(), _wrench_to_apply.torque.toString().c_str(), _link_name.c_str() );
    // Applying wrench to the specified link
    this->_onLink->AddForce ( force );
    this->_onLink->AddTorque ( torque );
}

void ApplyExternalWrench::Load ( physics::ModelPtr _model, sdf::ElementPtr _sdf )
{
    // Check if yarp network is active;
    if ( !this->_yarpNet.checkNetwork() ) {
        printf ( "ERROR Yarp Network was not found active in ApplyExternalWrench plugin" );
        return;
    }

    // Copy the pointer to the model
    this->_myModel = _model;

    bool configuration_loaded = false;

    // Read robot name
    if ( _sdf->HasElement ( "robotNamefromConfigFile" ) ) {
        std::string ini_robot_name      = _sdf->Get<std::string> ( "robotNamefromConfigFile" );
        std::string ini_robot_name_path =  gazebo::common::SystemPaths::Instance()->FindFileURI ( ini_robot_name );

        if ( ini_robot_name_path != "" && this->_iniParams.fromConfigFile ( ini_robot_name_path.c_str() ) ) {
            std::cout << "ApplyExternalWrench: Found robotNamefromConfigFile in "<< ini_robot_name_path << std::endl;
            yarp::os::Value robotNameParam = _iniParams.find ( "gazeboYarpPluginsRobotName" );
            this->robot_name = robotNameParam.asString();
            printf("ApplyExternalWrench: robotName is %s \n",robot_name.c_str());
            _rpcThread.setRobotName(robot_name);
        }
    }

    // Starting RPC thread to read desired wrench to be applied
    if ( !_rpcThread.start() ) {
        printf ( "ERROR: rpcThread did not start correctly\n" );
    }

    // Listen to the update event. This event is broadcast every
    // simulation iteration.
    this->_update_connection = event::Events::ConnectWorldUpdateBegin ( boost::bind ( &ApplyExternalWrench::UpdateChild, this ) );
}

}


// ############ RPCServerThread class ###############

void RPCServerThread::setRobotName(std::string robotName)
{
    this->_robotName = robotName;
}

bool RPCServerThread::threadInit()
{
    
    printf ( "Starting RPCServerThread\n" );
    printf ( "Opening rpc port\n" );
    if ( !_rpcPort.open ( std::string("/"+_robotName + "/applyExternalWrench/rpc:i" ).c_str() )) {
        printf ( "ERROR opening RPC port /applyExternalWrench\n" );
        return false;
    }
    // Default link on wihch wrenches are applied
    _cmd.addString ( "neck_1" );
    _cmd.addDouble ( 0 );
    _cmd.addDouble ( 0 );
    _cmd.addDouble ( 0 );
    _cmd.addDouble ( 0 );
    _cmd.addDouble ( 0 );
    _cmd.addDouble ( 0 );

    return true;
}
void RPCServerThread::run()
{
    while ( !isStopping() ) {
        yarp::os::Bottle command;

        _rpcPort.read ( command,true );
        this->_reply.addString ( "[ACK]" );
        this->_rpcPort.reply ( this->_reply );
        _reply.clear();
        _lock.lock();
        _cmd = command;
        _lock.unlock();
    }
}
void RPCServerThread::threadRelease()
{
    yarp::os::Thread::threadRelease();
    printf ( "Goodbye from RPC thread\n" );
}
yarp::os::Bottle RPCServerThread::get_cmd()
{
    return _cmd;
}
