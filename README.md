# UAV Simulation engine
This is a physics engine designed to test UAV control algorithms. It is made in a way so the basic physics are implemented and is open for expansion for the physics that are specific to the type of UAV.

## Data out
There are data streams that are implemented so that the engine streams out the state of the UAV (position, orientation...). The data streaming is also implemented in such a way that is open to add additional ways of streaming the data. In the `src/streamer/streams` there are some 2 examples on how to implement an additional data stream.

### UDP stream
This stream outputs the data with the UDP protocol. To receive this data you only have to implement a UDP server and receive the data. The data is in the following format.

|  Data                     | Data type    | Num of bytes  | Description                | 
| -----------               | ------------ | ------------- | -----------                |
| Position x                | float        | 4             | The x axis of the position |
| Position y                | float        | 4             | The y axis of the position |
| Position z                | float        | 4             | The y axis of the position |
| Orientation quaternion w  | float        | 4             | The w axis of the orienation quaternion |
| Orientation quaternion x  | float        | 4             | The x axis of the orienation quaternion |
| Orientation quaternion y  | float        | 4             | The y axis of the orienation quaternion |
| Orientation quaternion z  | float        | 4             | The z axis of the orienation quaternion |

