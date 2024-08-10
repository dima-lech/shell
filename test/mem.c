#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>


enum
{
	OP_MEM_READ,
	OP_MEM_WRITE,

	OP_UNKNOWN
};




void memCommand(int argc, char * argv[]);
static void memRead(int64_t addressPrint, int64_t address,
					int64_t length, int64_t width);
static void memWrite(int64_t addressPrint, int64_t address,
					int64_t value, int64_t width, int interactiveFlag);
static void memCommandUsagePrint(char * command);
static int64_t memArgParse(char * arg);
static uint64_t memReadVal(uint64_t address, int64_t width);
static void memWriteVal(uint64_t address, int64_t width, uint64_t value);
static void memPrintVal(int64_t width, uint64_t value);




void memCommand(int argc, char * argv[])
{
	int c;
	int operation = OP_UNKNOWN;
	int64_t length = 4;
	static int64_t width = 4;
	int64_t widthInput;
	int64_t value = 0;
	int64_t address = 0;
	int interactiveFlag = 1;

	if (argc <= 1)
	{
		memCommandUsagePrint(argv[0]);
		return;
	}

	optind = 0;
	optarg = 0;
	widthInput = width;

	while ((c = getopt (argc, argv, "w:i")) != -1)
	{
		switch (c)
		{
			case 'w':
				if (optarg)
				{
					widthInput = memArgParse(optarg);
				}
				break;
			case 'i':
				interactiveFlag = 1;
				break;
			default:
				memCommandUsagePrint(argv[0]);
				return;
		}
	}

	if ((widthInput != 1) && (widthInput != 2) && (widthInput != 4))
	{
		printf("unsupported width!\n");
		return;
	}
	width = widthInput;


	if ((argc - optind) < 1)
	{
		memCommandUsagePrint(argv[0]);
		return;
	}


	/* Command argument */
	if (strcmp(argv[optind], "d") == 0)
	{
		operation = OP_MEM_READ;
	}
	else if (strcmp(argv[optind], "m") == 0)
	{
		operation = OP_MEM_WRITE;
	}
	else
	{
		memCommandUsagePrint(argv[0]);
		return;
	}
	optind++;


	/* Expecting at least 1 additional non-optional argument */
	if ((argc - optind) < 1)
	{
		memCommandUsagePrint(argv[0]);
		return;
	}

	/* Address argument */
	address = memArgParse(argv[optind++]);

	/* Length/value argument */
	if (OP_MEM_READ == operation)
	{
		if (optind < argc)
		{
			length = memArgParse(argv[optind]);
			if (0 == length)
			{
				length = 4;
			}
		}
	}
	else if (OP_MEM_WRITE == operation)
	{
		if (optind < argc)
		{
			value = memArgParse(argv[optind]);
			interactiveFlag = 0;
		}
	}
	else
	{
		memCommandUsagePrint(argv[0]);
		return;
	}


	switch (operation)
	{
		case OP_MEM_READ:
			memRead(address, address, length, width);
			break;
		case OP_MEM_WRITE:
			memWrite(address, address, value, width, interactiveFlag);
			break;
		default:
			break;
	}
}




static void memCommandUsagePrint(char * command)
{
	printf("Usage\n");
	printf("\t%-16s%s d <address> [<length>] [-w <width>]\n",
			"memory dump:", command);
	printf("\t%-16s%s m <address> [<value>] [-w <width>]\n",
			"memory modify:", command);
}


static int64_t memArgParse(char * arg)
{
	int base = 10;

	if (!arg)
	{
		return 0;
	}

	if (('0' == arg[0]) && (('x' == arg[1]) || ('X' == arg[1])))
	{
		/* HEX value */
		base = 16;
	}

	return (int64_t)strtoul(arg, NULL, base);
}




static void memRead(int64_t addressPrint, int64_t address,
					int64_t length, int64_t width)
{
	int32_t value = 0x0;
	int32_t i;
	int32_t j;
	int32_t k;
	char charBuffer[17];
	int32_t charBufferIndex = 0;
	int64_t widthMask;
	int64_t widthMaskPrint;

	widthMask = ~(width - 1);
	widthMaskPrint = widthMask << 2;

	address = address & widthMask;
	addressPrint = addressPrint & widthMask;

	/* Print memory dump */
	for (i = 0; i < length; i += width)
	{
		if ((0 == i) || ((addressPrint % (width * 4)) == 0))
		{
			if (i > 0)
			{
				charBuffer[charBufferIndex] = '\0';
				charBufferIndex = 0;
				printf("\t/%s/\n", charBuffer);
			}
			printf("%08lX :", addressPrint & widthMaskPrint);
		}

		/* Print first blanks */
		if (0 == i)
		{
			for (j = 0; j < ((addressPrint / width) % 4); j++)
			{
				for (k = 0; k < width; k++)
				{
					printf("  ");
					charBuffer[charBufferIndex++] = ' ';
				}
				printf(" ");
			}
		}

		value = memReadVal(address, width);
		printf(" ");
		memPrintVal(width, value);

		for (k = 0; k < width; k++)
		{
			j = value & 0xff;
			value >>= 8;

			if ((j >= 32) && (j <= 127))
			{
				charBuffer[charBufferIndex++] = (char)j;
			}
			else
			{
				charBuffer[charBufferIndex++] = '.';
			}
		}

		address += width;
		addressPrint += width;
	}

	/* Print last blanks */
	for (j = 0; j < (4 - ((addressPrint / width) % 4)) % 4; j++)
	{
		for (i = 0; i < width; i++)
		{
			printf("  ");
			charBuffer[charBufferIndex++] = ' ';
		}
		printf(" ");
	}

	charBuffer[charBufferIndex] = '\0';
	printf("\t/%s/\n", charBuffer);
}


static void memWrite(int64_t addressPrint, int64_t address,
					int64_t value, int64_t width, int interactiveFlag)
{
	int64_t widthMask;
	int exitFlag = 0;
	int skipFlag = 0;
	char inputStr1[20] = { '.' };
	char inputStr2[20] = { '\0' };

	widthMask = ~(width - 1);

	address = address & widthMask;
	addressPrint = addressPrint & widthMask;

	if (!interactiveFlag)
	{
		printf("%08lX <= ", addressPrint);

		memPrintVal(width, value);
		printf("\n");
		memWriteVal(address, width, value);

	}
	else
	{
		printf("(enter '.' or 'q' to exit)\n");

		while (!exitFlag)
		{
			skipFlag = 0;

			printf("%08lX : ", addressPrint);

			value = memReadVal(address, width);
			memPrintVal(width, value);
			printf(" > ");

			if (fgets(inputStr1, sizeof(inputStr1), stdin) == 0)
			{
				skipFlag = 1;
			}
			else
			{
				if (sscanf(inputStr1, "%s", inputStr2) <= 0)
				{
					skipFlag = 1;
				}
			}

			if (('.' == inputStr2[0]) || ('q' == inputStr2[0])
					|| ('Q' == inputStr2[0]))
			{
				exitFlag = 1;
			}
			else if (!skipFlag)
			{
				value = memArgParse(inputStr2);
				
				memWriteVal(address, width, value);
			}
			address += width;
			addressPrint += width;
		}
	}

}


static uint64_t memReadVal(uint64_t address, int64_t width)
{
	uint64_t value;

	switch (width)
	{
		case 1:
			value = *(volatile uint8_t*)address;
			break;
		case 2:
			value = *(volatile uint16_t*)address;
			break;
		case 4:
			value = *(volatile uint32_t*)address;
			break;
		default:
			return 0;
	}

	return value;
}


static void memWriteVal(uint64_t address, int64_t width, uint64_t value)
{
	switch (width)
	{
		case 1:
			*(volatile uint8_t*)address = (uint8_t)value;
			break;
		case 2:
			*(volatile uint16_t*)address = (uint16_t)value;
			break;
		case 4:
			*(volatile uint32_t*)address = (uint32_t)value;
			break;
		default:
			return;
	}
}


static void memPrintVal(int64_t width, uint64_t value)
{
	switch (width)
	{
		case 1:
			printf("%02X", (uint8_t)value);
			break;
		case 2:
			printf("%04X", (uint16_t)value);
			break;
		case 4:
			printf("%08X", (uint32_t)value);
			break;
		default:
			return;
	}
}



