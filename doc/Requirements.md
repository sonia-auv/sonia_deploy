# Node monitoring

The **SystemMonitor** node is a monitoring tool used to observe, evaluate and showcase the operational status of the **ROS2** nodes. The monitored nodes periodically publish their current state and health quality. The tool provides a summarized system health status of all the nodes it monitors. 

## Requirements

### requirement 1
Titre : processNodeStatusCallback··
PreCondition : Node message from monitored node is received.··
Post condition : Message is registered in a map of nodes.··

### requirement 2
Titre : publishSystemStatus··
PreCondition : NA··
Post condition : publish summarized state of all mapped nodes defined in the config file.··

## Functional
### Exigence 1
Every 500ms, publish states all all system nodes.
