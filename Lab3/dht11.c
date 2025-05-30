#include "dht11.h"


void startSignal(){
	//setting the mode of the pin that corresponds to the DHT11 pin2 to output
	gpio_set_mode(PC_1, Output);
	gpio_set(PC_1, 0);
	delay_ms(18);
	gpio_set(PC_1, 1);
	delay_us(40);	
	
	//setting the pin as input to get the response from the sensor 
	gpio_set_mode(PC_1, Input);
	delay_us(160);
	//waiting the pin to get low to pass to the next procedure 
	while(gpio_get(PC_1));
}

// function called for the reception of each bit 
uint8_t dataTransmission(){
	delay_us(20); // start to transmit 1 bit data 
	while(!gpio_get(PC_1));//waiting the pin to get high to pass to the next procedure 
	
	delay_us(29); // in order to check the 28 us threshold of the "0" bit 
	if(!gpio_get(PC_1)) return 0;
	else { // else we set the output to "1" 
		delay_us(41); // also add a delay to reach the 70us duration of the "1" bit to keep the balance 
		return 1;
	}
}

float* getData(){
	 // initialization of the communication with the sensor only once for every measurement 
	startSignal();
	int i,j; 
	uint8_t data[5] = {0,0,0,0,0}; // array to keep the 5 bytes received from the communication with the sensor 
	static float measurments[2] = {0,0}; // array to keep the complete values of the humidity and temperature. It is static so we can return it
	// loop to receive all 40 bits 
	for(i=0,j=0; i < 40; i++){
		data[j] = (uint8_t)(data[j]<<1) | dataTransmission(); // shift the data[j] by 1 bit and pass the values returned from dataTransmission()
		if((i+1)%8==0){j++;} // once a byte is complete we pass to the next one 
	}	
	// checking the checksum and returning the measurements 
	if(checksum(data)){ 
		measurments[0] = data[0] + data[1]/(float)10;// Humudity
		measurments[1] = data[2] + data[3]/(float)10;//Temperature
		return measurments;
	}
	else{
		return NULL;
	}
}

bool checksum(uint8_t* data){
	int i;
	uint8_t checksum = 0;
	// calculating the ckecksum by adding the first 4 bytes 
	for(i=0; i<4;i++){
		checksum += data[i];
	}
	// chceking the calculated checksum with the one provided by the sensor
	if(checksum == data[4]){
		return true;
	}
	else{
		return false;
	}
}

