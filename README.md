source


To send content trough UDP:
	1. Import python file or copy paste whole file in python terminal
	2. Call startTransmit() with the corresponding parameters: Packetsize, Transmit period, file Divider (used to divide audio files to match samplerate)

	Best known method is to call as follows --> startTransmit(250, 0.010, 2)   for a 22.222Hz sample rate (configured in the PIT_Handler)