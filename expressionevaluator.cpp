#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

struct numbers{
	int number;
	numbers * next;
};

struct symbols{
	char symbol;
	symbols * next;
};

int power(int num1, int num2){
	if (num2 == 0)
		return 1;
	int ans = 1;
	for (int i = 1; i <= num2; i++){
		ans *= num1;
	}
	return ans;
}

bool findAnswer(numbers * main_n, symbols * syms_n, int * ans){
	char * precedence = "p/*+-";
	for (int i = 0; i < 5; i++){
		symbols * temp = syms_n, *prev_temp1 = syms_n;
		numbers * temp1 = main_n;
		while (temp1->next != NULL && temp != NULL){
			if (temp->symbol == precedence[i]){
				if (i == 0)
					temp1->number = power(temp1->number, temp1->next->number);
				else if (i == 1){
					if (temp1->next->number == 0){
						return false;
					}
					temp1->number = temp1->number / temp1->next->number;
				}
				else if (i == 2)
					temp1->number = temp1->number * temp1->next->number;
				else if (i == 3)
					temp1->number = temp1->number + temp1->next->number;
				else if (i == 4)
					temp1->number = temp1->number - temp1->next->number;
				temp1->next = temp1->next->next;
				//Removing the symbol
				if (temp == syms_n){
					temp = temp->next;
					syms_n = temp;
					prev_temp1 = temp;
				}
				else{
					prev_temp1->next = temp->next;
					temp = temp->next;
				}
			}
			else
			{
				if (temp != syms_n)
					prev_temp1 = prev_temp1->next;
				temp = temp->next;
				temp1 = temp1->next;
			}
		}
	}
	*ans = main_n->number;
	return true;
}

void printHashs(int * vals,int * checks){
	for (int i = 'a'; i <= 'z'; i++){
		if (checks[i] == 1)
			printf("%c  =>  %4d\n", i, vals[i]);
	}
	for (int i = 'A'; i <= 'Z'; i++){
		if (checks[i] == 1)
			printf("%c  =>  %4d\n", i, vals[i]);
	}
}

char * createExpression(char * buffer, int pos,int * map,int * exists,int line){
	/*
		b + 10    -c => 20+30-10
		removes spaces and substitutes variables in its place
	*/
	char * expression = NULL;
	int exp_length = 0;
	while (true){
		if (buffer[pos] == '\0'){
			expression[exp_length] = '\0';
			return expression;
		}
		//in case of variable
		if (('a' <= buffer[pos] && buffer[pos] <= 'z') || ('A' <= buffer[pos] && buffer[pos] <= 'Z')){
			if (exists[buffer[pos]] == 1){
				expression = (char *)realloc(expression, sizeof(char)*(exp_length + 1));
				int res = map[buffer[pos]];
				char * buff = (char *)malloc(sizeof(char)* 15);
				sprintf(buff, "%d", res);
				while (*buff != '\0'){
					expression = (char *)realloc(expression, sizeof(char)*(exp_length + 1));
					expression[exp_length] = *buff;
					exp_length++;
					buff++;
				}
			}
			else{
				printf("Ivalid variable name '%c' in line number %d", buffer[pos], line);
				return NULL;
			}
			pos++;
		}
		else if(buffer[pos]==' '){
			pos++;
		}
		else{
			expression = (char *)realloc(expression, sizeof(char)*(exp_length + 1));
			expression[exp_length] = buffer[pos];
			exp_length++;
			pos++;
		}
	}
}

void evaluate(){
	int * map = (int *)calloc(sizeof(int), 255);
	int * exists = (int *)calloc(sizeof(int), 255);
	char buffer[100];
	int line_no = 1;
	while (true){
		printf(">> ");
		fflush(stdin);
		gets(buffer);
		line_no++;
		if (strcmp(buffer, "%vars%") == 0){
			printHashs(map, exists);
		}
		else{
			if (buffer[0] == '@' && buffer[1] == '#')
				break;
			int i = 0; int bc = 0;
			while (buffer[i] != '\0'){
				int lhs;
				/*Spaces*/	while (buffer[i] == ' ')
					i++;
				if (('a' <= buffer[i] && buffer[i] <= 'z') || ('A' <= buffer[i] && buffer[i] <= 'Z')){
					lhs = buffer[i];
					exists[buffer[i]] = 1;
				}
				else{
					printf("Left hand must contain an Opearnd in %d line\n", line_no-1);
					return;
				}i++;
				/*Spaces*/	while (buffer[i] == ' ')
					i++;
				//Assignment opeartor
				if (buffer[i] == '\n' || buffer[i] == '\0')
					break;
				if (buffer[i] != '='){
					printf("Not found a assignment operator in %d line", line_no-1);
					return;
				}i++;
				/*Spaces*/ 	while (buffer[i] == ' ')
					i++;
				//Expression
				//a = 10
				//a = b + 10
				//a = 10 + b
				int num1 = 0; int ind = i;
				if ('0' <= buffer[i] && buffer[i] <= '9'){
					num1 = 0;
					while (true){
						num1 = num1 * 10 + buffer[i] - '0';
						i++;
						if (buffer[i] == ' ' || buffer[i] == '\0')
							break;
					}
				}
				if (buffer[i] == '\0'){
					exists[lhs] = 1;
					map[lhs] = num1;
				}
				else{
					char * solved_exp = createExpression(buffer, ind, map, exists, line_no);
					if (solved_exp == NULL)
						break;
					//Seperating numbers and symbols
					int k = 0;
					numbers * numbs = NULL; symbols * syms = NULL;
					while (solved_exp[k] != '\0'){
						if ('0' <= solved_exp[k] && solved_exp[k] <= '9'){
							num1 = 0;
							while ('0' <= solved_exp[k] && solved_exp[k] <= '9'){
								num1 = num1 * 10 + solved_exp[k] - '0';
								k++;
							}
							numbers * temp = (numbers *)malloc(sizeof(numbers));
							temp->number = num1;
							temp->next = NULL;
							if (numbs == NULL){
								numbs = temp;
							}
							else{
								numbers * reff = numbs;
								while (numbs->next != NULL){
									numbs = numbs->next;
								}
								numbs->next = temp;
							}
						}
						else{
							char sym;
							if (solved_exp[k] == '+' || solved_exp[k] == '-' || solved_exp[k] == '/' || solved_exp[k] == '%'){
								sym = solved_exp[k];
							}
							else if (solved_exp[k] == '*'){
								if (solved_exp[k + 1] == '*'){
									sym = 'p';
									k++;
								}
								else{
									sym = '*';
								}
							}k++;
							symbols * temp = (symbols *)malloc(sizeof(symbols));
							temp->symbol = sym;
							temp->next = NULL;
							if (syms == NULL){
								syms = temp;
							}
							else{
								numbers * reff = numbs;
								while (syms->next != NULL){
									syms = syms->next;
								}
								syms->next = temp;
							}
						}
					}
					//Passing structs to expression
					int ans = 0;
					findAnswer(numbs, syms, &ans);
					map[lhs] = ans;
					break;
				}
			}
		}
	}
}

int main(){
	printf("Enter @# to STOP\n");
	printf("Enter your expressions :\n");
	printf("========================\n");
	evaluate();
	getch();
	return 0;
}
