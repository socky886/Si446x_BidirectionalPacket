add length to rx fifo, the print information is below:
the payload length is 8
0x07, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
receive packet successfully
start rx--------------------


do not add length to rx fifo, the print information is below:
the payload length is 7
0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
receive packet successfully
start rx--------------------

you can get the rx fifo legnth by the way below:
si446x_fifo_info(0x00);
si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &customRadioPacket[0]);
len =Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;

for tx,you should set the length of the field2
si446x_set_property(0x12,0x02,0x11,0x00,length);

for rx,you should set the max length of the feild2
si446x_set_property(0x12,0x02,0x11,0x00,63);

you shoud redefination the function of si446x_set_property
void si446x_set_property( U8 GROUP, U8 NUM_PROPS, U8 START_PROP, U8 data1,U8 data2 )
{
    // fill data
    Pro2Cmd[0] = SI446X_CMD_ID_SET_PROPERTY;
    Pro2Cmd[1] = GROUP;
    Pro2Cmd[2] = NUM_PROPS;
    Pro2Cmd[3] = START_PROP;
    Pro2Cmd[4] = data1;
    Pro2Cmd[5] = data2;
    
    // send command
    radio_comm_SendCmd( 6, Pro2Cmd );
}
