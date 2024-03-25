// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef SILABS_EMBER_AF_COMMAND_PARSE_HEADER
#define SILABS_EMBER_AF_COMMAND_PARSE_HEADER


// This is a set of generated prototype for functions that parse the
// the incomming message, and call appropriate command handler.

// Cluster: Basic, server
EmberAfStatus emberAfBasicClusterServerCommandParse(EmberAfClusterCommand *cmd);

// Cluster: Identify, client
EmberAfStatus emberAfIdentifyClusterClientCommandParse(EmberAfClusterCommand *cmd);

// Cluster: Identify, server
EmberAfStatus emberAfIdentifyClusterServerCommandParse(EmberAfClusterCommand *cmd);

// Cluster: Groups, client
EmberAfStatus emberAfGroupsClusterClientCommandParse(EmberAfClusterCommand *cmd);

// Cluster: Groups, server
EmberAfStatus emberAfGroupsClusterServerCommandParse(EmberAfClusterCommand *cmd);

// Cluster: Scenes, client
EmberAfStatus emberAfScenesClusterClientCommandParse(EmberAfClusterCommand *cmd);

// Cluster: Scenes, server
EmberAfStatus emberAfScenesClusterServerCommandParse(EmberAfClusterCommand *cmd);

// Cluster: On/off, server
EmberAfStatus emberAfOnOffClusterServerCommandParse(EmberAfClusterCommand *cmd);

// Cluster: IAS Zone, client
EmberAfStatus emberAfIasZoneClusterClientCommandParse(EmberAfClusterCommand *cmd);

// Cluster: IAS Zone, server
EmberAfStatus emberAfIasZoneClusterServerCommandParse(EmberAfClusterCommand *cmd);

#endif // SILABS_EMBER_AF_COMMAND_PARSE_HEADER
