#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


enum
{
	OP_MEM_READ,
	OP_MEM_WRITE,

	OP_UNKNOWN
};




void memCommand(int argc, char * argv[]);
static void memRead(u_int64_t addressPrint, u_int64_t address,
					u_int64_t length, u_int64_t width);
static void memWrite(u_int64_t addressPrint, u_int64_t address,
					u_int64_t value, u_int64_t width, int interactiveFlag);
static void memCommandUsagePrint(char * command);
static u_int64_t memArgParse(char * arg);




void memCommand(int argc, char * argv[])
{
	int c;
	int operation = OP_UNKNOWN;
	u_int64_t length = 4;
	u_int64_t width = 4;
	u_int64_t value = 0;
	u_int64_t address = 0;
	int interactiveFlag = 1;

	if (argc <= 1)
	{
		memCommandUsagePrint(argv[0]);
		return;
	}

	optind = 0;
	optarg = 0;

	while ((c = getopt (argc, argv, "w:i")) != -1)
	{
		switch (c)
		{
			case 'w':
				if (optarg)
				{
					width = memArgParse(optarg);
					if (width <= 0)
					{
						width = 4;
					}
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


static u_int64_t memArgParse(char * arg)
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

	return (u_int64_t)strtoul(arg, NULL, base);
}




static void memRead(u_int64_t addressPrint, u_int64_t address,
					u_int64_t length, u_int64_t width)
{
	u_int32_t value = 0x0;
	u_int32_t i;
	u_int32_t j;
	u_int32_t k;
	char charBuffer[17];
	u_int32_t charBufferIndex = 0;
	u_int64_t widthMask;
	u_int64_t widthMaskPrint;

	if ((width != 1) && (width != 2) && (width != 4))
	{
		printf("unsupported width!\n");
		return;
	}

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

		switch (width)
		{
			case 1:
				value = *(volatile u_int8_t*)address;
				printf(" %02X", value);
				break;
			case 2:
				value = *(volatile u_int16_t*)address;
				printf(" %04X", value);
				break;
			case 4:
				value = *(volatile u_int32_t*)address;
				printf(" %08X", value);
				break;
			default:
				printf("unsupported width!\n");
				return;
		}
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


static void memWrite(u_int64_t addressPrint, u_int64_t address,
					u_int64_t value, u_int64_t width, int interactiveFlag)
{
	u_int64_t widthMask;
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

		switch (width)
		{
			case 1:
				printf("%02X\n", (u_int8_t)value);
				*(volatile u_int8_t*)address = (u_int8_t)value;
				break;
			case 2:
				printf("%04X\n", (u_int16_t)value);
				*(volatile u_int16_t*)address = (u_int16_t)value;
				break;
			case 4:
				printf("%08X\n", (u_int32_t)value);
				*(volatile u_int32_t*)address = (u_int32_t)value;
				break;
			default:
				printf("unsupported width!\n");
				return;
		}
	}
	else
	{
		printf("(enter '.' or 'q' to exit)\n");

		while (!exitFlag)
		{
			skipFlag = 0;

			printf("%08lX : ", addressPrint);
			switch (width)
			{
				case 1:
					value = *(volatile u_int8_t*)address;
					printf("%02X", (u_int8_t)value);
					break;
				case 2:
					value = *(volatile u_int16_t*)address;
					printf("%04X", (u_int16_t)value);
					break;
				case 4:
					value = *(volatile u_int32_t*)address;
					printf("%08X", (u_int32_t)value);
					break;
				default:
					printf("unsupported width!\n");
					return;
			}
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
				switch (width)
				{
					case 1:
						*(volatile u_int8_t*)address = (u_int8_t)value;
						break;
					case 2:
						*(volatile u_int16_t*)address = (u_int16_t)value;
						break;
					case 4:
						*(volatile u_int32_t*)address = (u_int32_t)value;
						break;
					default:
						printf("unsupported width!\n");
						return;
				}
			}
			address += width;
			addressPrint += width;
		}
	}

}


