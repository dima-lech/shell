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
					u_int64_t value, u_int64_t width);
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

	if (argc <= 1)
	{
		memCommandUsagePrint(argv[0]);
		return;
	}

	optind = 1;

	while ((c = getopt (argc, argv, "w:")) != -1)
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
    else if (strcmp(argv[optind], "w") == 0)
    {
    	operation = OP_MEM_WRITE;	
    }
    else
    {
    	memCommandUsagePrint(argv[0]);
    	return;
    }
    optind++;

    if (OP_MEM_READ == operation)
	{
		/* Expecting at least 1 additional non-optional argument */
		if ((argc - optind) < 1)
		{
			memCommandUsagePrint(argv[0]);
			return;
		}
	}
	else if (OP_MEM_WRITE == operation)
	{
		/* Expecting exactly 2 additional non-optional arguments */
		if ((argc - optind) != 2)
		{
			memCommandUsagePrint(argv[0]);
			return;
		}
	}
	else
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
    	value = memArgParse(argv[optind]);
    }
	else
    {
    	memCommandUsagePrint(argv[0]);
    	return;
    }


    switch (operation)
    {
    	case OP_MEM_READ:
			memRead(0x0, address, length, width);
    		break;
		case OP_MEM_WRITE:
    		memWrite(0x0, address, value, width);
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
	printf("\t%-16s%s w <address> <value> [-w <width>]\n",
			"memory write:", command);
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

	return (u_int64_t)strtol(arg, NULL, base);
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


	/* Print memory dump */
	for (i = 0; i < length; i++)
	{
		if ((0 == i) || ((address % 0x10) == 0))
		{
			if (i > 0)
			{
				charBuffer[charBufferIndex] = '\0';
				charBufferIndex = 0;
				printf("\t/%s/\n", charBuffer);
			}
			printf("%08lX :", addressPrint & 0xfffffffffffffff0lu);
		}

		/* Print first blanks */
		if (0 == i)
		{
			for (j = 0; j < ((address % 0x10) / 0x4); j++)
			{
				printf("         ");

				for (k = 0; k < 4; k++)
				{
					charBuffer[charBufferIndex++] = ' ';
				}
			}
		}

		value = *(volatile unsigned int*)address;
		printf(" %08X", value);
		for (k = 0; k < 4; k++)
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

		address += 0x4;
		addressPrint += 0x4;
	}

	/* Print last blanks */
	for (i = address; (i % 0x10) != 0x0; i += 4)
	{
		printf("         ");

		for (k = 0; k < 4; k++)
		{
			charBuffer[charBufferIndex++] = ' ';
		}
	}

	charBuffer[charBufferIndex] = '\0';
	printf("\t/%s/\n", charBuffer);
}


static void memWrite(u_int64_t addressPrint, u_int64_t address,
					u_int64_t value, u_int64_t width)
{

}


