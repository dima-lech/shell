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


void memCommandUsagePrint(char * command)
{
	printf("Usage\n");
	printf("\t%-16s%s d <address> <length> [-w <width>]\n",
			"memory dump:", command);
	printf("\t%-16s%s w <address> <value> [-w <width>]\n",
			"memory write:", command);
}


unsigned long memArgParse(char * arg)
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

	return (unsigned long)strtol(arg, NULL, base);
}


void memCommand(int argc, char * argv[])
{
	int c;
	unsigned long length = 4;
	unsigned long width = 4;
	int opeartion = OP_UNKNOWN;
	unsigned long value = 0;
	unsigned long address = 0;

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

	/* Expecting exactly 3 non-optional arguments */
	if ((argc - optind) != 3)
	{
		memCommandUsagePrint(argv[0]);
		return;
	}

	/* Command argument */
	if (strcmp(argv[optind], "d") == 0)
    {
    	opeartion = OP_MEM_READ;
    }
    else if (strcmp(argv[optind], "w") == 0)
    {
    	opeartion = OP_MEM_WRITE;	
    }
    else
    {
    	memCommandUsagePrint(argv[0]);
    	return;
    }
    optind++;

    /* Address argument */
    address = memArgParse(argv[optind++]);

    /* Length/value argument */
    if (OP_MEM_READ == opeartion)
    {
    	length = memArgParse(argv[optind]);
    	if (0 == length)
    	{
    		length = 4;
    	}
    }
    else if (OP_MEM_WRITE == opeartion)
    {
    	value = memArgParse(argv[optind]);
    }
	else
    {
    	memCommandUsagePrint(argv[0]);
    	return;
    }


	/* === DEBUG === */
    switch (opeartion)
    {
    	case OP_MEM_READ:
    		printf ("READ\n");
			printf("address = 0x%lx\n", address);
			printf("length  = 0x%lx\n", length);
			printf("width   = %lu\n", width);
    		break;
		case OP_MEM_WRITE:
    		printf ("WRITE\n");
    		printf("address = 0x%lx\n", address);
			printf("value   = 0x%lx\n", value);
			printf("width   = %lu\n", width);
    		break;
		default:
    		printf ("UNKNOWN\n");
    		break;
    }
}

