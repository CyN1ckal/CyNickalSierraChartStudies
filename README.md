# CyNickal's Sierra Chart Studies
This file is to be used with [Sierra Chart](https://sierrachart.com/). It is my personal collection of studies. 

### Current Bar Range vs Average True Range (CBR / ATR)
This function is used to compare the range of the current bar to the average true range of bars from the same period. The length and moving average type of the ATR are customizable. 
![Example of the CBR / ATR Function](https://cdn.discordapp.com/attachments/1123793132480372736/1192304466347704370/image.png?ex=65a89710&is=65962210&hm=a50e030002fbc28792487e93a58b93e228083f00d7359bebf79366346dd8d446&)

### Random Entries
This function will randomly enter and exit trades with 1 contract/quantity. 
The user inputs a 1/x chance for entry and exit per second.

By default, it will enter a trade 1/15 seconds and exit a trade 1/60 seconds. 
![Example of Random Entries](https://cdn.discordapp.com/attachments/1123793132480372736/1192305868570632232/image.png?ex=65a8985f&is=6596235f&hm=bebca135814b9bd62fb24fad5c745503c3e300e171fdaf2499673b669afccf61&)

### CBR/ATR Bar Color
Based on the Current Bar Range vs Average True Range function. It will highlight the current bar if it exceeds a certain threshold provided. 
![Example of CBR/ATR Bar Color](https://cdn.discordapp.com/attachments/1123793132480372736/1193232504639995924/NQH24_FUT_CMEM_1_Min_1_2024-01-06_08_39_37.176.png?ex=65abf75e&is=6599825e&hm=3e414453cc6a9c3f1187fda67bdcc0e10e327ac114cdbc44e4394b77164eb8f7&)

### CBR/ATR Text Drawing
Same calculations and inputs as the original CBR/ATR, but displayed differently. The current bar's CBR/ATR is displayed in the top left of the main price region in text. 
![Example of CBR/ATR Text Drawing](https://cdn.discordapp.com/attachments/804797697851654164/1194132746294009881/image.png?ex=65af3dc8&is=659cc8c8&hm=87e2af8d02701f0ec9773abd168b0e17e995eda42885ff8e36e2803ab6307d3a&)
