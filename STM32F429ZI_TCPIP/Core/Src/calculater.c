#include "calculater.h"

void convert_infix_to_postfix(char* infix, char* postfix);
void removec(char* source);
void Push(struct calculation** top_add, double x);
double Pop(struct calculation** top_add);
double evaluate(char* postfix);
void print_postfix(const char* postfix);
uint8_t button0_check();
uint8_t button1_check();
uint8_t button2_check();
uint8_t button3_check();
int count0 = 0;
int count1 = 0;
int count2 = 0;
int count3 = 0;


uint8_t button0_check() // calculator 초기화
{
	if(get_button(GPIOC, GPIO_PIN_8, BUTTON0) == BUTTON_PRESS)
	{
		count0++;
	}

	return 0;
}

uint8_t button1_check() // calculator backspace 기능
{
	if(get_button(GPIOC, GPIO_PIN_9, BUTTON1) == BUTTON_PRESS)
	{
		count1++;
	}

	return 0;
}

uint8_t button2_check() // '(' 담당
{

	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == GPIO_PIN_RESET)
    {
		count2++;
        return '(';
    }
    return 0;
}

uint8_t button3_check() //')' 담당
{

    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_RESET)
    {
    	count3++;
        return ')';
    }
    return 0;
}

void Push(struct calculation** top_add, double x) {
    // 새 노드 생성
    struct calculation *new_node = (struct calculation*)malloc(sizeof(struct calculation));
    //printf("Key Pushed: %c\n", x);
    // 메모리 할당 실패 시 처리
    if (new_node == NULL) {
        //printf("메모리 할당 실패\n");
        return;
    }
    // 새로운 값 저장
    new_node->data = x;
    new_node->next = *top_add; // 현재 스택의 top을 가리킴
    *top_add = new_node; // 스택 top을 새로운 노드로 갱신
}



double Pop(struct calculation **top_add) {
    // 스택이 비어 있는지 확인
	//printf("출력\n");
    if (*top_add == NULL) {
        //printf("스택이 비어 있음\n");
        return 0;  // 비어 있을 경우 0 반환
    }

    // pop할 데이터와 노드를 저장
    struct calculation *temp = *top_add;
    double pop_value = temp->data; // pop할 값 저장

    *top_add = temp->next; // top을 다음 노드로 이동
    free(temp); // 현재 top 메모리 해제

    return pop_value; // pop된 값 반환
}



double Top(struct calculation *top)
{
	char x;
	x = top->data;
	return(x);
}

void Push_char(struct calculation** top_add, char x)
{
	struct calculation *temp;
	temp = (struct calculation*)malloc(sizeof(struct calculation));

	if(temp !=NULL)
	{
		temp->data = x;
		temp->next = *top_add;
		*top_add = temp;
	}
	else
		printf("오버플로우 발생\n");
}

char Pop_char(struct calculation **top_add)
{
	char x;
	struct calculation *temp;

	temp = *top_add;
	x = temp->data;
	*top_add = temp->next;

	free(temp);

	return(x);
}

char Top_char(struct calculation *top)
{
	char x;
	x = top->data;
	return(x);
}

int pre(char op)
{
	switch(op)
	{
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	}
	return 0;
}


void convert_infix_to_postfix(char* infix, char* postfix)
{
	struct calculation* top = NULL; // 스택 초기화
	int i, j = 0; // 반복자 및 후위 표기법 배열 인덱스 초기화
	for (i = 0; infix[i]; i++)
	{ // 중위 표기법 문자열 반복
		if (isdigit(infix[i]) || infix[i] == '.')
		{ // 숫자 또는 소수점인 경우
			if (j > 0 && !isdigit(postfix[j - 1]) && postfix[j - 1] != '.' && postfix[j - 1] != ' ')
			{
				postfix[j++] = ' '; // 숫자와 연산자 사이에 공백 추가
			}
			postfix[j++] = infix[i]; // 후위 표기법 배열에 문자 추가
		}
		else if (infix[i] == '(')
		{ // 여는 괄호인 경우
			Push_char(&top, infix[i]); // 스택에 여는 괄호 추가
		}
		else if (infix[i] == ')')
		{ // 닫는 괄호인 경우
			while (top != NULL && Top_char(top) != '(')
			{ // 여는 괄호를 만날 때까지
				postfix[j++] = ' '; // 후위 표기법 배열에 공백 추가
				postfix[j++] = Pop_char(&top); // 스택에서 연산자를 꺼내 후위 표기법배열에 추가
			}
			Pop_char(&top); // 스택에서 여는 괄호 제거
		}
		else if (infix[i] != '=')
		{ // '=' 기호 무시
			postfix[j++] = ' '; // 후위 표기법 배열에 공백 추가
			while (top != NULL && pre(Top_char(top)) >= pre(infix[i]))
			{
				postfix[j++] = Pop_char(&top); // 스택에서 연산자를 꺼내 후위 표기법배열에 추가
				postfix[j++] = ' '; // 후위 표기법 배열에 공백 추가
			}
			Push_char(&top, infix[i]); // 스택에 연산자 추가
		}
	}
	while (top != NULL)
	{ // 스택이 비어있지 않으면
		postfix[j++] = ' '; // 후위 표기법 배열에 공백 추가
		postfix[j++] = Pop_char(&top); // 스택에서 연산자를 꺼내 후위 표기법 배열에 추가
	}
	postfix[j] = '\0'; // 후위 표기법 배열 문자열 종료
}

double evaluate(char* postfix)
{
    struct calculation* top = NULL; // 스택 초기화
    char* token = strtok(postfix, " "); // 문자열을 공백으로 분할
    char* endptr; // strtod에서 변환 종료 위치를 추적할 포인터

    while (token != NULL)
    { // 문자열 끝까지 반복
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        { // 숫자 또는 음수인 경우
            Push(&top, strtod(token, &endptr)); // atof 대신 strtod 사용
        }
        else
        { // 연산자인 경우
            double val2 = Pop(&top); // 스택에서 두 번째 피연산자 꺼냄
            double val1 = Pop(&top); // 스택에서 첫 번째 피연산자 꺼냄
            switch (token[0])
            { // 연산자에 따라 계산 수행
            case '+': Push(&top, val1 + val2); break;
            case '-': Push(&top, val1 - val2); break;
            case '*': Push(&top, val1 * val2); break;
            case '/': Push(&top, val1 / val2); break;
            default: printf("Invalid operator: %c\n", token[0]); exit(1); // 잘못된 연산자 처리
            }
        }
        token = strtok(NULL, " "); // 다음 토큰으로 이동
    }
    return Pop(&top); // 최종 결과 반환
}


void removec(char* source)
{
	char* i = source; // 반복자 초기화
	char* j = source; // 반복자 초기화
	while (*j != 0)
	{ // 문자열 끝까지 반복
		*i = *j++; // 공백이 아닌 문자를 앞으로 이동
		if (*i != ' ') // 공백이 아니면
			i++; // 반복자 증가
	}
	*i = 0; // 문자열 종료
}
