# biogas_pd_lakah

## Installation

1.  Install latest LTS of Node.js
    https://nodejs.org/en/
    
2.  Install Node-RED :
    a. Open "command prompt" (cmd)
    
    b. Type below command and press Enter:
       > npm install -g node-red
    
    c. Install node-red's dashboard, Type below command and press Enter:     
       > npm install node-red-dashboard
        
3.  Install Mosquitto Broker/Server
    a. Download, choose the suitable type (x32 or x64):
       https://mosquitto.org/download/
       
    b. Run the downloaded installer (.exe)
    
    c. Install and follow instructions. You can also follow the instructions written in this website below.
       http://bytesofgigabytes.com/mqtt/installing-mqtt-broker-on-windows/
    
    d. Open command prompt as administrator (Right-click -> Run as Adminsitrator)
    
    e. Go to mosquitto's directory, type the command below and press Enter:
       > cd C:\Program File\mosquitto
    
    f. Type commands below to start mosquitto server and press Enter
       > mosquitto install
       > net start mosquitto
       
    g. Mosquitto service is installed and ready to use. If any of you encounter any problems, refer to the website written in the point c.
    
## Run
### Run Node-red
1.  Type below command in new command prompt
    > node-red
    
2.  Open browser and type:
    127.0.0.1:1880

3.  Import file named "Flow"

4.  The Nodes are displayed.

### Run Node-red Dashboard
1.  Follow the above run Node-red instructions

2.  Create a new browser tab, type "127.0.0.1:1880/ui" and press Enter

3.  The dasboard is displayed
