/**
 * bld/host_fixbstini.c
 *
 * Utility for fix bst.ini in runtime
 *
 * History:
 *    2014/05/09 - [Cao Rongrong] created file
 *
 * Copyright (C) 2012-2016, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "config.h"
#include "bsp.h"
#include "fio/partition.h"

#ifdef LIBXML_TREE_ENABLED

static xmlNode *find_node_by_name(xmlNode *root_node, xmlChar *name)
{
	xmlNode *node = root_node->xmlChildrenNode;

	while (node != NULL) {
		if (!xmlStrcmp(node->name, name))
			break;
		node = node->next;
	}

	return node;
}


/**
 * Simple example to parse a file called "file.xml",
 * walk down the DOM, and print the name of the
 * xml elements nodes.
 */
int main(int argc, char **argv)
{
	xmlDoc *doc = NULL;
	xmlNode *root_node = NULL, *node;
	xmlChar *xmlbuff, xmlstr[128];
	int fd, buffersize;
	int min_size, value;

	if (argc != 3 || strncmp("bld_", argv[2], 4)){
		fprintf(stderr, "error: invalid arguments\n");
		return -1;
	}

	/* this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	doc = xmlReadFile(argv[1], NULL, XML_PARSE_NOBLANKS);
	if (doc == NULL) {
		fprintf(stderr, "error: could not parse file %s\n", argv[1]);
		return -1;
	}

	/*Get the root element node */
	root_node = xmlDocGetRootElement(doc);

	/* BST must occupy one block or sector at least */
#if defined(CONFIG_FLASH_ERASE_256k)
	min_size = 256 * 1024;
#elif defined(CONFIG_FLASH_ERASE_128k)
	min_size = 128 * 1024;
#elif defined(CONFIG_FLASH_ERASE_64k)
	min_size = 64 * 1024;
#elif defined(CONFIG_FLASH_ERASE_16k)
	min_size = 16 * 1024;
#elif defined(CONFIG_FLASH_ERASE_4k)
	min_size = 4 * 1024;
#else
	min_size = 128 * 1024;
#endif

	/* fix BLD start address in media */
	value = AMBOOT_BST_SIZE > min_size ? AMBOOT_BST_SIZE : min_size;
	if ((value % min_size) != 0) {
		fprintf(stderr, "Invalid bst partition size\n");
		return -1;
	}
	snprintf(xmlstr, 128, "0x%08x", value);

	node = find_node_by_name(root_node, BAD_CAST "AMBOOT_BLD_MEDIA_START");
	if (node == NULL) {
		node = xmlNewChild(root_node, NULL, BAD_CAST "AMBOOT_BLD_MEDIA_START", NULL);
	}
	xmlSetProp(node, BAD_CAST "value", xmlstr);

	/* fix BLD size in media */
	fd = open(argv[2], O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Cannot open %s\n", argv[2]);
		return -1;
	}
	value = lseek(fd, 0, SEEK_END);
	snprintf(xmlstr, 128, "0x%08x", value);

	node = find_node_by_name(root_node, BAD_CAST "AMBOOT_BLD_MEDIA_SIZE");
	if (node == NULL) {
		node = xmlNewChild(root_node, NULL, BAD_CAST "AMBOOT_BLD_MEDIA_SIZE", NULL);
	}
	xmlSetProp(node, BAD_CAST "value", xmlstr);

	/* fix BLD start address in DRAM */
	value = AMBOOT_BLD_RAM_START;
	snprintf(xmlstr, 128, "0x%08x", value);

	node = find_node_by_name(root_node, BAD_CAST "AMBOOT_BLD_RAM_ADDRESS");
	if (node == NULL) {
		node = xmlNewChild(root_node, NULL, BAD_CAST "AMBOOT_BLD_RAM_ADDRESS", NULL);
	}
	xmlSetProp(node, BAD_CAST "value", xmlstr);

	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
	printf("%s", (char *) xmlbuff);

	/*free the document and associated memory. */
	xmlFree(xmlbuff);
	xmlFreeDoc(doc);

	/*
	*Free the global variables that may
	*have been allocated by the parser.
	*/
	xmlCleanupParser();

	return 0;
}
#else
int main(void) {
	fprintf(stderr, "Tree support not compiled in\n");
	exit(1);
}
#endif