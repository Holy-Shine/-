/*
example:
	input  5: 101
	output 2: 010
*/

int findComplete(int num){
	int mask =  ~0;   			// mask=11111111
	while(num & mask) mask<<=1; // mask=11111000
	return ~num & ~ mask;
}