# GL-A-uart
GL Automotive Basecamp project (Dmytro Hnatiuk squad).
 
### Create an application with 3 FreeRTOS tasks on the ESP32 board.

### Task 1: Transmit through UART the structure:  

```
typedef struct{
  uint16_t  start;
  int16_t   cmd1;
  int16_t   cmd2;
  uint16_t  checksum;  
} SerialData 
```

where
`checksum` = (`start^cmd1^cmd2`) and
`start` =  `0xABCD` is start data marker. 

### Task 2: Receive data from UART without polling.
This task validates received data and sends parsed commands to Task 3.

### Task 3: Display information. 
This task is running always. It shows on TFT display received valid data or errors.


# Step by step task solution.

- [x] Use UART Asynchronous Example with Separate Receive and Transfer Tasks.
- [x] Pin tasks to different cores.
- [x] Add TFT simple test output.
- [x] Add TFT text output.
- [ ] Implement 'struct' of data.
- [ ] Send 'struct' of data.
- [ ] Receive 'struct' of data.
- [ ] Parse input data.
- [ ] Validate input data.
- [ ] Display on TFT valid received data 'struct'.
- [ ] Test stability of HW and SW.
