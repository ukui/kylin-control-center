#include "networkaccount.h"

networkaccount::networkaccount()
{
    pluginWidget = new config_list_widget();
    pluginName = tr("NetworkAccount");
    pluginType = ACCOUNT;
}


QString networkaccount::get_plugin_name(){
    return pluginName;
}

int networkaccount::get_plugin_type(){
    return pluginType;
}

QWidget * networkaccount::get_plugin_ui(){
    return pluginWidget;
}

void networkaccount::plugin_delay_control(){

}


