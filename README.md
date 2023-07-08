# UAV Simulation engine
This is a physics engine designed to test UAV control algorithms. It is made in a way so the basic physics are implemented and is open for expansion for the physics that are specific to the type of UAV.

## Configuration
The basic configuration file looks something like this.

```
udp_ip_address: 127.0.0.1
udp_port: 5050
out_folder: ./logs
data_link_port: 9000
uav_initial_position: 46.404582 15.634940 270.0
```

|  Parameter            | Description                                                                       |
| -----------           | ------------                                                                      | 
|  udp_ip_address       | UDP stream server ip address                                                      |
|  udp_port             | UDP stream server port                                                            |
|  out_folder           | File stream folder                                                                |
|  data_link_port       | The data link TCP/IP port                                                         |
|  uav_initial_position | The initial geodetic position of the UAV and the ENU coordinate system center     |


## Data out
There are data streams that are implemented so that the engine streams out the state of the UAV (position, orientation...). The data streaming is also implemented in such a way that is open to add additional ways of streaming the data. In the `src/streamer/streams` there are some 2 examples on how to implement an additional data stream.

### UDP stream
This stream outputs the data with the UDP protocol. To receive this data you only have to implement a UDP server and receive the data. The data is in the following format.

|  Data                     | Data type    | Num of bytes  | Description                | 
| -----------               | ------------ | ------------- | -----------                |
| Position ENU x                | float        | 4             | The x axis of the position from the initial position |
| Position ENU y                | float        | 4             | The y axis of the position from the initial position |
| Position ENU z                | float        | 4             | The z axis of the position from the initial position |
| Position lat              | float        | 4             | Latitude of the UAV  |
| Position lon              | float        | 4             | Longitude of the UAV |
| Position alt              | float        | 4             | Altitude of the UAV |
| Orientation quaternion w  | float        | 4             | The w axis of the orienation quaternion |
| Orientation quaternion x  | float        | 4             | The x axis of the orienation quaternion |
| Orientation quaternion y  | float        | 4             | The y axis of the orienation quaternion |
| Orientation quaternion z  | float        | 4             | The z axis of the orienation quaternion |

### File stream
The file stream writes the data into a file and is in the same format as the UDP stream. The only difference is that the file is written in ASCII and not in binary. It also includes the timestamp in microseconds in the first line. Samples are seperated with a line of dashes.

