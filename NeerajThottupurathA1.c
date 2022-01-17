#include <stdio.h>
#include <string.h>

#define MAX_BUF  256
#define BYTE_CONSTANT 8
#define IV 0b11001011

void encode(unsigned char*, unsigned char*, unsigned char);
void decode(unsigned char*, unsigned char*, unsigned char, int);

unsigned char computeKey(unsigned char);
unsigned char encryptByte(unsigned char, unsigned char);
unsigned char decryptByte(unsigned char, unsigned char);
unsigned char XOR(unsigned char, unsigned char);

unsigned char getBit(unsigned char, int);
unsigned char setBit(unsigned char, int);
unsigned char clearBit(unsigned char, int);


int main()
{
	char str[8];
	int  choice;
	unsigned char partialKeyInput;
	unsigned char key;
	unsigned char pt[MAX_BUF];
	unsigned char ct[MAX_BUF];

	printf("\nYou may:\n");
	printf("  (1) Encrypt a message \n");
	printf("  (2) Decrypt a message \n");
	printf("\nEnter your selection: ");
	fgets(str, sizeof(str), stdin);
	sscanf(str, "%d", &choice);
		

	switch (choice) {

		case 1:
			printf("\nEnter a partial key value between 1 and 15: ");
			scanf("%hhu", &partialKeyInput);
			// clear buffer
			fgetc(stdin);
			key = computeKey(partialKeyInput);
			
			printf("\nEnter plaintext: ");
			fgets((char*)pt, 256, stdin);
			encode(pt, ct, key);
			break;

		case 2:
			printf("\nEnter a partial key value between 1 and 15: ");
			scanf("%hhu", &partialKeyInput);
			// clear buffer
			fgetc(stdin);
			key = computeKey(partialKeyInput);

			printf("\nEnter ciphertext (type -1 when finished): ");
			unsigned char input;
			int numBytes = 0;
			// loop until -1 is entered
			while (input != 255)
			{
				scanf("%hhu", &input);
				getchar();
				ct[numBytes] = input;
				numBytes++;
			}			
			decode(ct, pt, key, numBytes);					
			break;			

		default:
			printf("\nYou didn't enter a valid choice.");
			break;
	}

	return 0;
}


/*
	Function:   encode
	 Purpose:   encrypts plaintext string using the key and stores in an array
		  in:   plaintext array
		  in:   ciphertext array
		  in:   key
*/
void encode(unsigned char *pt, unsigned char *ct, unsigned char k)
{
	int i = 0;
	unsigned char source;
	unsigned char coded;
	while (pt[i] != 10)
	{
		// if first plaintext byte, XOR with IV; else XOR with previous ciphertext byte
		if (i == 0)
		{
			source = XOR(pt[i], IV);
			coded = encryptByte(source, k);
			ct[i] = coded;
			i++;
		}
		else
		{	
			source = XOR(pt[i], ct[i-1]);
			coded = encryptByte(source, k);
			ct[i] = coded;
			i++;
		}
	}
	// printing ciphertext
	printf("\n");
	for(int x = 0; x < i; x++)
	{
		printf("%u ", ct[x]);
	}
	printf("\n\n");	
}


/*
	Function:   decode
	 Purpose:   decrypts ciphertext using the key and stores in an array
		  in:   ciphertext array
		  in:   plaintext array
		  in:   key
		  in:   number of bytes of ciphertext
*/
void decode(unsigned char *ct, unsigned char *pt, unsigned char k, int numBytes)
{
	unsigned char source;
	unsigned char decoded;
	for (int i = 0; i < numBytes-1; i++)
	{
		// if first plaintext byte, XOR with IV; else XOR with previous ciphertext byte
		if (i == 0)
		{
			source = decryptByte(ct[i], k);
			decoded = XOR(source, IV);
			pt[i] = decoded;
		}
		else
		{
			source = decryptByte(ct[i], k);
			decoded = XOR(source, ct[i-1]);
			pt[i] = decoded;
		}
		// printing plaintext
		printf("%c", pt[i]);
	}
	printf("\n");
}

/*
	Function:   computeKey
	 Purpose:   computes key given the partial key
		  in:   partial key
	  return:   computed key
*/
unsigned char computeKey(unsigned char partial)
{
	for (int i = 3; i >= 0; i--)
	{
		if (getBit(partial, i) == 1)
		{			
			partial = setBit(partial, 7-i);
		}
	}
	return partial;	
}

/*
	Function:   XOR
	 Purpose:   helper function to find source byte
		  in:   plaintext byte
		  in:   previous ciphertext byte
	  return:   result of XOR between plaintext and ciphertext byte
*/
unsigned char XOR(unsigned char pt, unsigned char previous)
{
	return pt ^ previous;
}

/*
	Function:   encryptByte
	 Purpose:   encrypts given byte
		  in:   source byte
		  in:   key used to encrypt source byte
	  return:   ciphertext byte
*/
unsigned char encryptByte(unsigned char src, unsigned char k)
{

	unsigned char ct;
	// sets ciphertext byte to 0
	for (int i = 7; i >+ 0; i--)
	{
		ct = clearBit(ct, i);
	}
	
	// XOR between the two shifts to achieve circular shift
	unsigned char left;
	unsigned char right;
	unsigned char temp;
	left = src << 2;
	right = src >> (BYTE_CONSTANT - 2);
	temp = left | right;
	
	unsigned char newVal = -1;
	for (int i = 7; i >= 0; i--)
	{
		newVal = (getBit(temp, i)) ^ (getBit(k, 7-i));
		if (newVal == 1)
		{
			ct = setBit(ct, i);
		}
		else
		{
			ct = clearBit(ct , i);
		}
	}
	return ct;		
}

/*
	Function:   decryptByte
	 Purpose:   decrypts given byte
		  in:   ciphertext byte
		  in:   key used to decrypt ciphertext
	  return:   source byte to be input into decode() function
*/
unsigned char decryptByte(unsigned char ct, unsigned char k)
{
	unsigned char pt;
	// sets plaintext byte to 0
	for (int i = 7; i >+ 0; i--)
	{
		pt = clearBit(pt, i);
	}
	
	unsigned char temp = XOR(ct, k);
	
	// XOR between the two shifts to achieve circular shift
	unsigned char right;
	unsigned char left;
	unsigned char source;
	right = temp >> 2;
	left = temp << (BYTE_CONSTANT - 2);
	source = left | right;
	
	return source;		
}

/*
	Function:   getBit
	 Purpose:   retrieve value of bit at specified position
		  in:   character from which a bit will be returned
		  in:   position of bit to be returned
	  return:   value of bit n in character c (0 or 1)
*/
unsigned char getBit(unsigned char c, int n)   
{ 
	return ((c & (1 << n)) >> n);
}

/*
	Function:   setBit
	 Purpose:   set specified bit to 1
		  in:   character in which a bit will be set to 1
		  in:   position of bit to be set to 1
	  return:   new value of character c with bit n set to 1
*/
unsigned char setBit(unsigned char c, int n)   
{ 
	c = c | (1 << n);
	return c;
}

/*
	Function:   clearBit
	 Purpose:   set specified bit to 0
		  in:   character in which a bit will be set to 0
		  in:   position of bit to be set to 0
	  return:   new value of character c with bit n set to 0
*/
unsigned char clearBit(unsigned char c, int n) 
{ 
	c = c & (~(1 << n));
	return c;
}

