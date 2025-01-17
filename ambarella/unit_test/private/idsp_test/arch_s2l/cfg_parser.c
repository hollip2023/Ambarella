/*
 *
 * idsp_test
 *
 * History:
 *	2015/09/10 - [Jingyang Qiu] Created this file
 *
 * Description :
 *	Load the tuned file which generated by Img tuning tool.
 *
 * Copyright (C) 2015 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CP_MAIN_KEY_NUMBER 16
#define CP_SUB_KEY_NUMBER 64
#define CP_CHAR_NUMBER 64
#define CP_LINE_LENGTH 1024

typedef struct{
	char key[CP_CHAR_NUMBER];
	int  value;
}cp_m_pair;

typedef struct{
	char key[CP_CHAR_NUMBER];
	char value[CP_CHAR_NUMBER];
}cp_s_pair;

static cp_m_pair main_pair[CP_MAIN_KEY_NUMBER];
static cp_s_pair sub_pair[CP_MAIN_KEY_NUMBER][CP_SUB_KEY_NUMBER];

int cp_reset_parser(void)
{
	int i;

	memset(main_pair, 0, sizeof(main_pair));
	memset(sub_pair, 0, sizeof(sub_pair));

	for(i=0; i<CP_MAIN_KEY_NUMBER;i++){
		main_pair[i].value = (int)sub_pair[i];
	}

	return 0;
}

int cp_add_key(char* main_key, char* sub_key, char* value)
{
	int i;
	cp_s_pair* pair;

	if(main_key == NULL || sub_key == NULL)
		return -1;

	for(i=0; i<CP_MAIN_KEY_NUMBER; i++){
		if(!strcmp(main_pair[i].key, main_key)|| strlen(main_pair[i].key)==0)
			break;
	}

	if(i == CP_MAIN_KEY_NUMBER){
		printf("main key out of range\n");
		return -1;
	}
	if(strlen(main_pair[i].key)==0)
		strcpy(main_pair[i].key, main_key);

	pair = (cp_s_pair*)main_pair[i].value;

	for(i=0; i<CP_SUB_KEY_NUMBER; i++){
		if(!strcmp(pair[i].key, sub_key) || strlen(pair[i].key)==0)
			break;
	}

	if(i == CP_SUB_KEY_NUMBER){
			printf("sub key out of range\n");
			return -1;
	}

	if(strlen(pair[i].key)==0)
		strcpy(pair[i].key, sub_key);

//	pair[i].value = value;
	strcpy(pair[i].value, value);

	return 0;

}

int cp_get_key(char* main_key, char* sub_key, char* value)
{
	int i;
	cp_s_pair* pair;

	for(i=0; i<CP_MAIN_KEY_NUMBER; i++){
		if(!strcmp(main_pair[i].key, main_key))
			break;
	}

	if(i == CP_MAIN_KEY_NUMBER){
		printf("main key not found\n");
		return -1;
	}

//	printf("main id %d\n", i);
	pair = (cp_s_pair*)main_pair[i].value;

	for(i=0; i<CP_SUB_KEY_NUMBER; i++){
//		printf("---%s\n", pair[i].key);
		if(!strcmp(pair[i].key, sub_key))
			break;
	}

	if(i == CP_SUB_KEY_NUMBER){
			printf("%s sub key %s not found\n", main_key, sub_key);
			return -1;
	}

//	*value = pair[i].value;
	strcpy(value, pair[i].value);

	return 0;
}

int cp_parse_file(char* file_name)
{
	FILE* file;
	char line[CP_LINE_LENGTH], mk[CP_CHAR_NUMBER], sk[CP_CHAR_NUMBER], v[CP_CHAR_NUMBER];
	int line_num = 0;
	int str_size = 0;
	int i=0, dot_pos = -1, space_pos = -1;

	file = fopen(file_name, "r");
	if(file == NULL){
		perror("fopen");
		return -1;
	}

	while(fgets(line, CP_LINE_LENGTH, file)!=NULL){
		line_num++;
		memset(mk, 0, CP_CHAR_NUMBER);
		memset(sk, 0, CP_CHAR_NUMBER);
		memset(v, 0, CP_CHAR_NUMBER);
		dot_pos = space_pos = -1;

		if(line[0]=='#' || line[0]=='\r' || line[0]=='\t' ||
			line[0]=='\n' || line[0]==' ')
			continue;
		str_size = strlen(line);
		line[str_size-1] = '\0';
		//printf("get line %d cnt %d: %s\n", line_num, strlen(line), line);
		for(i=0; i<str_size; i++){
			if(line[i]=='.') dot_pos = i;
			if(line[i]==' ' || line[i]=='\t') space_pos = i;
		}
		//printf("dot %d sp %d\n", dot_pos, space_pos);
		if(dot_pos==-1 || space_pos==-1){
			printf("warning: incomplete line %d\n", line_num);
			continue;
		}
		strncpy(mk, line, dot_pos);
		strncpy(sk, &line[dot_pos+1], space_pos-dot_pos-1);
		strncpy(v, &line[space_pos+1], str_size-space_pos);
		if(strlen(mk)==0 || strlen(sk)==0 || strlen(v)==0 ){
			printf("warning: incomplete key/value line %d\n", line_num);
			continue;
		}
		if(strlen(mk)>CP_CHAR_NUMBER || strlen(sk)>CP_CHAR_NUMBER || strlen(v)>CP_CHAR_NUMBER ){
			printf("warning: string size > %d\n", CP_CHAR_NUMBER);
			continue;
		}
		cp_add_key(mk, sk, v);
		//printf("key %s.%s v: %s\n", mk, sk, v);
	}
	fclose(file);

	return 0;
}

int cp_print_all_pair(void)
{
	int i, j;
	cp_s_pair* pair;

	for(i=0; i<CP_MAIN_KEY_NUMBER; i++){
		pair = (cp_s_pair*)main_pair[i].value;
		if(strlen(main_pair[i].key)!=0)
			printf("%s\n", main_pair[i].key);
		for(j=0; j<CP_SUB_KEY_NUMBER;j++){
			if(strlen(pair[j].key)!=0)
				printf("%s : %s\n", pair[j].key, pair[j].value);
		}
	}

	return 0;
}
